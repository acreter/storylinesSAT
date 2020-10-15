#ifndef __STRUCTS_H_INCLUDED
#define __STRUCTS_H_INCLUDED 1

#include "slsolver.h"
#include <acvector.h>

#define ACVECTOR_FOREACH(c, v) for ((c) = acvector_get_first((&(v)));\
		 (void*) (c) <= acvector_get_last((&(v)));\
		 (c) += 1)

typedef struct Event Event;
typedef struct Entity Entity;
typedef struct Context Context;
typedef struct Literals Literals;
typedef struct Seperated_Entities Seperated_Entities;

Literals* literals_create(Storylines*);
Seperated_Entities* seperated_entities_create();
void literals_release(Literals**);
void seperated_entities_release(Seperated_Entities**);
void build_ec_map(Storylines*);
/* void seperated_entities_reset(Seperated_Entities*); */

/* vector hold pointers to contexts */
Context* find_context_of(Entity*, acVector*);

#ifndef NDEBUG
#include <csolver.h>
void debug_dump_storylines(Storylines*);
void debug_dump_event(Event*);
void debug_dump_entity(Entity*);
void debug_dump_context(Context*);
void debug_dump_ec_map(Storylines*, acVector**);
void debug_dump_contextgroups(acVector*);
void debug_dump_ec_map_entity(acVector**);
void debug_dump_ec_map_context(acVector**);
void debug_dump_seperated_entities(Seperated_Entities*);
void debug_dump_solution(Solution);
void debug_dump_solver_table(CSolver*, Literals*, unsigned int, unsigned int);
#endif

struct Event {
	Byte_t starting;
	Byte_t ending;
	Byte_t decline;
	Byte_t incline;
};

struct Entity {
	unsigned int id;
	unsigned int alias;
	Time_t start;
	Time_t end;
};

struct Context {
	unsigned int size;
	unsigned int* members;
	Time_t start;
	Time_t end;
};

struct Literals {
	unsigned long number_of_literals;
	unsigned long b_starts_at;
	unsigned long c_starts_at;
	unsigned long q_starts_at;
};

struct Seperated_Entities {
	acVector* in_context;
	acVector* not_in_context;
};

struct Storylines {
	Byte_t prepared;
	Time_t max_time;
	acVector* entities;
	acVector* contexts;
	/* entity-context-map */
	acVector** ec_map;
	/* maps to the entity-context-map */
	acVector* contextgroups;
	Entity dummy;
	Event events[];
};
#endif
