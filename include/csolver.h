#ifndef CSOLVER_H_INCLUDED
#define CSOLVER_H_INCLUDED

#ifdef __cplusplus
extern "C" {
#endif

#include <stdarg.h>

typedef struct CSolver CSolver;

/* this struct enables calling functions like
 * 'LibCSolver.init()' etc. same effect as the
 * functions down below with different syntax */
struct clibrary{
	CSolver*	(*init)();
	CSolver*	(*reset)(CSolver *);
	void 		(*release)(CSolver *);
	int			(*solve)(CSolver *);

	void		(*clause)(CSolver *, int, ...);
	void		(*clausezt)(CSolver *, ...);
	void		(*aclause)(CSolver *, int, int *);
	void		(*aclausezt)(CSolver *, int *);

	int		(*val)(CSolver *, int);
};

extern const struct clibrary LibCSolver;

CSolver* csolver_init();
CSolver* csolver_reset(CSolver *);
void csolver_release(CSolver *);
int csolver_solve(CSolver *);

void csolver_clause(CSolver *, int, ...); /* takes number of literals in the clause */
void csolver_clausezt(CSolver *, ...); /* zero terminated */
void csolver_aclause(CSolver *, int, int *);
void csolver_aclausezt(CSolver *, int *);

int csolver_val(CSolver *, int);

#ifdef __cplusplus
}
#endif
#endif
