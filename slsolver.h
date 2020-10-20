#ifndef __SLSOLVER_H_INCLUDED
#define __SLSOLVER_H_INCLUDED

typedef unsigned long Time_t;
typedef unsigned char Byte_t;
typedef struct Storylines Storylines;
typedef struct Result Result;

/* the solution is a pointer to an acVector (https://github.com/acreter/acvector) */
typedef void* Solution;

struct Result {
	double time_spent_in_solver; /* time in seconds */
	unsigned long number_of_block_crossings;
	unsigned long number_of_context_groups;
	Solution solution;
};

/* TODO: setting the max time here can be avoided */
/* time always starts at 0 */
Storylines *	storylines_init(Time_t max_time);
int				storylines_add_entity(Storylines *, Time_t time_starting, Time_t time_ending, unsigned int alias);
int				storylines_add_context(Storylines *, Time_t time_starting, Time_t time_ending, unsigned int size, unsigned int members[]);
Result *		storylines_solve(Storylines *);

/* zero-terminated solution matrix */
long **			storylines_solution_convert(Storylines *, Solution);

void			storylines_release(Storylines **);
void			storylines_result_release(Result **);
void 			storylines_converted_release(long*** converted);
#endif
