#include <stdlib.h>

#include <acvector.h>
#include <context.h>
#include <solver.h>
#include <storyline.h>
#include <util.h>

void
die(
	struct storyline* instance
) {
	storyline_release(&instance);
	exit(EXIT_FAILURE);
}
