#include <stdlib.h>
#include <stdio.h>
#include "unity.h"
#include "../src/appdata.h"
#include "stdbool.h"
#include "string.h"
#include "test_util.h"


void setUp() {
    backup_data_dir();
}

void tearDown() {
    restore_data_dir();
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
