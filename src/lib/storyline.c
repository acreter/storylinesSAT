#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <acvector.h>
#include <context.h>
#include <storyline.h>
#include <solver.h>
#include <entity.h>
#include <color.h>
#include <time.h>
#include <csolver.h>

#ifndef NCHECK
#include <util.h>
#endif

struct storyline*
storyline_init() {
	struct storyline* s = malloc(sizeof (struct storyline));
	s->entities         = acvector_create(16, sizeof (entity_t));
	s->map              = malloc(sizeof (context_map_t));
	s->map->t_max       = 0;
	s->map->events      = malloc(1);

	return s;
}

void
storyline_release(
	struct storyline** instance
) {
	acvector_destroy(&((*instance)->entities));

	for (
		unsigned int x=0;
		x < (*instance)->map->t_max;
		x+=1
	) {
		acvector_destroy((*instance)->map->timeslots + x);
	}

	free((*instance)->map->events);
	free((*instance)->map);
	free(*instance);
	*instance = NULL;

	return;
}

struct result*
storyline_solve(
	struct storyline* instance
) {
	acVector* cg = NULL;
	acVector* inter = NULL;
	context_map_t* cm = NULL;
	struct result* result = malloc(sizeof (struct result));
	clock_t start, end;

#ifndef NVERBOSE
	printf("Assinging character IDs:");
	fflush(stdout);
#endif
	start = clock();

	for (
		entity_t* x = acvector_get_first(&(instance->entities));
		x <= (entity_t*) acvector_get_last(&(instance->entities));
		x+=1
	) {
		x->id = x - (entity_t*) acvector_get_first(&(instance->entities));
	}

	for (
		acVector** a = instance->map->timeslots;
		a < instance->map->timeslots + instance->map->t_max;
		a+=1
	) {
		unsigned int t_cur = a - instance->map->timeslots;
		for (
			unsigned int* x = acvector_get_first(a);
			x < (unsigned int*) acvector_get_last(a);
			x += *x * 2 + 3
		) {
			for (
				unsigned int* p = x + *x + 3;
				p < x + *x * 2 + 3;
				p += 1
			) {
				for (
					entity_t* e = acvector_get_first(&(instance->entities));
					e <= (entity_t*) acvector_get_last(&(instance->entities));
					e+=1
				) {
					if (e->alias == *p && e->t_birth <= t_cur && e->t_death >= t_cur) {
						*(p - (*x)) = e->id;
						break;
					}
				}
			}
		}
	}

	end = clock();
	result->time_assign_ids = ((double) end - start) / CLOCKS_PER_SEC;
#ifndef NVERBOSE
	printf(ANSI_COLOR_GREEN " done" ANSI_COLOR_RESET "\n");
	printf("Copying contexts:");
	fflush(stdout);
#endif
	start = clock();

	cm = malloc(sizeof (context_map_t) + sizeof (acVector*) * instance->map->t_max);
	memcpy(cm, instance->map, sizeof (context_map_t));

	for (
		unsigned int x = 0;
		x < cm->t_max;
		x += 1
	) {
		cm->timeslots[x] = acvector_copy(instance->map->timeslots + x);
	}

	end = clock();
	result->time_copy_contexts = ((double) end - start) / CLOCKS_PER_SEC;
#ifndef NVERBOSE
	printf(ANSI_COLOR_GREEN " done" ANSI_COLOR_RESET "\n");
	printf("Adding entities as single member contexts:");
	fflush(stdout);
#endif
	start = clock();

	{
	unsigned int a[5] = {1,0,0,0,0};
	unsigned int found = 0;
	for (
		entity_t* e = acvector_get_first(&(instance->entities));
		e <= (entity_t*) acvector_get_last(&(instance->entities));
		e += 1
	) {
		for (
			unsigned int x = e->t_birth;
			x <= e->t_death;
			x += 1
		) {
			found = 0;
			for (
				unsigned int* q = acvector_get_first(cm->timeslots + x);
				q <= (unsigned int*) acvector_get_last(cm->timeslots + x);
				q += *q * 2 + 3
			) {
				for (
					unsigned int* w = q + *q + 3;
					w < q + *q * 2 + 3;
					w += 1
				) {
					if (e->alias == *w) {
						found += 1;
#ifndef NCHECK
						if (found >= 2) {
							fprintf(stderr,
								ANSI_COLOR_RED " ERROR" ANSI_COLOR_RESET \
									"\nAn entity has two contexts at the same time:\n" \
									"time = %i\t character = %i\n", x, e->alias
							);
							for (unsigned int x = 0;
								x < instance->map->t_max;
								x+=1
							) {
								acvector_destroy(cm->timeslots + x);
							}
							free(cm);
							cm = NULL;
							die(instance);
						}
#endif
#ifdef NCHECK
						break;
#endif
					} else continue;
				}
#ifdef NCHECK
				if (found) break;
#endif
			}

			if (!found) {
				a[1] = e->t_birth;
				a[2] = e->t_death;
				a[3] = e->id;
				a[4] = e->alias;
				acvector_append(cm->timeslots + x, a, 5);
			} else continue;
		}
	}
	}

	end = clock();
	result->time_add_entity_contexts = ((double) end - start) / CLOCKS_PER_SEC;
#ifndef NVERBOSE
	printf(ANSI_COLOR_GREEN " done" ANSI_COLOR_RESET "\n");
	printf("Bulding context groups:");
	fflush(stdout);
#endif
	start = clock();

	cg = acvector_create(32, sizeof(acVector**));

	{
	unsigned char last_meeting_started_f = 1;
	unsigned char last_meeting_ended_at = 0;
	acVector** v = NULL;

	for (
		unsigned int x = 0;
		x < cm->t_max;
		x += 1
	) {
		if (cm->events[x].starting) {
			if (!last_meeting_started_f) {
				cm->events[x].incline = 1;
				cm->events[last_meeting_ended_at].decline = 1;
			}
			last_meeting_started_f = 1;
		}
		if (cm->events[x].ending) {
			if (last_meeting_started_f) {
				v = cm->timeslots + x;
				acvector_append(&cg, &v, 1);
			}
			last_meeting_started_f = 0;
			last_meeting_ended_at = x;
		}
	}
	}

	{
	unsigned int time_of_next_cg;
	inter = acvector_create(cg->number_of_elements, sizeof(acVector*));
	acVector* v = NULL;
	acVector** vec = NULL;

	for (
		acVector*** x = acvector_get_first(&cg);
		x < (acVector***) acvector_get_last(&cg);
		x += 1
	) {
		time_of_next_cg = x[1] - cm->timeslots;
		v = acvector_create(8, sizeof (unsigned int));
		acvector_append(&inter, &v, 1);
		vec = acvector_get_last(&inter);
		for (
			unsigned int* q = acvector_get_first(*x);
			q <= (unsigned int*) acvector_get_last(*x);
			q += *q * 2 + 3
		) {
			if (q[2] >= time_of_next_cg) {
				acvector_append(vec, q, *q * 2 + 3);
			} else continue;
		}
	}
	}

	result->number_of_context_groups = cg->number_of_elements * 2 - 1;

	end = clock();
	result->time_build_context_groups = ((double) end - start) / CLOCKS_PER_SEC;
#ifndef NVERBOSE
		printf(ANSI_COLOR_GREEN " done" ANSI_COLOR_RESET "\n");
#endif

	{
	unsigned int sat = 0;
	unsigned int n_layers = 0;
	CSolver* solver = NULL;

	unsigned int n_entities = instance->entities->number_of_elements;
	unsigned int x = 1;
	unsigned int b = n_entities * n_entities + 1;
	unsigned int c = b + n_entities;
	unsigned int q = c + n_entities;
	unsigned int inter_start = q + cg->number_of_elements;
	unsigned int n_lits = inter_start + inter->number_of_elements;

	result->time_build_sat = 0;

	while (!sat) {
		n_layers += 1;
		csolver_release(solver);

#ifndef NVERBOSE
		printf("Building instance of SAT with %i permutation(s):", n_layers);
		fflush(stdout);
#endif
		start = clock();

		solver = csolver_init();
		for (
			unsigned int l = 0;
			l < n_layers;
			l += 1
		) {
			for (
				unsigned int i = 0;
				i < n_entities;
				i += 1
			) {
				csolver_clausezt(solver,
					-(b + i + n_lits * l),
					-(c + i + n_lits * l), 0
				);
				for (
					unsigned int j = 0;
					j < n_entities;
					j += 1
				) {
					if (i == j) continue;
					csolver_clausezt(solver,
						n_entities * i + j + x + n_lits * l,
						n_entities * j + i + x + n_lits * l, 0
					);
					csolver_clausezt(solver,
						-(n_entities * i + j + x + n_lits * l),
						-(n_entities * j + i + x + n_lits * l), 0
					);
					if (l > 0) {
						csolver_clausezt(solver,
							-(b + i + n_lits * (l - 1)),
							-(c + j + n_lits * (l - 1)),
							-(n_entities * i + j + x + n_lits * l),
							-(n_entities * i + j + x + n_lits * (l - 1)), 0
						);
						csolver_clausezt(solver,
							b + i + n_lits * (l - 1),
							-(n_entities * i + j + x + n_lits * (l - 1)),
							n_entities * i + j + x + n_lits * l, 0
						);
						csolver_clausezt(solver,
							c + j + n_lits * (l - 1),
							-(n_entities * i + j + x + n_lits * (l - 1)),
							n_entities * i + j + x + n_lits * l, 0
						);
					}
					for (
						unsigned int k = 0;
						k < n_entities;
						k += 1
					) {
						if (i == j || i == k || j == k) continue;
						csolver_clausezt(solver,
							-(n_entities * i + j + x + n_lits * l),
							-(n_entities * j + k + x + n_lits * l),
							n_entities * i + k + x + n_lits * l, 0
						);
						csolver_clausezt(solver,
							-(n_entities * i + j + x + n_lits * l),
							-(n_entities * j + k + x + n_lits * l),
							-(b + i + n_lits * l),
							-(b + k + n_lits * l),
							b + j + n_lits * l, 0
						);
						csolver_clausezt(solver,
							-(n_entities * i + j + x + n_lits * l),
							-(n_entities * j + k + x + n_lits * l),
							-(c + i + n_lits * l),
							-(c + k + n_lits * l),
							c + j + n_lits * l, 0
						);
						csolver_clausezt(solver,
							-(n_entities * i + j + x + n_lits * l),
							-(n_entities * j + k + x + n_lits * l),
							-(b + i + n_lits * l),
							-(c + k + n_lits * l),
							b + j + n_lits * l,
							c + j + n_lits * l, 0
						);
					}
				}
			}
			for (
				acVector*** group = acvector_get_first(&cg);
				group <= (acVector***) acvector_get_last(&cg);
				group += 1
			) {
				for (
					unsigned int* g = acvector_get_first(*group);
					g <= (unsigned int*) acvector_get_last(*group);
					g += *g * 2 + 3
				) {
					if (*g <= 1) continue;
					for (
						unsigned int* w = g + 3;
						w < g + *g + 3;
						w += 1
					) {
						for (
							unsigned int* y = g + 3;
							y < g + *g + 3;
							y += 1
						) {
							if (w == y) continue;
							for (
								unsigned int* u = acvector_get_first(*group);
								u <= (unsigned int*) acvector_get_last(*group);
								u += *u * 2 + 3
							) {
								if (u == g) continue;
								for (
									unsigned int* z = u + 3;
									z < u + *u + 3;
									z += 1
								) {
									csolver_clausezt(solver,
										-(q + (group - (acVector***) acvector_get_first(&cg)) + n_lits * l),
										n_entities * *y + *z + x + n_lits * l,
										n_entities * *z + *w + x + n_lits * l, 0
									);
								}
							}
						}
					}
				}
			}
			for (
				acVector** group = acvector_get_first(&inter);
				group <= (acVector**) acvector_get_last(&inter);
				group += 1
			) {
				for (
					unsigned int* g = acvector_get_first(group);
					g <= (unsigned int*) acvector_get_last(group);
					g += *g * 2 + 3
				) {
					if (*g <= 1) continue;
					for (
						unsigned int* w = g + 3;
						w < g + *g + 3;
						w += 1
					) {
						for (
							unsigned int* y = g + 3;
							y < g + *g + 3;
							y += 1
						) {
							if (w == y) continue;
							for (
								unsigned int* u = acvector_get_first(group);
								u <= (unsigned int*) acvector_get_last(group);
								u += *u * 2 + 3
							) {
								if (u == g) continue;
								for (
									unsigned int* z = u + 3;
									z < u + *u + 3;
									z += 1
								) {
									csolver_clausezt(solver,
										-(inter_start + (group - (acVector**) acvector_get_first(&inter)) + n_lits * l),
										n_entities * *y + *z + x + n_lits * l,
										n_entities * *z + *w + x + n_lits * l, 0
									);
								}
							}
						}
					}
				}
			}
		}

		unsigned int* clause = malloc((n_layers + 2) * sizeof (unsigned int));
		clause[n_layers] = 0;
		for (
			unsigned int g = 0;
			g < cg->number_of_elements;
			g += 1
		) {
			for (
				unsigned int l = 0;
				l < n_layers;
				l += 1
			) {
				clause[l] = q + g + n_lits * l;
			}
			csolver_aclausezt(solver, clause);
		}
		for (
			unsigned int g = 1;
			g < cg->number_of_elements;
			g += 1
		) {
			for (
				unsigned int l = 0;
				l < n_layers;
				l += 1
			) {
				for (
					unsigned int r = 0;
					r <= l;
					r += 1
				) {
					clause[r] = q + g - 1 + n_lits * r;
				}
				clause[l+1] = -(q + g + n_lits * l);
				clause[l+2] = 0;
				csolver_aclausezt(solver, clause);
			}
		}
		free(clause);
		if (n_layers >= 3) {
		for (
			unsigned int g = 0;
			g < cg->number_of_elements - 1;
			g += 1
		) {
			for (
				unsigned int l = 0;
				l < n_layers - 2;
				l += 1
			) {
				for (
					unsigned int r = l + 2;
					r < n_layers;
					r += 1
				) {
					for (
						unsigned int t = l + 1;
						t < r;
						t += 1
					) {
						csolver_clausezt(solver,
							-(q + g + n_lits * l),
							-(q + g + 1 + n_lits * r),
							inter_start + g + n_lits * t, 0);
					}
				}
			}
		}
		}

		end = clock();
		result->time_build_sat += ((double) end - start) / CLOCKS_PER_SEC;
#ifndef NVERBOSE
		printf(ANSI_COLOR_GREEN " done " ANSI_COLOR_RESET);
		printf("Solving:");
		fflush(stdout);
#endif

		start = clock();
		sat = csolver_solve(solver);

		end = clock();
		result->time_solve_sat += ((double) end - start) / CLOCKS_PER_SEC;
#ifndef NVERBOSE
		printf(ANSI_COLOR_GREEN " done" ANSI_COLOR_RESET "\n");
#endif

	}

	result->number_of_block_crossings = n_layers - 1;

#ifndef NVERBOSE
		printf("Sorting output:");
		fflush(stdout);
#endif
	start = clock();

	csolver_release(solver);
	}

	end = clock();
	result->time_sort_output += ((double) end - start) / CLOCKS_PER_SEC;
#ifndef NVERBOSE
	printf(ANSI_COLOR_RED " NOT YET IMPLEMENTED" ANSI_COLOR_RESET "\n");
	printf("Freeing copy of contexts:");
	fflush(stdout);
#endif
	start = clock();

	for (unsigned int x = 0;
		x < instance->map->t_max;
		x+=1
	) {
		acvector_destroy(cm->timeslots + x);
	}

#ifndef NVERBOSE
	printf(ANSI_COLOR_GREEN " done" ANSI_COLOR_RESET "\n");
	printf("Freeing context groups:");
	fflush(stdout);
#endif

	for (
		acVector** x = acvector_get_first(&inter);
		x <= (acVector**) acvector_get_last(&inter);
		x += 1
	) {
		acvector_destroy(x);
	}
	acvector_destroy(&inter);
	acvector_destroy(&cg);
	free(cm);
	cm = NULL;

	end = clock();
	result->time_cleanup = ((double) end - start) / CLOCKS_PER_SEC;
#ifndef NVERBOSE
	printf(ANSI_COLOR_GREEN " done" ANSI_COLOR_RESET "\n");
#endif

	return result;
}
