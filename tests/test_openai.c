#include "unity.h"
#include "../src/openai.h"
#include "mock_server.h"

void setUp() {}

void tearDown() {}

void test_openai_model_new() {
    OpenAiModel *model = openai_model_new();
    TEST_ASSERT_NOT_NULL(model);
    TEST_ASSERT_NULL(model->id);
    TEST_ASSERT_NULL(model->object);
    TEST_ASSERT_NULL(model->owned_by);
    TEST_ASSERT_NULL(model->permissions);
    openai_model_free(model);
}

void test_openai_model_from_json_string() {
    char *json_string = "{\"id\": \"curie:ft-user-1234\", \"object\": \"user\", \"owned_by\": \"user_1234\", \"permissions\": [\"read\", \"write\"]}";
    OpenAiModel *model = openai_model_from_json_string(json_string);
    TEST_ASSERT_EQUAL_STRING("curie:ft-user-1234", model->id);
    TEST_ASSERT_EQUAL_STRING("user", model->object);
    TEST_ASSERT_EQUAL_STRING("user_1234", model->owned_by);
    TEST_ASSERT_EQUAL_STRING("read", model->permissions[0]);
    TEST_ASSERT_EQUAL_STRING("write", model->permissions[1]);
    openai_model_free(model);
}


void test_openai_model_from_json() {
    char *json_string = "{\"id\": \"curie:ft-user-1234\", \"object\": \"user\", \"owned_by\": \"user_1234\", \"permissions\": [\"read\", \"write\"]}";
    cJSON *model_json = cJSON_Parse(json_string);
    OpenAiModel *model = openai_model_from_json(model_json);
    TEST_ASSERT_EQUAL_STRING("curie:ft-user-1234", model->id);
    TEST_ASSERT_EQUAL_STRING("user", model->object);
    TEST_ASSERT_EQUAL_STRING("user_1234", model->owned_by);
    TEST_ASSERT_EQUAL_STRING("read", model->permissions[0]);
    TEST_ASSERT_EQUAL_STRING("write", model->permissions[1]);
    openai_model_free(model);
    cJSON_Delete(model_json);
}



void test_openai_model_list_new() {
    OpenAiModelList *model_list = openai_model_list_new();
    TEST_ASSERT_NOT_NULL(model_list);
    TEST_ASSERT_NULL(model_list->models);
    TEST_ASSERT_NULL(model_list->object);
    openai_model_list_free(model_list);
}

void test_openai_parse_model_list_string() {
    char *json_string = "{\"data\": [{\"id\": \"curie:ft-user-1234\", \"object\": \"user\", \"owned_by\": \"user_1234\", \"permissions\": [\"read\", \"write\"]}, {\"id\": \"curie:ft-user-1234\", \"object\": \"user\", \"owned_by\": \"user_1234\", \"permissions\": [\"read\", \"write\"]}], \"object\": \"list\"}";
    OpenAiModelList *model_list = openai_model_list_from_json_string(json_string);
    TEST_ASSERT_EQUAL_STRING("list", model_list->object);
    TEST_ASSERT_EQUAL_STRING("curie:ft-user-1234", model_list->models[0]->id);
    TEST_ASSERT_EQUAL_STRING("user", model_list->models[0]->object);
    TEST_ASSERT_EQUAL_STRING("user_1234", model_list->models[0]->owned_by);
    TEST_ASSERT_EQUAL_STRING("read", model_list->models[0]->permissions[0]);
    TEST_ASSERT_EQUAL_STRING("write", model_list->models[0]->permissions[1]);
    TEST_ASSERT_EQUAL_STRING("curie:ft-user-1234", model_list->models[1]->id);
    TEST_ASSERT_EQUAL_STRING("user", model_list->models[1]->object);
    TEST_ASSERT_EQUAL_STRING("user_1234", model_list->models[1]->owned_by);
    TEST_ASSERT_EQUAL_STRING("read", model_list->models[1]->permissions[0]);
    TEST_ASSERT_EQUAL_STRING("write", model_list->models[1]->permissions[1]);
    openai_model_list_free(model_list);
}

void test_get_models() {
    OPENAI_API *api = openai_init("http://localhost:8080", "secret_key");
    OpenAiModelList *model_list = openai_model_list_new();
    long response_code = openai_get_models(api, model_list);
    TEST_ASSERT_EQUAL_INT(200, response_code);
    TEST_ASSERT_NOT_NULL(model_list);
    TEST_ASSERT_NOT_NULL(model_list->models);
    TEST_ASSERT_NOT_NULL(model_list->object);
    TEST_ASSERT_EQUAL_STRING("list", model_list->object);
    TEST_ASSERT_EQUAL_STRING("curie:ft-user-1234", model_list->models[0]->id);
    TEST_ASSERT_EQUAL_STRING("user", model_list->models[0]->object);
    TEST_ASSERT_EQUAL_STRING("user_1234", model_list->models[0]->owned_by);
    TEST_ASSERT_EQUAL_STRING("read", model_list->models[0]->permissions[0]);
    TEST_ASSERT_EQUAL_STRING("write", model_list->models[0]->permissions[1]);
    TEST_ASSERT_EQUAL_STRING("curie:ft-user-1234", model_list->models[1]->id);
    TEST_ASSERT_EQUAL_STRING("user", model_list->models[1]->object);
    TEST_ASSERT_EQUAL_STRING("user_1234", model_list->models[1]->owned_by);
    TEST_ASSERT_EQUAL_STRING("read", model_list->models[1]->permissions[0]);
    TEST_ASSERT_EQUAL_STRING("write", model_list->models[1]->permissions[1]);
    openai_model_list_free(model_list);
    openai_free(api);
}

void test_get_model() {
    OPENAI_API *api = openai_init("http://localhost:8080", "secret_key");
    OpenAiModel *model = openai_model_new();
    long response_code = openai_get_model(api, model, "curie:ft-user-1234");
    TEST_ASSERT_EQUAL_INT(200, response_code);
    TEST_ASSERT_NOT_NULL(model);
    TEST_ASSERT_EQUAL_STRING("curie:ft-user-1234", model->id);
    TEST_ASSERT_EQUAL_STRING("user", model->object);
    TEST_ASSERT_EQUAL_STRING("user_1234", model->owned_by);
    TEST_ASSERT_EQUAL_STRING("read", model->permissions[0]);
    TEST_ASSERT_EQUAL_STRING("write", model->permissions[1]);
    openai_model_free(model);
    openai_free(api);
}

void test_create_completion() {
    OPENAI_API *api = openai_init("http://localhost:8080", "secret_key");
    OpenAiCompletion *completion = openai_completion_new();
    long response_code = openai_create_completion(api, completion, "{\"prompt\": \"Once upon a time\", \"max_tokens\": 5}");
    TEST_ASSERT_EQUAL_INT(200, response_code);
    TEST_ASSERT_NOT_NULL(completion);
    TEST_ASSERT_EQUAL_STRING("cmpl-1234", completion->id);
    TEST_ASSERT_EQUAL_STRING("text_completion", completion->object);
    TEST_ASSERT_EQUAL_STRING("curie:ft-user-1234", completion->model);
    TEST_ASSERT_EQUAL_INT(1612211368, completion->created);
    TEST_ASSERT_EQUAL_STRING("Once upon a time", completion->choices[0]->text);
    TEST_ASSERT_EQUAL_STRING("length", completion->choices[0]->finish_reason);
    TEST_ASSERT_EQUAL_INT(0, completion->choices[0]->index);
    openai_completion_free(completion);
    openai_free(api);
}

int main() {
    MockServerRequestHandlerData *hd[] = {
            ms_create_handler_data("/models", "{\"data\": [{\"id\": \"curie:ft-user-1234\", \"object\": \"user\", \"owned_by\": \"user_1234\", \"permissions\": [\"read\", \"write\"]}, {\"id\": \"curie:ft-user-1234\", \"object\": \"user\", \"owned_by\": \"user_1234\", \"permissions\": [\"read\", \"write\"]}], \"object\": \"list\"}", 200),
            ms_create_handler_data("/models/curie:ft-user-1234", "{\"id\": \"curie:ft-user-1234\", \"object\": \"user\", \"owned_by\": \"user_1234\", \"permissions\": [\"read\", \"write\"]}", 200),
            ms_create_handler_data("/chat/completions", "{\"id\": \"cmpl-1234\", \"object\": \"text_completion\", \"created\": 1612211368, \"model\": \"curie:ft-user-1234\", \"choices\": [{\"text\": \"Once upon a time\", \"index\": 0, \"logprobs\": null, \"finish_reason\": \"length\"}]}", 200),
            NULL
    };

    struct MHD_Daemon *daemon = create_mock_server(hd);
    UNITY_BEGIN();
    RUN_TEST(test_get_models);
    RUN_TEST(test_get_model);
    RUN_TEST(test_openai_model_new);
    RUN_TEST(test_openai_model_list_new);
    RUN_TEST(test_openai_model_from_json_string);
    RUN_TEST(test_openai_parse_model_list_string);
    RUN_TEST(test_create_completion);
    for (int i = 0; hd[i] != NULL; i++) {
        ms_free_handler_data(hd[i]);
    }
    MHD_stop_daemon(daemon);
    return UNITY_END();

}