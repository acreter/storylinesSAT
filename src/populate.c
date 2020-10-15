#include "slsolver.h"
#include "structs.h"

#include <acvector.h>
#include <stdlib.h>
#include <string.h>

int
storylines_add_entity(Storylines * storylines,
		Time_t time_starting,
		Time_t time_ending,
		unsigned int alias) {
	if (time_ending < time_starting || time_ending > storylines->max_time) return 1;

	Entity e = {storylines->entities->number_of_elements, alias, time_starting, time_ending};
	acvector_append(&(storylines->entities), &e);

	storylines->events[time_starting].starting = 1;
	storylines->events[time_ending].ending = 1;

	return 0;
}

int
storylines_add_context(Storylines * storylines,
		Time_t time_starting,
		Time_t time_ending,
		unsigned int size,
		unsigned int members[]) {
	if (!size) return 1;
	if (time_ending < time_starting || time_ending > storylines->max_time) return 2;

	Context c;
	c.size = size;
	c.start = time_starting;
	c.end = time_ending;
	c.members = malloc(sizeof (unsigned int) * size);

	/* NOTE: this is the sole reason for string.h being included.
	 * maybe a simple for loop might be better */
	memcpy(c.members, members, sizeof (unsigned int) * size);

	acvector_append(&(storylines->contexts), &c);

	storylines->events[time_starting].starting = 1;
	storylines->events[time_ending].ending = 1;

	return 0;
}
