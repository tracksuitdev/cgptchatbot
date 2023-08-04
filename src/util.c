#include <stdlib.h>
#include <string.h>
#include "util.h"
#include "cJSON.h"

static const int BUFFER_SIZE = 10000;

void free_array(void **array) {
    if (array == NULL) {
        return;
    }
    for (int i = 0; array[i] != NULL; i++) {
        FREE(array[i])
    }
    FREE(array);
}

cJSON *parse_json_file(FILE *file) {
    char *json = malloc(BUFFER_SIZE);
    int buffer_size = BUFFER_SIZE;
    size_t offset = 0;
    *json = '\0';
    while(1) {
        size_t read = fread(json + offset, 1, buffer_size, file);
        if (read != buffer_size - offset) {
            break;
        }

        offset += read;
        buffer_size = buffer_size * 2;
        char *tmp = realloc(json, buffer_size + 1);
        if (tmp == NULL) {
            FREE(json)
            fclose(file);
            return NULL;
        }
        json = tmp;
    }
    json[buffer_size] = '\0';

    fclose(file);

    cJSON *parsed = cJSON_Parse(json);
    FREE(json)
    return parsed;
}

char* cjson_string_at(cJSON *json, char *key) {
    cJSON *value = cJSON_GetObjectItem(json, key);
    if (value != NULL) {
        return strdup(value->valuestring);
    } else {
        return NULL;
    }
}

int cjson_int_at(cJSON *json, char *key) {
    cJSON *value = cJSON_GetObjectItem(json, key);
    if (value != NULL) {
        return (int) value->valuedouble;
    } else {
        return 0;
    }
}

long cjson_long_at(cJSON *json, const char *key) {
    cJSON *value = cJSON_GetObjectItem(json, key);
    if (value != NULL) {
        return (long) value->valuedouble;
    } else {
        return 0;
    }
}