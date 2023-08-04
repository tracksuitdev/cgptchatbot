#include <glib.h>
#include <gio/gio.h>
#include "test_util.h"
#include "../src/appdata.h"
#include "unity.h"

void copy_directory(const char *src_dir, const char *dest_dir) {
    GDir *dir;
    const gchar *filename;
    GError *error = NULL;

    // Create the destination directory if it doesn't exist
    g_mkdir_with_parents(dest_dir, 0777);

    // Open the source directory
    dir = g_dir_open(src_dir, 0, &error);
    if (!dir) {
        g_printerr("Error opening source directory: %s\n", error->message);
        g_error_free(error);
        return;
    }

    // Iterate over the directory entries
    while ((filename = g_dir_read_name(dir)) != NULL) {
        gchar *src_path = g_build_filename(src_dir, filename, NULL);
        gchar *dest_path = g_build_filename(dest_dir, filename, NULL);

        GFile *src_file = g_file_new_for_path(src_path);
        GFile *dest_file = g_file_new_for_path(dest_path);

        if (g_file_query_file_type(src_file, G_FILE_QUERY_INFO_NONE, NULL) == G_FILE_TYPE_DIRECTORY) {
            // Recursively copy subdirectories
            copy_directory(src_path, dest_path);
        } else {
            // Copy regular files
            if (!g_file_copy(src_file, dest_file, G_FILE_COPY_OVERWRITE, NULL, NULL, NULL, &error)) {
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

void backup_data_dir() {
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

void restore_data_dir() {
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
