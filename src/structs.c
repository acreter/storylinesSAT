#include "slsolver.h"
#include "structs.h"
#include <acvector.h>

#include <stdlib.h>

Storylines *
storylines_init(Time_t max_time) {
	Storylines * sl = malloc(sizeof (Storylines) + sizeof (Event) * (max_time + 1));
	sl->entities = acvector_create(MAX_NUMBER_OF_ENTITIES, sizeof (Entity));
	sl->contexts = acvector_create(MAX_NUMBER_OF_CONTEXTS, sizeof (Context));
	sl->ec_map = malloc(sizeof (acVector*) * (max_time + 1) * 2);
	sl->contextgroups = acvector_create(MAX_NUMBER_OF_CONTEXTGROUPS, sizeof(acVector**));
	sl->max_time = max_time;
	sl->prepared = 0;

	sl->dummy.id = 0;
	sl->dummy.alias = 0;
	sl->dummy.start = 0;
	sl->dummy.end = 0;

	for (Time_t t = 0; t <= sl->max_time; t += 1) {
		(sl->ec_map)[t*2] = acvector_create(8, sizeof (Entity*));
		(sl->ec_map)[t*2+1] = acvector_create(4, sizeof (Context*));
		sl->events[t].starting = 0;
		sl->events[t].ending   = 0;
		sl->events[t].decline  = 0;
		sl->events[t].incline  = 0;
	}

	return sl;
}

void
storylines_release(Storylines** sl) {
	Context* c;
	ACVECTOR_FOREACH(c, (**sl).contexts) {
		free(c->members);
	}

	for (Time_t t = 0; t <= (**sl).max_time; t += 1) {
		 acvector_destroy(((**sl).ec_map) + t*2);
		 acvector_destroy(((**sl).ec_map) + t*2 + 1);
	}

	acvector_destroy(&((**sl).entities));
	acvector_destroy(&((**sl).contexts));
	acvector_destroy(&((**sl).contextgroups));
	free((**sl).ec_map);
	free(*sl);
	*sl = 0;

	return;
}

/* these are the literals for just ONE layer */
Literals*
literals_create(Storylines* sl) {
	Literals* lits = malloc(sizeof (Literals));

	/* x starts at 0 */	
	lits->b_starts_at = (sl->entities->number_of_elements * sl->entities->number_of_elements) + 1;
	lits->c_starts_at = lits->b_starts_at + sl->entities->number_of_elements;
	lits->q_starts_at = lits->c_starts_at + sl->entities->number_of_elements;

	lits->number_of_literals = lits->q_starts_at + sl->contextgroups->number_of_elements;

	return lits;
}

void
literals_release(Literals** l) {
	free(*l);
	*l = 0;

	return;
}

Seperated_Entities*
seperated_entities_create() {
	Seperated_Entities* se = malloc(sizeof (Seperated_Entities));
	se->in_context = acvector_create(8, sizeof(Entity*));
	se->not_in_context = acvector_create(8, sizeof(Entity*));

	return se;
}

void
seperated_entities_release(Seperated_Entities** se) {
	acvector_destroy(&((**se).in_context));
	acvector_destroy(&((**se).not_in_context));
	free(*se);
	*se = 0;

	return;
}

void
seperated_entities_reset(Seperated_Entities* se) {
	se->in_context->number_of_elements = 0;
	se->not_in_context->number_of_elements = 0;

	return;
}

void
storylines_result_release(Result** result) {
	Result* r = *result;
	acVector** layer;
	acVector* sol = (acVector*) r->solution;
	ACVECTOR_FOREACH(layer, sol) {
		acvector_destroy(layer);
	}

	acvector_destroy(&sol);
	r->solution = 0;
	free(*result);
	*result = 0;

	return;
}

#ifndef NDEBUG
#include <stdio.h>
void
debug_dump_storylines(Storylines* sl) {
	printf("### Storylines dump\n");
	printf("Prepared: %d\n", sl->prepared);
	printf("Max Time: %d\n", sl->max_time);
	printf("Dummy:\n");
	debug_dump_entity(&(sl->dummy));
	printf("Events:\n");
	for (Time_t t = 0; t <= sl->max_time; t += 1) {
		debug_dump_event(sl->events + t);
	}
	printf("\n");

	printf("Entities:\n");
	Entity* e;
	ACVECTOR_FOREACH(e, sl->entities) {
		debug_dump_entity(e);
	}

	printf("Contexts:\n");
	Context* c;
	ACVECTOR_FOREACH(c, sl->contexts) {
		debug_dump_context(c);
	}

	debug_dump_ec_map(sl, sl->ec_map);
	debug_dump_contextgroups(sl->contextgroups);

	printf("### End of Storylines dump\n\n");
	return;
}

void
debug_dump_event(Event* e) {
	printf("Start: %d\tEnd: %d\tDecline: %d\tIncline: %d\n", e->starting, e->ending, e->decline, e->incline);

	return;
}

void
debug_dump_entity(Entity* e) {
	printf("Entity %d:\n", e->id);
	printf("\tAlias %d\n", e->alias);
	printf("\tStart %d\n", e->start);
	printf("\tEnd %d\n\n", e->end);

	return;
}

void
debug_dump_context(Context* c) {
	printf("Context:\n");
	printf("\tSize %d\n", c->size);
	printf("\tStart %d\n", c->start);
	printf("\tEnd %d\n", c->end);
	printf("\tMembers:");

	for (unsigned int i = 0; i < c->size; i += 1) {
		printf(" %d", c->members[i]);
	}

	printf("\n\n");

	return;
}

void
debug_dump_ec_map(Storylines* sl, acVector** map) {

	printf("ec_map:\n");
	for (Time_t t = 0; t <= sl->max_time; t += 1) {
		printf("Entry %d:\n", t);
		debug_dump_ec_map_entity(map + 2 * t);
		debug_dump_ec_map_context(map + 2 * t + 1);
		printf("\n\n");
	}
	printf("\n\n");

	return;
}

void
debug_dump_contextgroups(acVector* cg) {
	if (!cg->number_of_elements) {
		printf("No contextgroups found yet!\n\n");
	} else {
		printf("contextgroups:\n");
		acVector*** v;
		ACVECTOR_FOREACH(v, cg) {
			debug_dump_ec_map_context(*v + 1);
		}
	}
	printf("\n\n");

	return;
}

void debug_dump_ec_map_entity(acVector** v) {
	acVector* entry = *v;
	Entity** e;
	ACVECTOR_FOREACH(e, entry) {
		debug_dump_entity(*e);
	}

	return;
}

void debug_dump_ec_map_context(acVector** v) {
	acVector* entry = *v;
	Context** c;
	ACVECTOR_FOREACH(c, entry) {
		debug_dump_context(*c);
	}

	return;
}

void
debug_dump_seperated_entities(Seperated_Entities* se) {
	Entity** e;
	printf("In Context:\n");
	ACVECTOR_FOREACH(e, se->in_context) {
		debug_dump_entity(*e);
	}
	printf("\n\n");

	printf("Not In Context:\n");
	ACVECTOR_FOREACH(e, se->not_in_context) {
		debug_dump_entity(*e);
	}
	printf("\n\n");
}

void
debug_dump_solution(Solution s) {
	acVector* v = (acVector*) s;

	acVector** sol;
	Entity** e;
	unsigned int counter = 0;
	ACVECTOR_FOREACH(sol, v) {
		printf("Solution at time %d:\n", counter);
		ACVECTOR_FOREACH(e, *sol) {
			debug_dump_entity(*e);
		}
		printf("\n\n");
		counter += 1;
	}

	return;
}

#include <csolver.h>
void
debug_dump_solver_table(CSolver* s, Literals* l, unsigned int number_of_entities, unsigned int number_of_layers) {
	printf("solver table:\n");
	for (unsigned int k = 0; k < number_of_layers; k += 1) {
		printf("layer %d\n", k);
		printf("x:\nread row over column\n");
		for (unsigned int i = 0; i < number_of_entities; i += 1) {
			for (unsigned int j = 0; j < number_of_entities; j += 1) {
				printf(" %d", csolver_val(s, (1 + i * number_of_entities + j) + l->number_of_literals * k));
			}
			printf("\n");
		}

		printf("\nb:\n");
		for (unsigned int i = l->b_starts_at; i < l->c_starts_at; i += 1) {
			printf(" %d", csolver_val(s, i + l->number_of_literals * k));
		}

		printf("\n\nc:\n");
		for (unsigned int i = l->c_starts_at; i < l->q_starts_at; i += 1) {
			printf(" %d", csolver_val(s, i + l->number_of_literals * k));
		}

		printf("\n\nq:\n");
		for (unsigned int i = l->q_starts_at; i < l->number_of_literals; i += 1) {
			printf(" %d", csolver_val(s, i + l->number_of_literals * k));
		}
		printf("\n");
	}
	printf("\n\n");

	return;
}
#endif

Context*
find_context_of(Entity* e, acVector* v) {
	Context** c;
	ACVECTOR_FOREACH(c, v) {
		for (unsigned int i = 0; i < (**c).size; i += 1) {
			if (e->alias == (**c).members[i]) return *c;
		}
	}

	return 0;
}
