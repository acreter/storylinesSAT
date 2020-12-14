#include <stdio.h>
#include <stdlib.h>

#include <acvector.h>
#include <context.h>
#include <storyline.h>
#include <solver.h>
#include <entity.h>
#include <color.h>
#include <util.h>

void
storyline_add_entity(
	struct storyline* instance,
	unsigned int t_birth,
	unsigned int t_death,
	unsigned int alias
) {
#ifndef NCHECK
	if (t_death < t_birth) {
		fprintf(stderr,
			ANSI_COLOR_RED "ERROR" ANSI_COLOR_RESET \
				": Invalid character dies before it gets born:" \
				" t_birth=%i, t_death=%i\n",
			t_birth,
			t_death
		);
		die(instance);
	}
#endif

	unsigned int t_max_new;
	if (t_death >= instance->map->t_max) {
		t_max_new = t_death + 1;
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

	instance->map->events[t_birth].starting = 1;
	instance->map->events[t_death].ending = 1;

	entity_t E;
	E.id = 0;
	E.alias = alias;
	E.t_birth = t_birth;
	E.t_death = t_death;

	acvector_append(&(instance->entities), &E, 1);
	return;
}
