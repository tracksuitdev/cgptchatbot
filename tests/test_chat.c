#include <stdlib.h>
#include <string.h>
#include "unity.h"
#include "../src/chat.h"
#include "test_util.h"
#include "../src/appdata.h"
#include "../src/util.h"

void setUp() {
    backup_data_dir();
}

void tearDown() {
    restore_data_dir();
}

void test_chat_create_save_load() {
    CGptChat *chat = cgpt_chat_new();
    chat->messages_count = 1;
    chat->messages = malloc(sizeof(OpenAiMessage *) * 2);
    chat->messages[0] = openai_message_new();
    chat->messages[0]->content = strdup("Hello");
    chat->messages[1] = NULL;
    chat->completions_count = 1;
    chat->completions = malloc(sizeof(OpenAiCompletion *) * 2);
    chat->completions[0] = openai_completion_new();
    chat->completions[0]->id = strdup("123");
    chat->completions[1] = NULL;
    chat->id = 123;

    create_data_dir();
    bool result = cgpt_chat_save(chat);
    TEST_ASSERT_TRUE(result);

    CGptChat *loaded_chat = cgpt_chat_new();
    int load_result = cgpt_chat_load(123, loaded_chat);
    TEST_ASSERT_EQUAL(123, loaded_chat->id);
    TEST_ASSERT_EQUAL(123, load_result);
    TEST_ASSERT_EQUAL(1, loaded_chat->messages_count);
    TEST_ASSERT_EQUAL(1, loaded_chat->completions_count);
    TEST_ASSERT_EQUAL_STRING("Hello", loaded_chat->messages[0]->content);
    TEST_ASSERT_EQUAL_STRING("123", loaded_chat->completions[0]->id);
    cgpt_chat_free(chat);
}

void test_chat_load_nonexistent() {
    CGptChat *chat = cgpt_chat_new();
    int load_result = cgpt_chat_load(123, chat);
    TEST_ASSERT_EQUAL(-1, load_result);
    cgpt_chat_free(chat);
}

void test_chat_to_json() {
    CGptChat *chat = cgpt_chat_new();
    chat->id = 123;
    chat->messages_count = 1;
    chat->messages = malloc(sizeof(OpenAiMessage *) * 2);
    chat->messages[0] = openai_message_new();
    chat->messages[0]->content = strdup("Hello");
    chat->messages[1] = NULL;
    chat->completions_count = 1;
    chat->completions = malloc(sizeof(OpenAiCompletion *) * 2);
    chat->completions[0] = openai_completion_new();
    chat->completions[0]->id = strdup("123");
    chat->completions[1] = NULL;

    cJSON *chat_json = cgpt_chat_to_json(chat);
    TEST_ASSERT_NOT_NULL(chat_json);
    TEST_ASSERT_EQUAL(123, cjson_int_at(chat_json, "id"));
    cJSON *messages = cJSON_GetObjectItemCaseSensitive(chat_json, "messages");
    TEST_ASSERT_NOT_NULL(messages);
    TEST_ASSERT_EQUAL(1, cJSON_GetArraySize(messages));
    cJSON *message = cJSON_GetArrayItem(messages, 0);
    TEST_ASSERT_NOT_NULL(message);
    TEST_ASSERT_EQUAL_STRING("Hello", cJSON_GetStringValue(message->child));
    cJSON *completions = cJSON_GetObjectItemCaseSensitive(chat_json, "completions");
    TEST_ASSERT_NOT_NULL(completions);
    TEST_ASSERT_EQUAL(1, cJSON_GetArraySize(completions));
    cJSON *completion = cJSON_GetArrayItem(completions, 0);
    TEST_ASSERT_NOT_NULL(completion);
    TEST_ASSERT_EQUAL_STRING("123", cJSON_GetStringValue(completion->child));
    cgpt_chat_free(chat);
}

int main() {
    UNITY_BEGIN();
    RUN_TEST(test_chat_create_save_load);
    RUN_TEST(test_chat_load_nonexistent);
    RUN_TEST(test_chat_to_json);
    return UNITY_END();
}

