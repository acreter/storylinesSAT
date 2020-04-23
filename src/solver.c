#include "slsolver.h"
#include "structs.h"
#include "contextgroups.h"
#include "solution.h"

#include <acvector.h>
#include <csolver.h>
#include <stdlib.h>
#include <time.h>
#ifndef NDEBUG
#include <stdio.h>
#endif

#define SOLUTION_BUFFER 5

/* TODO: these macros are kind of hacky.
 * can we do better? */
#define SCALE(prev) (lits->number_of_literals * (layer - prev))
#define XVAR(i, j, prev) (nCharacters * i + j + x + SCALE(prev))
#define B(i, prev) (b + i + SCALE(prev))
#define C(i, prev) (c + i + SCALE(prev))
#define Q(i) (q + i + SCALE(0))
#define NOT(x) (-x)

CSolver * build(Literals*, acVector* contextgroups, unsigned long number_of_layers);
void add_layer(CSolver * s, Literals* lits, acVector* contextgroups, unsigned long layer);
void add_layer_independent_clauses(CSolver* s, Literals* lits, acVector* contextgroups, unsigned int number_of_layers);

void add_entity_variables(CSolver* s,  Literals* lits, unsigned long layer);
void add_context_variables(CSolver* s, Literals* lits, acVector* contextgroups, unsigned long layer);

Seperated_Entities* seperate_entities(acVector** contextgroup, Context* c);

acVector* sort_entities_at(unsigned long layer_index, unsigned int ec_map_index, CSolver*, Storylines*, unsigned long number_of_lits);


Result*
storylines_solve(Storylines* sl) {
	Result* r = malloc(sizeof (Result));
	r->time_spent_in_solver = 0;
	find_contextgroups(sl);
#ifndef NDEBUG
	debug_dump_storylines(sl);
#endif
	Literals* lits = literals_create(sl);

	int result = 0;
	CSolver * solver = 0;
	unsigned long number_of_layers = 0;
	clock_t start, end;

	while(!result) {
		number_of_layers += 1;
#ifndef NDEBUG
		printf("Testing with %ld layers...\n", number_of_layers);
#endif
		csolver_release(solver);
		solver = build(lits, sl->contextgroups, number_of_layers);
		start = clock();
		result = csolver_solve(solver);
		end = clock();
		r->time_spent_in_solver += ((double) (end - start)) / CLOCKS_PER_SEC;
	}
#ifndef NDEBUG
	printf("Solution found with %ld blockcrossings\n", number_of_layers - 1);
#endif

	r->number_of_block_crossings = number_of_layers - 1;

#ifndef NDEBUG
	debug_dump_solver_table(solver, lits, sl->entities->nElements, number_of_layers);
#endif

	/* TODO: put this into its own function */
	acVector* sol = acvector_create(sl->max_time + SOLUTION_BUFFER, sizeof (acVector*), 2);
	acVector* layer;
	unsigned int cg_index = 0;
	unsigned long layer_index = 0;
	unsigned long delta_layer_index = 0;

	enum Mode {normal, chaos, cleanup};
	enum Mode mode = normal;

	for (unsigned int t = 0; t <= sl->max_time; t += 1) {
		if (sl->events[t].incline) mode = cleanup;

		switch (mode) {
			case normal:
				layer = sort_entities_at(layer_index, t, solver, sl, lits->number_of_literals);
				break;
			case chaos:
				if (delta_layer_index < layer_index) delta_layer_index += 1;
				layer = sort_entities_at(delta_layer_index, t, solver, sl, lits->number_of_literals);
				break;
			case cleanup:
				for (unsigned long i = delta_layer_index; i + 1 < layer_index; i += 1) {
					layer = sort_entities_at(delta_layer_index, t, solver, sl, lits->number_of_literals);
					acvector_push_back(&sol, &layer);
				}

				layer = sort_entities_at(layer_index, t, solver, sl, lits->number_of_literals);
				mode = normal;
				break;
		}

		if (sl->events[t].decline) {
			mode = chaos;
			cg_index += 1;
			delta_layer_index = layer_index;
			while (csolver_val(solver, lits->q_starts_at + cg_index + lits->number_of_literals * layer_index) < 0) {
				layer_index += 1;
			}
		}

		acvector_push_back(&sol, &layer);
	}

	literals_release(&lits);
	csolver_release(solver);

#ifndef NDEBUG
	debug_dump_solution((Solution) sol);
#endif

	r->solution = (Solution) sol;
	return r;
}

CSolver *
build(Literals* lits, acVector* contextgroups, unsigned long number_of_layers) {
	CSolver* s = csolver_init();

	for(unsigned long i = 0; i < number_of_layers; i += 1){
		add_layer(s, lits, contextgroups, i);
	}

	add_layer_independent_clauses(s, lits, contextgroups, number_of_layers);

	return s;
}

void
add_layer(CSolver * s, Literals* lits, acVector* contextgroups, unsigned long layer) {
	add_entity_variables(s, lits, layer);
	add_context_variables(s, lits, contextgroups, layer);
	
	return;
}

void
add_entity_variables(CSolver * s, Literals* lits, unsigned long layer) {
	/* these are just some shortcuts */
	unsigned long nCharacters = lits->c_starts_at - lits->b_starts_at;
	unsigned long x = 1,
			b = lits->b_starts_at,
			c = lits->c_starts_at;

	/* TODO: this codeblock is copypasted from the old implementation
	 * and does not follow the coding style of the the new implementation:
	 * REFACTOR! */
	for(int i = 0; i < nCharacters; ++i){
		/* ~(b_i & c_i) */
		csolver_clausezt(s, NOT(B(i,0)), NOT(C(i,0)), 0);

		for(int j = 0; j < nCharacters; ++j){
			if(i != j){
				/* x_ij <-> ~x_ji */
				csolver_clausezt(s, XVAR(i,j,0), XVAR(j,i,0), 0);
				csolver_clausezt(s, NOT(XVAR(i,j,0)), NOT(XVAR(j,i,0)), 0);

				if(layer > 0){
					/* (b_i & c_j & x_ij) -> ~x_ij^r+1 */
					csolver_clausezt(s, NOT(B(i,1)), NOT(C(j,1)), NOT(XVAR(i,j,1)), NOT(XVAR(i,j,0)), 0);
					/* (x_ij & ~x_ij^r+1) -> b_i */
					csolver_clausezt(s, B(i,1), NOT(XVAR(i,j,1)), XVAR(i,j,0), 0);
					/* (x_ij & ~x_ij^r+1) -> c_j */
					csolver_clausezt(s, C(j,1), NOT(XVAR(i,j,1)), XVAR(i,j,0), 0);
				}
			}
			for(int k = 0; k < nCharacters; ++k){
				if(i != j && j != k && i != k){
					/* x_ij & x_jk -> x_ik */
					csolver_clausezt(s, NOT(XVAR(i,j,0)), NOT(XVAR(j,k,0)), XVAR(i,k,0), 0);
					/* x_ij & x_jk & b_i & b_k -> b_j */
					csolver_clausezt(s, NOT(XVAR(i,j,0)), NOT(XVAR(j,k,0)), NOT(B(i,0)), NOT(B(k,0)), B(j,0), 0);
					/* x_ij & x_jk & c_i & c_k -> c_j */
					csolver_clausezt(s, NOT(XVAR(i,j,0)), NOT(XVAR(j,k,0)), NOT(C(i,0)), NOT(C(k,0)), C(j,0), 0);
					/* x_ij & x_jk & b_i & c_k -> c_j | b_j */
					csolver_clausezt(s, NOT(XVAR(i,j,0)), NOT(XVAR(j,k,0)), NOT(B(i,0)), NOT(C(k,0)), B(j,0), C(j,0), 0);
				}
			}
		}
	}
	return;
}

void
add_context_variables(CSolver* s, Literals* lits, acVector* contextgroups, unsigned long layer) {
	unsigned int index_of_contextgroup = 0;

	unsigned long nCharacters = lits->c_starts_at - lits->b_starts_at;
	unsigned long x = 1,
			q = lits->q_starts_at;

	acVector*** cg;
	Seperated_Entities* se;
	Context** con;
	Entity** e1;
	Entity** e2;
	Entity** e3;

	ACVECTOR_FOREACH(cg, contextgroups) {
		ACVECTOR_FOREACH(con, *((*cg) + 1)) {
			se = seperate_entities(*cg, *con);
			ACVECTOR_FOREACH(e1, se->in_context) {
				ACVECTOR_FOREACH(e2, se->in_context) {
					if (!(*e1 == *e2)) {
						ACVECTOR_FOREACH(e3, se->not_in_context) {
							csolver_clausezt(s, NOT(Q(index_of_contextgroup)), XVAR((**e2).id,(**e3).id,0), XVAR((**e3).id,(**e1).id,0), 0);
						}
					}
				}
			}
			seperated_entities_release(&se);
		}
		index_of_contextgroup += 1;
	}

	return;
}

/* NOTE: maybe put this function in another file */
Seperated_Entities*
seperate_entities(acVector** contextgroup, Context* c) {
	Seperated_Entities* se = seperated_entities_create();

	Entity** e;
	Byte_t added = 0;
	ACVECTOR_FOREACH(e, *contextgroup) {
		for (unsigned int i = 0; i < c->size; i += 1) {
			if (c->members[i] == (**e).alias) {
				acvector_push_back(&(se->in_context), e);
				added = 1;
				break;
			}
		}

		if (!added) {
			acvector_push_back(&(se->not_in_context), e);
		}

		added = 0;
	}

	return se;
}

void
add_layer_independent_clauses(CSolver * s, Literals* lits, acVector* contextgroups, unsigned int number_of_layers) {
	int * clause = malloc((number_of_layers + 2) * sizeof(int));
	clause[number_of_layers] = 0;
	/* q_l^r | ... | q_l^nLayers */
	for(unsigned int i = 0; i < contextgroups->nElements; ++i){
		for(unsigned int j = 0; j < number_of_layers; ++j){
			clause[j] = lits->q_starts_at + i + lits->number_of_literals * j;
		}
		csolver_aclausezt(s, clause);
	}

	for(unsigned int i = 1; i < contextgroups->nElements; ++i){
		for(unsigned int j = 0; j < number_of_layers; ++j){
			for(unsigned int r = 0; r <= j; ++r){
				clause[r] = lits->q_starts_at + i - 1 + lits->number_of_literals * r;
			}
			clause[j+1] = -(lits->q_starts_at + i + lits->number_of_literals * j);
			clause[j+2] = 0;
			csolver_aclausezt(s, clause);
		}
	}
	free(clause);
}

acVector*
sort_entities_at(unsigned long layer_index, unsigned int ec_map_index, CSolver* s, Storylines* sl, unsigned long number_of_lits) {
	Entity** e = 0;
	Entity** e1 = 0;
	unsigned int index = 0;
	acVector* result = acvector_create(sl->entities->nElements * 2, sizeof(Entity*), 2);

	ACVECTOR_FOREACH(e, sl->ec_map[ec_map_index * 2]) {
		index = 0;
		ACVECTOR_FOREACH(e1, result) {
			/* NOTE: is this true?
			 * if e under e1 */
			if (csolver_val(s, (sl->entities->nElements * (**e1).id + (**e).id + 1) + number_of_lits * layer_index) > 0) {
				break;
			}
			index += 1;
		}

		acvector_insert(&result, index, e);
	}

#ifndef NDEBUG
	printf("Entity sort report:\n");
	printf("Sorted entry %d with layer %d\nResult:", ec_map_index, layer_index);
	ACVECTOR_FOREACH(e, result) {
		printf(" %d", (**e).alias);
	}
	printf("\n\n");
#endif
	result = solution_add_seperators_single_layer(result, sl->ec_map[ec_map_index * 2 + 1], &(sl->dummy));

	return result;
}
