#ifndef __ENTITY_H_INCLUDED
#define __ENTITY_H_INCLUDED 1

typedef struct entity entity_t;

struct entity {
	unsigned int id,
				 alias,
				 t_birth,
				 t_death;
};

#endif
