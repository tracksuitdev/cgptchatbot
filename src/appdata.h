#ifndef CGPTCHATBOT_APPDATA_H
#define CGPTCHATBOT_APPDATA_H
#include <stdbool.h>

typedef struct {
    char *api_key;
} APPDATA;
void appdata_free(APPDATA *appdata);

typedef enum {
    CREATE_DIR_SUCCESS,
    CREATE_DIR_ERROR,
    CREATE_DIR_ALREADY_EXISTS
} CREATE_DIR_STATUS;

bool data_dir_exists();
CREATE_DIR_STATUS create_data_dir();
bool remove_data_dir();

APPDATA *read_data_file();
APPDATA *save_api_key_and_create_data(const char *api_key);
bool save_api_key(const char *api_key);

#endif //CGPTCHATBOT_APPDATA_H