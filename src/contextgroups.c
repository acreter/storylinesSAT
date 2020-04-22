#include "slsolver.h"
#include "structs.h"
#include "contextgroups.h"
#include <acvector.h>

#ifndef NDEBUG
#include <stdio.h>

void
print_contextgroups(Storylines* sl) {
	printf("### PRINTING <%d> CONTEXTGROUPS ###\n", (sl->contextgroups)->nElements);
	acVector*** a;
	Entity** b;
	Context** c;
	ACVECTOR_FOREACH(a, sl->contextgroups) {
		ACVECTOR_FOREACH(b, **a) {
			printf("%d ", (**b).alias);
		}
		printf("\n");

		ACVECTOR_FOREACH(c, *((*a) + 1)) {
			for (unsigned int i = 0; i < (**c).size; i += 1) {
				printf("%d ", (**c).members[i]);
			}
			printf("\t");
		}
		printf("\n");

	}
	printf("### DONE PRINTING <%d> CONTEXTGROUPS ###\n\n", (sl->contextgroups)->nElements);

	return;
}
#endif

void
find_contextgroups(Storylines* sl) {
	Byte_t last_meeting_started_flag = 1;
	Time_t last_meeting_ended_at = 0;
	acVector** v;
	for(Time_t t = 0; t <= sl->max_time; t += 1) {
		if(sl->events[t].starting) {
			if (!last_meeting_started_flag) {
				sl->events[t].incline = 1;
				sl->events[last_meeting_ended_at].decline = 1;
			}
			last_meeting_started_flag = 1;
		}

		if(sl->events[t].ending) {
			if(last_meeting_started_flag) {
				v = (sl->ec_map) + t*2;
				acvector_push_back(&(sl->contextgroups), &v);
			}
			last_meeting_started_flag = 0;
			last_meeting_ended_at = t;
		}
	}

	return;
}
