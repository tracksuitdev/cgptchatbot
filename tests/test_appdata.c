#include <stdlib.h>
#include <glib.h>
#include <stdio.h>
#include <gio/gio.h>
#include "unity.h"
#include "../src/appdata.h"
#include "stdbool.h"
#include "string.h"

void copy_directory(const char *src_dir, const char *dest_dir) {
    GDir *dir;
    const gchar *filename;
    GError *error = NULL;

    // Create the destination directory if it doesn't exist
    g_mkdir_with_parents(dest_dir, 0777);

    // Open the source directory
    dir = g_dir_open(src_dir, 0, &error);
    if (!dir)
    {
        g_printerr("Error opening source directory: %s\n", error->message);
        g_error_free(error);
        return;
    }

    // Iterate over the directory entries
    while ((filename = g_dir_read_name(dir)) != NULL)
    {
        gchar *src_path = g_build_filename(src_dir, filename, NULL);
        gchar *dest_path = g_build_filename(dest_dir, filename, NULL);

        GFile *src_file = g_file_new_for_path(src_path);
        GFile *dest_file = g_file_new_for_path(dest_path);

        if (g_file_query_file_type(src_file, G_FILE_QUERY_INFO_NONE, NULL) == G_FILE_TYPE_DIRECTORY)
        {
            // Recursively copy subdirectories
            copy_directory(src_path, dest_path);
        }
        else
        {
            // Copy regular files
            if (!g_file_copy(src_file, dest_file, G_FILE_COPY_OVERWRITE, NULL, NULL, NULL, &error))
            {
                g_printerr("Error copying file '%s' to '%s': %s\n", src_path, dest_path, error->message);
                g_error_free(error);
            }
        }

        g_free(src_path);
        g_free(dest_path);
        g_object_unref(src_file);
        g_object_unref(dest_file);
    }

    // Close the directory
    g_dir_close(dir);
}



void setUp() {
    if (data_dir_exists()) {
        APPDATA *data = read_data_file();

        // create backup in case of existing installation
        if (data != NULL) {
            char file_path[CGPT_MAX_FILE_PATH];
            data_dir_path(file_path);
            copy_directory(file_path, "backup_cgptchatbot");
        }

        bool result = remove_data_dir();
        TEST_ASSERT_TRUE(result);
    }
}

void tearDown() {
    if (data_dir_exists()) {
        bool result = remove_data_dir();
        TEST_ASSERT_TRUE(result);

        // restore backup in case of existing installation
        GError *error = NULL;
        GDir *dir = g_dir_open("backup_cgptchatbot", 0, &error);
        char file_path[CGPT_MAX_FILE_PATH];
        data_dir_path(file_path);
        if (dir != NULL) {
            copy_directory("backup_cgptchatbot", file_path);
            g_dir_close(dir);
        } else {
            g_printerr("Error opening backup directory: %s\n", error->message);
            g_error_free(error);
        }
    }
}

void test_create_data_dir() {
    CREATE_DIR_STATUS status = create_data_dir();
    TEST_ASSERT_EQUAL_INT(CREATE_DIR_SUCCESS, status);
    CREATE_DIR_STATUS status2 = create_data_dir();
    TEST_ASSERT_EQUAL_INT(CREATE_DIR_ALREADY_EXISTS, status2);
}

void test_remove_data_dir() {
    CREATE_DIR_STATUS status = create_data_dir();
    TEST_ASSERT_EQUAL_INT(CREATE_DIR_SUCCESS, status);
    bool result = remove_data_dir();
    TEST_ASSERT_TRUE(result);
}

void test_read_data_file() {
    CREATE_DIR_STATUS status = create_data_dir();
    TEST_ASSERT_EQUAL_INT(CREATE_DIR_SUCCESS, status);

    char file_path[257];
    char *home = getenv("HOME");
    snprintf(file_path, 257, "%s/.cgptchatbot/config.txt", home);
    FILE *f = fopen(file_path, "w");
    fputs("test", f);
    fclose(f);

    APPDATA *data = read_data_file();
    TEST_ASSERT_NOT_NULL(data);
    TEST_ASSERT_EQUAL_STRING("test", data->api_key);
    appdata_free(data);
}

void test_save_api_key() {
    CREATE_DIR_STATUS status = create_data_dir();
    TEST_ASSERT_EQUAL_INT(CREATE_DIR_SUCCESS, status);

    APPDATA *data = save_api_key_and_create_data("test");
    TEST_ASSERT_NOT_NULL(data);
    TEST_ASSERT_EQUAL_STRING("test", data->api_key);
    appdata_free(data);

    char file_path[257];
    char *home = getenv("HOME");
    snprintf(file_path, 257, "%s/.cgptchatbot/config.txt", home);
    FILE *f = fopen(file_path, "r");
    char api_key[1000];
    fgets(api_key, 1000, f);
    fclose(f);
    TEST_ASSERT_EQUAL_STRING("test", api_key);
}

int main() {
    UNITY_BEGIN();
    RUN_TEST(test_create_data_dir);
    RUN_TEST(test_remove_data_dir);
    RUN_TEST(test_read_data_file);
    RUN_TEST(test_save_api_key);
    return UNITY_END();
}
