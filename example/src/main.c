#include <slsolver.h>
#ifdef SLDRAW
#include <sldrawer.h>
#endif
#include "simple_parser.h"

#include <stdio.h>

int
main(int argc, char ** argv) {
	Storylines * s = 0;
	metainfo m;
	if(argc >= 2) {
		m = parse_input_file(&s, argv[1]);
	}
	Result* result = storylines_solve(s);
	printf("Solved %s using %ld block crossings.\n", argv[1], result->number_of_block_crossings);
#ifdef SLDRAW
	printf("Writing drawing to storylines.svg\n");
	storylines_draw_solution(s, result->solution, "storylines.svg", "svg");
#endif
	storylines_result_release(&result);
	storylines_release(&s);
	return 0;
}
