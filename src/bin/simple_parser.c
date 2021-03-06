#include <acvector.h>
#include <solver.h>
#include <simple_parser.h>
#include <cJSON.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define cJSON_get cJSON_GetObjectItemCaseSensitive

char * read_file_raw(const char *);
int cJSON_int_array(int **, cJSON *);
int parse_int(char *);

void
parse_input_file(struct storyline** storyline, const char * filepath) {
	char * raw = read_file_raw(filepath);
	cJSON * root = cJSON_Parse(raw);
	cJSON * e = NULL;

	int * births = NULL,
		* deaths = NULL,
		nBirths,
		id;
	cJSON_ArrayForEach(e, cJSON_get(root, "characters")) {
		id = cJSON_get(e, "id")->valueint;
		nBirths = cJSON_int_array(&births, cJSON_get(e, "births"));
		cJSON_int_array(&deaths, cJSON_get(e, "deaths"));

		for (int i = 0; i < nBirths; i += 1) {
			storyline_add_entity(*storyline, births[i], deaths[i], id);
		}

		free(births);
		free(deaths);
	}

	int time_starting,
		time_ending,
		nMembers,
		* members = NULL;
	cJSON_ArrayForEach(e, cJSON_get(root, "meetings")) {
		time_starting = cJSON_get(e, "time_starting")->valueint;
		time_ending = cJSON_get(e, "time_ending")->valueint;
		nMembers = cJSON_int_array(&members, cJSON_get(e, "members"));

		storyline_add_context(*storyline, time_starting, time_ending, nMembers, members);

		free(members);
	}
	
	cJSON_Delete(root);
	free(raw);
	
	return;
}

int cJSON_int_array(int ** array, cJSON * a) {
	if(!cJSON_IsArray(a)) return 0;

	*array = malloc(cJSON_GetArraySize(a) * sizeof(int));
	cJSON * e = NULL;
	int c = 0;
	cJSON_ArrayForEach(e, a) {
		(*array)[c++] = e->valueint;
	}

	return c;
}

char *
read_file_raw(const char * filepath) {
    char * buffer = 0;
    long length;
    FILE * file = fopen(filepath, "r");

    if(file) {
		fseek(file, 0, SEEK_END);
		length = ftell(file);
		fseek(file, 0, SEEK_SET);
		buffer = (char *) malloc(sizeof(char) * length);
		if(buffer) {
		    fread(buffer, 1, length, file);
		} else {
			fclose(file);
			return NULL;
        }
        fclose(file);
    } else {
		return NULL;
    }

    return buffer;
}
