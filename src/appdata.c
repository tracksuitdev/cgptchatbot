#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <stdbool.h>
#include "util.h"
#include "appdata.h"

#ifdef _WIN32
    #include <direct.h>
    #define MKDIR(path) _mkdir(path)
    #include "windows.h"
#else
    #include <sys/stat.h>
    #include "unistd.h"
    #define MKDIR(path) mkdir(path, 0777)
#endif

const char CGPT_DATA_DIR[] = ".cgptchatbot";
const int CGPT_MAX_FILE_PATH = 1024;

static const char DATA_FILE_NAME[] = "config.txt";
static const int MAX_LINE_LENGTH = 1000;

void data_dir_path(char *path) {
    char *home = getenv("HOME");
    snprintf(path, CGPT_MAX_FILE_PATH, "%s/%s", home, CGPT_DATA_DIR);
}

bool data_dir_exists() {
#ifdef _WIN32
    char dir_path[CGPT_MAX_FILE_PATH];
    data_dir_path(dir_path);
    DWORD attrib = GetFileAttributes(dir_path);
    return (attrib != INVALID_FILE_ATTRIBUTES && (attrib & FILE_ATTRIBUTE_DIRECTORY));
#else
    char dir_path[CGPT_MAX_FILE_PATH];
    data_dir_path(dir_path);
    struct stat st;
    return (stat(dir_path, &st) == 0 && S_ISDIR(st.st_mode));
#endif
}

static inline FILE *get_data_file(char *mode) {
    char file_path[CGPT_MAX_FILE_PATH];
    data_dir_path(file_path);
    strcat(file_path, "/");
    strcat(file_path, DATA_FILE_NAME);
    FILE *file = fopen(file_path, mode);
    return file;
}

void appdata_free(APPDATA *appdata) {
    FREE(appdata->api_key)
    FREE(appdata)
}

CREATE_DIR_STATUS create_data_dir() {
    char dir_path[CGPT_MAX_FILE_PATH];
    data_dir_path(dir_path);
    if (MKDIR(dir_path) == 0) {
        return CREATE_DIR_SUCCESS;
    } else if (errno == EEXIST) {
        return CREATE_DIR_ALREADY_EXISTS;
    } else {
        return CREATE_DIR_ERROR;
    }
}

bool remove_dir(const char *path) {
#ifdef _WIN32
    // Windows implementation
    if (!RemoveDirectory(path)) {
        printf("Error deleting directory: %s\n", path);
        return false;
    }
#else
    // Unix-based systems implementation
    if (rmdir(path) != 0) {
        perror("Error deleting directory");
        return false;
    }
#endif
    return true;
}

bool remove_data_dir() {
    char dir_path[CGPT_MAX_FILE_PATH];
    data_dir_path(dir_path);

    char file_path[CGPT_MAX_FILE_PATH];

    strcpy(file_path, dir_path);
    strcat(file_path, "/");
    strcat(file_path, DATA_FILE_NAME);
    remove(file_path);

    return remove_dir(dir_path);
}

APPDATA *read_data_file() {
    FILE *file = get_data_file("r");

    if (file == NULL) {
        return NULL;
    }

    APPDATA *appdata = malloc(sizeof(APPDATA));
    char api_key[MAX_LINE_LENGTH];
    fgets(api_key, MAX_LINE_LENGTH, file);

    appdata->api_key = strdup(api_key);
    fclose(file);
    return appdata;
}

APPDATA *save_api_key_and_create_data(const char *api_key) {
    if (save_api_key(api_key)) {

        APPDATA *appdata = malloc(sizeof(APPDATA));
        appdata->api_key = strdup(api_key);
        return appdata;
    }
    return NULL;
}

bool save_api_key(const char *api_key) {
    FILE *file = get_data_file("w");

    if (file == NULL) {
        return false;
    }

    fputs(api_key, file);
    fclose(file);
    return true;
}





