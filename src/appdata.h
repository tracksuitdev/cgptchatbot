#ifndef CGPTCHATBOT_APPDATA_H
#define CGPTCHATBOT_APPDATA_H
#include <stdbool.h>
#include "openai.h"

/**
 * This module is responsible for managing the application data. Application data is stored in a directory named
 * CGPT_DATA_DIR in the user's home directory. This directory is created if it does not exist.
 */

/**
 * The name of the directory where the application data is stored.
 */
extern const char CGPT_DATA_DIR[];

extern const int CGPT_MAX_FILE_PATH;

/**
 * Struct that holds application data, currently only the API key.
 */
typedef struct {
    char *api_key;
    OPENAI_API *openai_api;
} APPDATA;

void appdata_free(APPDATA *appdata);

typedef enum {
    CREATE_DIR_SUCCESS,
    CREATE_DIR_ERROR,
    CREATE_DIR_ALREADY_EXISTS
} CREATE_DIR_STATUS;

/**
 * Fill the given path with the path to the application data directory.
 * @param path path to data directory
 */
void data_dir_path(char *path);

bool data_dir_exists();
CREATE_DIR_STATUS create_data_dir();
/**
 * Remove the application data directory and all its contents.
 * @return true if the directory was removed successfully, false otherwise
 */
bool remove_data_dir();

/**
 * Read the API key from the data file and creates an APPDATA struct with the API key. The caller is responsible for
 * freeing the returned APPDATA struct.
 * @return APPDATA struct with the API key, or NULL if the data file does not exist
 */
APPDATA *read_data_file();

/**
 * Save the given API key to the data file. Overwrites the data file, if it doesn't exist, it is created. APPDATA struct
 * is created to hold the API key. The caller is responsible for freeing the returned APPDATA struct.
 * @param api_key api key to save
 * @return APPDATA struct with the API key, or NULL if the data file could not be created
 */
APPDATA *save_api_key_and_create_data(const char *api_key);

/**
 * Save the given API key to the data file. Overwrites the data file. If the file doesn't exist, it is created.
 * @param api_key api key to save
 * @return true if the API key was saved successfully, false otherwise
 */
bool save_api_key(const char *api_key);

/**
 * Will fill the given path with the path to the file with the given name in the application data directory. Basically
 * path = {CGPT_DATA_DIR}/{file_name}.
 */
void file_path_in_data_dir(const char *file_name, char *path);

/**
 * Initializes the cgpt_global_appdata struct with the data from the data file. If the struct is already initialized, it
 * will be freed first. The openai_api field is also initialized with the OpenAI API URL and the API key.
 */
void init_appdata();

/**
 * Get the API key from the cgpt_global_appdata struct.
 * @return the API key or NULL if the cgpt_global_appdata struct is NULL, this char pointer should not be freed
 */
char *cgpt_global_appdata_get_api_key();

/**
 * Get the OpenAI API from the cgpt_global_appdata struct.
 * @return the OpenAI API or NULL if the cgpt_global_appdata struct is NULL, this pointer should not be freed
 */
OPENAI_API *cgpt_global_appdata_get_openai_api();

/**
 * Check if the cgpt_global_appdata struct has an API key.
 */
bool cgpt_global_appdata_has_api_key();

/**
 * Free the cgpt_global_appdata struct and all its fields.
 */
void cgpt_global_appdata_free();

#endif //CGPTCHATBOT_APPDATA_H
