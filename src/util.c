#include <stdlib.h>
#include "util.h"

void free_array(void **array) {
    if (array == NULL) {
        return;
    }
    for (int i = 0; array[i] != NULL; i++) {
        FREE(array[i])
    }
    FREE(array);
}