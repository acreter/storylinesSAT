#include <acvector.h>
#include "sldrawer.h"
#include <structs.h>

#include <stdio.h>
#include <plot.h>
#include <math.h>

void
storylines_draw_solution(Storylines* sl, Solution s, char* filename, char* type) {
	plPlotter* plotter;
	plPlotterParams* plotter_params;

	acVector* v = (acVector*) s;

	plotter_params = pl_newplparams();
	pl_setplparam(plotter_params, "PAGESIZE", "a4");

	FILE* f = fopen(filename, "w");
	plotter = pl_newpl_r(type, stdin, f, stderr, plotter_params);
	pl_openpl_r(plotter);

#ifndef NDEBUG
	printf("Drawing space: %d x %ld\n", 30, v->number_of_elements * 2);
#endif
	pl_space_r(plotter, 0, 0, 100, v->number_of_elements * 2);
	pl_pencolorname_r(plotter, "black");

	Entity* e, ** e1;
	unsigned int index = 0, old_y = 0, middle_point, counter;
	acVector** v1;
	ACVECTOR_FOREACH(e, sl->entities) {
		counter = 0;
		v1 = acvector_get_at(&v, e->start);
		ACVECTOR_FOREACH(e1, *v1) {
			if (*e1 == e) {
				old_y = counter;
				break;
			}
			counter += 1;
		}
#ifndef NDEBUG
		printf("\nMoving cursor to %ld,%d\n", e->start*2, old_y);
#endif
		pl_move_r(plotter, e->start*2, old_y);
		for (unsigned int i = e->start + 1; i <= e->end; i += 1) {
			middle_point = (i-1)*2+1;
			counter = 0;
			v1 = acvector_get_at(&v, i);
			ACVECTOR_FOREACH(e1, *v1) {
				if (*e1 == e) {
					index = counter;
					break;
				}
				counter += 1;
			}
#ifndef NDEBUG
			printf("Drawing curve from %d,%d to %d,%d\n", middle_point - 1, old_y, i*2, index);
#endif
			pl_bezier3_r(plotter, middle_point - 1, old_y, middle_point, old_y, middle_point, index, i*2, index);
			old_y = index;
		}
		pl_endpath_r(plotter);
	}

	pl_closepl_r(plotter);
	pl_deletepl_r(plotter);
	fclose(f);

	return;
}
