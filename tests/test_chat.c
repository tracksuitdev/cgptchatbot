#include <stdlib.h>
#include "unity.h"
#include "../src/chat.h"

void setUp() {}
void tearDown() {}

void test_chat_create_save_load() {
    CGptChat *chat = cgpt_chat_new();
    chat->messages_count = 1;
    chat->messages = malloc(sizeof(OpenAiMessage *) * 2);
    chat->messages[0] = openai_message_new();
    chat->messages[0]->content = "Hello";
    chat->messages[1] = NULL;
    chat->completions_count = 1;
    chat->completions = malloc(sizeof(OpenAiCompletion *) * 2);
    chat->completions[0] = openai_completion_new();
    chat->completions[0]->id = "123";
    chat->completions[1] = NULL;
    chat->id = 123;

    bool result = cgpt_chat_save(chat);
    TEST_ASSERT_TRUE(result);

    CGptChat *loaded_chat = cgpt_chat_new();
    result = cgpt_chat_load(123, loaded_chat);
    TEST_ASSERT_EQUAL(123, loaded_chat->id);
    TEST_ASSERT_EQUAL(123, result);
    TEST_ASSERT_EQUAL(1, loaded_chat->messages_count);
    TEST_ASSERT_EQUAL(1, loaded_chat->completions_count);
    TEST_ASSERT_EQUAL_STRING("Hello", loaded_chat->messages[0]->content);
    TEST_ASSERT_EQUAL_STRING("123", loaded_chat->completions[0]->id);

    cgpt_chat_free(chat);
}

int main() {
    UNITY_BEGIN();
    RUN_TEST(test_chat_create_save_load);
    return UNITY_END();
}

