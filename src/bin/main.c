#include <stdio.h>
#include <stdlib.h>
#include <acvector.h>
#include <solver.h>
#include <context.h>
#include <storyline.h>
#include <simple_parser.h>

int
main(int argc, char ** argv) {
	struct storyline* s = storyline_init();

	if(argc >= 2) {
		parse_input_file(&s, argv[1]);
	}

	struct result* r = storyline_solve(s);

	free(r);
	storyline_release(&s);
	return 0;
}
