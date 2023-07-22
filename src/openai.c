#include <stdlib.h>
#include <curl/curl.h>
#include <string.h>
#include "openai.h"
#include "cJSON.h"
#include "util.h"
#include <stdarg.h>

static const char AUTH_HEADER[] = "Authorization: Bearer ";
static const char MODELS[] = "/models";
static const char COMPLETIONS[] = "/chat/completions";

static char *concat(int num_args, const char *s, ...) {
    va_list args;
    va_start(args, s);
    size_t len = strlen(s);
    for (int i = 0; i < num_args - 1; i++) {
        char *arg = va_arg(args, char *);
        len += strlen(arg);
    }
    va_end(args);
    char *result = malloc(len + 1);
    strcpy(result, s);
    va_start(args, s);
    for (int i = 0; i < num_args -1; i++) {
        strcat(result, va_arg(args, char *));
    }
    va_end(args);
    return result;
}

static char* string_at(cJSON *json, char *key) {
    cJSON *value = cJSON_GetObjectItem(json, key);
    if (value != NULL) {
        return strdup(value->valuestring);
    } else {
        return NULL;
    }
}

static int int_at(cJSON *json, char *key) {
    cJSON *value = cJSON_GetObjectItem(json, key);
    if (value != NULL) {
        return (int) value->valuedouble;
    } else {
        return 0;
    }
}

static long long_at(cJSON *json, const char *key) {
    cJSON *value = cJSON_GetObjectItem(json, key);
    if (value != NULL) {
        return (long) value->valuedouble;
    } else {
        return 0;
    }
}

OPENAI_API *openai_init(const char *url, const char *api_key) {
    char *auth_header = concat(2, AUTH_HEADER, api_key);
    char *models_url = concat(2,url, MODELS);
    char *completions_url = concat(2,url, COMPLETIONS);

    OPENAI_API *api = malloc(sizeof (OPENAI_API));
    api->auth_header = auth_header;
    api->models_url = models_url;
    api->completions_url = completions_url;
    return api;
}

void openai_free(OPENAI_API *api) {
    FREE(api->auth_header)
    FREE(api->models_url)
    FREE(api->completions_url)
    FREE(api)
}

OpenAiModel *openai_model_new() {
    OpenAiModel *model = malloc(sizeof(OpenAiModel));
    model->id = NULL;
    model->object = NULL;
    model->owned_by = NULL;
    model->permissions = NULL;
    return model;
}


void openai_model_free(OpenAiModel *model) {
    FREE(model->id);
    FREE(model->object);
    FREE(model->owned_by);
    free_array((void **) model->permissions);
    FREE(model);
    model = NULL;
}

OpenAiModelList *openai_model_list_new() {
    OpenAiModelList *model_list = malloc(sizeof(OpenAiModelList));
    model_list->models = NULL;
    model_list->object = NULL;
    return model_list;
}

void openai_model_list_free(OpenAiModelList *model_list) {
    free_array((void **) model_list->models);
    FREE(model_list->object);
    FREE(model_list);
    model_list = NULL;
}

OpenAiMessage *openai_message_new() {
    OpenAiMessage *message = malloc(sizeof(OpenAiMessage));
    message->role = NULL;
    message->name = NULL;
    message->function_call = NULL;
    message->content = NULL;
    return message;
}

void openai_message_free(OpenAiMessage *message) {
    FREE(message->role)
    FREE(message->name)
    FREE(message->function_call)
    FREE(message->content)
    FREE(message)
}

OpenAiChoice *openai_choice_new() {
    OpenAiChoice *choice = malloc(sizeof(OpenAiChoice));
    choice->index = 0;
    choice->message = *openai_message_new();
    choice->finish_reason = NULL;
    choice->text = NULL;
    choice->logprobs = 0;
    return choice;
}

void openai_choice_free(OpenAiChoice *choice) {
    FREE(choice->finish_reason)
    FREE(choice->text)
    FREE(choice)
}

OpenAiUsage *openai_usage_new() {
    OpenAiUsage *usage = malloc(sizeof(OpenAiUsage));
    usage->prompt_tokens = 0;
    usage->completion_tokens = 0;
    usage->total_tokens = 0;
    return usage;
}

OpenAiCompletion *openai_completion_new() {
    OpenAiCompletion *completion = malloc(sizeof(OpenAiCompletion));
    completion->id = NULL;
    completion->object = NULL;
    completion->created = 0;
    completion->model = NULL;
    completion->choices = NULL;
    completion->usage = NULL;
    return completion;
}

void openai_completion_free(OpenAiCompletion *completion) {
    FREE(completion->id)
    FREE(completion->object)
    FREE(completion->model)
    free_array((void **) completion->choices);
    FREE(completion->usage)
    FREE(completion)
}

typedef struct {
    char *response;
    size_t size;
} CurlResponseMemory;

size_t curl_write_function(void *data, size_t size, size_t nmemb, void *userdata) {
    size_t real_size = size * nmemb;
    CurlResponseMemory *mem = (CurlResponseMemory *)userdata;

    char *ptr = realloc(mem->response, mem->size + real_size + 1);
    if (ptr == NULL)
        return 0;  /* out of memory! */

    mem->response = ptr;
    memcpy(&(mem->response[mem->size]), data, real_size);
    mem->size += real_size;
    mem->response[mem->size] = 0;

    return real_size;
}


static struct curl_slist *get_headers(char *auth_header) {
    struct curl_slist *headers = NULL;
    headers = curl_slist_append(headers, "Content-Type: application/json");
    headers = curl_slist_append(headers, auth_header);
    return headers;
}

void openai_model_assign_from_json(OpenAiModel *model, cJSON *model_json) {
    model->id = string_at(model_json, "id");
    model->object = string_at(model_json, "object");
    model->owned_by = string_at(model_json, "owned_by");
    cJSON *permissions = cJSON_GetObjectItem(model_json, "permissions");
    int permissions_size = cJSON_GetArraySize(permissions);
    if (permissions != NULL && permissions_size > 0) {
        cJSON *permissions_element;
        char **permissions_array = malloc(sizeof(char *) * (cJSON_GetArraySize(permissions) + 1));
        int i = 0;
        cJSON_ArrayForEach(permissions_element, permissions) {
            permissions_array[i] = strdup(permissions_element->valuestring);
            i++;
        }
        permissions_array[i] = NULL;
        model->permissions = permissions_array;
    } else {
        model->permissions = NULL;
    }
}

void openai_model_assign_from_json_string(OpenAiModel *model, char *json_string) {
    cJSON *model_json = cJSON_Parse(json_string);
    openai_model_assign_from_json(model, model_json);
}

OpenAiModel *openai_model_from_json(cJSON* model_json) {
    OpenAiModel *model = openai_model_new();
    openai_model_assign_from_json(model, model_json);
    return model;
}

OpenAiModel *openai_model_from_json_string(char* json_string) {
    return openai_model_from_json(cJSON_Parse(json_string));
}

void openai_model_list_assign_from_json(OpenAiModelList *model_list, cJSON *model_list_json) {
    cJSON *models = cJSON_GetObjectItem(model_list_json, "data");
    cJSON *model_element;
    int models_size = cJSON_GetArraySize(models);
    if (models_size > 0) {
        model_list->models = malloc(sizeof(OpenAiModel *) * (cJSON_GetArraySize(models) + 1));
        int i = 0;
        cJSON_ArrayForEach(model_element, models) {
            model_list->models[i] = openai_model_from_json(model_element);
            i++;
        }
        model_list->models[i] = NULL;
    } else {
        model_list->models = NULL;
    }
    model_list->object = string_at(model_list_json, "object");
}

void openai_model_list_assign_from_json_string(OpenAiModelList *model_list, char *json_string) {
    openai_model_list_assign_from_json(model_list, cJSON_Parse(json_string));
}

OpenAiModelList *openai_model_list_from_json(cJSON* model_list_json) {
    OpenAiModelList *model_list = openai_model_list_new();
    openai_model_list_assign_from_json(model_list, model_list_json);
    return model_list;
}

OpenAiModelList *openai_model_list_from_json_string(char *json_string) {
    return openai_model_list_from_json(cJSON_Parse(json_string));
}



void openai_choice_assign_from_json(OpenAiChoice *choice, cJSON *choice_json) {
    if (choice_json == NULL) {
        return;
    }
    choice->text = string_at(choice_json, "text");
    choice->index = int_at(choice_json, "index");
    choice->finish_reason = string_at(choice_json, "finish_reason");
    choice->logprobs = int_at(choice_json, "logprobs");
}

void openai_choice_assign_from_json_string(OpenAiChoice *choice, char *choice_json_string) {
    cJSON *choice_json = cJSON_Parse(choice_json_string);
    openai_choice_assign_from_json(choice, choice_json);
}

OpenAiChoice *openai_choice_from_json(cJSON* choice_json) {
    OpenAiChoice *choice = openai_choice_new();
    openai_choice_assign_from_json(choice, choice_json);
    return choice;
}

OpenAiChoice *openai_choice_from_json_string(char *json_string) {
    return openai_choice_from_json(cJSON_Parse(json_string));
}

void openai_usage_assign_from_json(OpenAiUsage *usage, cJSON *usage_json) {
    if (usage_json == NULL) {
        return;
    }
    usage->prompt_tokens = int_at(usage_json, "prompt_tokens");
    usage->completion_tokens = int_at(usage_json, "completion_tokens");
    usage->total_tokens = int_at(usage_json, "total_tokens");
}

OpenAiUsage *openai_usage_from_json(cJSON* usage_json) {
    OpenAiUsage *usage = openai_usage_new();
    openai_usage_assign_from_json(usage, usage_json);
    return usage;
}

OpenAiUsage *openai_usage_from_json_string(char *json_string) {
    return openai_usage_from_json(cJSON_Parse(json_string));
}

void openai_completion_assign_from_json(OpenAiCompletion *completion, cJSON *completion_json) {
    if (completion_json == NULL) {
        return;
    }
    completion->id = string_at(completion_json, "id");
    completion->object = string_at(completion_json, "object");
    completion->created = long_at(completion_json, "created");
    completion->model = string_at(completion_json, "model");
    cJSON *choices = cJSON_GetObjectItem(completion_json, "choices");
    cJSON *choice_element;
    int choices_size = cJSON_GetArraySize(choices);
    if (choices_size > 0) {
        completion->choices = malloc(sizeof(OpenAiChoice *) * (cJSON_GetArraySize(choices) + 1));
        int i = 0;
        cJSON_ArrayForEach(choice_element, choices) {
            completion->choices[i] = openai_choice_from_json(choice_element);
            i++;
        }
        completion->choices[i] = NULL;
    } else {
        completion->choices = NULL;
    }
    cJSON *usage = cJSON_GetObjectItem(completion_json, "usage");
    if (usage != NULL) {
        completion->usage = openai_usage_from_json(usage);
    } else {
        completion->usage = NULL;
    }
}

typedef struct {
    cJSON *response;
    long response_code;
} Response;

static Response perform_request(char *url, char *auth_header, const char *data) {
    CURL *curl = curl_easy_init();

    curl_easy_setopt(curl, CURLOPT_URL, url);

    struct curl_slist *headers = get_headers(auth_header);
    curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);

    CurlResponseMemory response_memory = {0};
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, (void *)&response_memory);
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, curl_write_function);

    if (data != NULL) {
        curl_easy_setopt(curl, CURLOPT_POSTFIELDS, data);
        curl_easy_setopt(curl, CURLOPT_POSTFIELDSIZE, strlen(data) + 1);
    }

    curl_easy_perform(curl);

    long response_code;
    curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &response_code);
    cJSON *json = cJSON_Parse(response_memory.response);
    Response response = {json, response_code};

    curl_slist_free_all(headers);
    curl_easy_cleanup(curl);
    FREE(response_memory.response);
    return response;
}



static Response get(char *url, char *auth_header) {
    return perform_request(url, auth_header, NULL);
}

static Response post(char *url, char *auth_header, const char *data) {
    return perform_request(url, auth_header, data);
}

long openai_get_models(OPENAI_API *api, OpenAiModelList *model_list) {
    Response response = get(api->models_url, api->auth_header);

    openai_model_list_assign_from_json(model_list, response.response);

    cJSON_Delete(response.response);

    return response.response_code;
}

long openai_get_model(OPENAI_API *api, OpenAiModel *model, const char *model_id) {
    char *url = concat(3,api->models_url, "/", model_id);

    Response response = get(url, api->auth_header);

    openai_model_assign_from_json(model, response.response);

    cJSON_Delete(response.response);
    
    FREE(url)

    return response.response_code;
}

long openai_create_completion(OPENAI_API *api, OpenAiCompletion *completion, const char *json_body) {
    Response response = post(api->completions_url, api->auth_header, json_body);

    openai_completion_assign_from_json(completion, response.response);

    cJSON_Delete(response.response);

    return response.response_code;
}

