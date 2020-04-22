#include "slsolver.h"
#include "sort.h"

void gnomesort(Time_t* start, Time_t* end) {
	Time_t swap;
	Time_t* current = start;

	while (current <= end) {
		if (current == start || current[0] >= current[-1]) {
			current += 1;
		} else {
			swap = current[0];
			current[0] = current[-1];
			current[-1] = swap;
			current -= 1;
		}
	}
}
