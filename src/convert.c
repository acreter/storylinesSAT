#include "slsolver.h"
#include "structs.h"
#include <acvector.h>
#include <stdlib.h>

long**
storylines_solution_convert(Storylines* sl, Solution s) {
	acVector* v = (acVector*) s;
	long** converted = malloc((v->nElements + 1) * sizeof (long*));
	converted[v->nElements] = 0;

	acVector** sol;
	Entity** e;
	unsigned int counter = 0;
	unsigned int counter1 = 0;
	ACVECTOR_FOREACH(sol, v) {
		counter1 = 0;
		converted[counter] = malloc(((**sol).nElements + 1) * sizeof (long));
		converted[counter][(**sol).nElements] = 0;
		ACVECTOR_FOREACH(e, *sol) {
			if (*e == &(sl->dummy)) {
				converted[counter][counter1] = -1;
			} else {
				converted[counter][counter1] = (**e).alias;
			}
			counter1 += 1;
		}
		counter += 1;
	}

	return converted;
}

void
storylines_converted_release(long*** converted) {
	long** it0 = *converted;
	while (*it0 != 0) {
		free(*it0);
		it0++;
	}
	free(*converted);
	*converted = 0;
}
