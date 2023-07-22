#ifndef CGPTCHATBOT_UTIL_H
#define CGPTCHATBOT_UTIL_H

// define a macro to free and nullify a pointer
#define FREE(ptr) \
    free(ptr); \
    ptr = NULL;


void free_array(void **array);

#endif //CGPTCHATBOT_UTIL_H
