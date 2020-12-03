#ifndef __SIMPLE_PARSER_H
#define __SIMPLE_PARSER_H
#include <slsolver.h>

typedef struct metainfo metainfo;

struct metainfo {
	unsigned int number_of_entities;
	unsigned int number_of_contexts;
};

metainfo parse_input_file(Storylines **, const char *);
#endif
