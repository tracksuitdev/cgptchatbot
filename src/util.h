#ifndef CGPTCHATBOT_UTIL_H
#define CGPTCHATBOT_UTIL_H
#include "cJSON.h"
#include "stdio.h"

#define FREE(ptr) \
    free(ptr); \
    ptr = NULL;
void free_array(void **array);

/**
 * Parse a JSON file into a cJSON object.
 * @param file Opened file to parse. The file will be closed.
 * @return cJSON object or NULL if the file could not be parsed. The caller is responsible for freeing the cJSON object.
 */
cJSON *parse_json_file(FILE *file);

char *cjson_string_at(cJSON *json, char *key);
int cjson_int_at(cJSON *json, char *key);
long cjson_long_at(cJSON *json, const char *key);

#endif //CGPTCHATBOT_UTIL_H
