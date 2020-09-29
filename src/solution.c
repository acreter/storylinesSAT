#include "solution.h"
#include "structs.h"

acVector*
solution_add_seperators_single_layer(acVector* result, acVector* ec_map_context_entry, Entity* sldummy) {
	unsigned int index = 0;
	Entity** e = 0;
	Context* c = 0;
	unsigned int skip = 0;
	//printf("DUMP SEPERATORS\n");
	ACVECTOR_FOREACH(e, result) {
		if (!*e) break;
		c = find_context_of(*e, ec_map_context_entry);
	//	debug_dump_context(c);
		if (index) acvector_insert(&result, index, &sldummy);
		skip = c ? c->size - 1: 0;
		skip += index ? 1 : 0;
		for (unsigned int i = 0; i < skip; i += 1) {
			e += 1;
		}
		index += skip + 1;
	}

	return result;
}
