#ifndef __SOLVER_H_INCLUDED
#define __SOLVER_H_INCLUDED 1

struct storyline;
struct result {
	double        time_assign_ids;
	double        time_copy_contexts;
	double        time_add_entity_contexts;
	double        time_build_context_groups;
	double        time_build_sat;
	double        time_solve_sat;
	double        time_sort_output;
	double        time_cleanup;
	
	unsigned int number_of_block_crossings;
	unsigned int number_of_context_groups;
};

struct storyline*
storyline_init();

void
storyline_add_entity(
	struct storyline*,
	unsigned int time_birth_inclusive,
	unsigned int time_death_inclusive,
	unsigned int non_unique_alias
);

void
storyline_add_context(
	struct storyline*,
	unsigned int time_starting_inclusive,
	unsigned int time_ending_inclusive,
	unsigned int size,
	unsigned int members[]
);

struct result*
storyline_solve(
	struct storyline*
);

void
storyline_release(
	struct storyline**
);

#endif
