#ifndef __CONTEXT_H_INCLUDED
#define __CONTEXT_H_INCLUDED 1

typedef struct event event_t;
typedef struct context_map context_map_t;

struct event {
	unsigned int starting;
	unsigned int ending;
	unsigned int incline;
	unsigned int decline;
};

struct context_map {
	unsigned int t_max;
	event_t* events;
	acVector* timeslots[];
};

#endif
