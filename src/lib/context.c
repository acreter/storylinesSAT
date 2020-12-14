#include <stdio.h>
#include <stdlib.h>

#include <acvector.h>
#include <context.h>
#include <storyline.h>
#include <solver.h>
#include <util.h>
#include <color.h>

void
storyline_add_context(
	struct storyline* instance,
	unsigned int t_start,
	unsigned int t_end,
	unsigned int size,
	unsigned int members[]
) {
#ifndef NCHECK
	if (t_end < t_start) {
		fprintf(stderr,
			ANSI_COLOR_RED "ERROR" ANSI_COLOR_RESET \
				": Invalid context ends before it starts:" \
				" t_start=%i, t_end=%i\n",
			t_start,
			t_end
		);
		die(instance);
	}
#endif

	unsigned int t_max_new;
	if (t_end >= instance->map->t_max) {
		t_max_new = t_end + 1;
		event_t* new_e = realloc(instance->map->events, sizeof (event_t)
				* t_max_new);
		context_map_t* new_cm = realloc(instance->map, sizeof (context_map_t)
				+ sizeof (acVector*) * t_max_new);
		if (!new_cm || !new_e) {
			fprintf(stderr,
				ANSI_COLOR_RED "ERROR" ANSI_COLOR_RESET \
					": Unable to allocate memory:" \
					" realloc failed"
			);
			die(instance);
		}

		for (
			unsigned int x = new_cm->t_max;
			x < t_max_new;
			x+=1
		) {
			new_e[x].starting = 0;
			new_e[x].ending = 0;
			new_e[x].incline = 0;
			new_e[x].decline = 0;
			new_cm->timeslots[x] = acvector_create(8, sizeof (unsigned int));
		}

		new_cm->t_max = t_max_new;
		new_cm->events = new_e;
		instance->map = new_cm;
	}

	instance->map->events[t_start].starting = 1;
	instance->map->events[t_end].ending = 1;

	unsigned int zero = 0;
	for (unsigned int x = t_start; x <= t_end; x+=1) {
		acvector_append(instance->map->timeslots + x, &size, 1);
		acvector_append(instance->map->timeslots + x, &t_start, 1);
		acvector_append(instance->map->timeslots + x, &t_end, 1);
		for (unsigned int y = 0; y < size; y+=1) {
			acvector_append(instance->map->timeslots + x, &zero, 1);
		}
		acvector_append(instance->map->timeslots + x, members, size);
	}
}
