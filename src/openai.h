#ifndef CGPTCHATBOT_OPENAI_H
#define CGPTCHATBOT_OPENAI_H

#include <stdbool.h>
#include "cJSON.h"

typedef struct {
    char *auth_header;
    char *models_url;
    char *completions_url;
} OPENAI_API;

typedef struct {
    char *id;
    char *object;
    char *owned_by;
    char **permissions;
} OpenAiModel;

OpenAiModel *openai_model_new();
void openai_model_free(OpenAiModel *model);
void openai_model_assign_from_json(OpenAiModel *model, cJSON *model_json);
void openai_model_assign_from_json_string(OpenAiModel *model, char *json_string);
OpenAiModel *openai_model_from_json_string(char* json_string);
OpenAiModel *openai_model_from_json(cJSON *model_json);

typedef struct {
    OpenAiModel **models;
    char *object;
} OpenAiModelList;

OpenAiModelList *openai_model_list_new();
void openai_model_list_free(OpenAiModelList *model_list);
void openai_model_list_assign_from_json(OpenAiModelList *model_list, cJSON *model_list_json);
void openai_model_list_assign_from_json_string(OpenAiModelList *model_list, char *json_string);
OpenAiModelList *openai_model_list_from_json_string(char* json_string);
OpenAiModelList *openai_model_list_from_json(cJSON *model_list_json);

typedef struct {
    char *role;
    char *name;
    char *function_call;
    char *content;
} OpenAiMessage;
OpenAiMessage *openai_message_new();
void openai_message_free(OpenAiMessage *message);
void openai_message_assign_from_json(OpenAiMessage *message, cJSON *message_json);
void openai_message_assign_from_json_string(OpenAiMessage *message, char *json_string);
OpenAiMessage *openai_message_from_json_string(char* json_string);
OpenAiMessage *openai_message_from_json(cJSON *message_json);
cJSON *openai_message_to_json(OpenAiMessage *message);

typedef struct {
    int index;
    OpenAiMessage *message;
    char *finish_reason;
    int logprobs;
} OpenAiChoice;
OpenAiChoice *openai_choice_new();
void openai_choice_free(OpenAiChoice *choice);
cJSON *openai_choice_to_json(OpenAiChoice *choice);

typedef struct {
   int prompt_tokens;
   int completion_tokens;
   int total_tokens;
} OpenAiUsage;
OpenAiUsage *openai_usage_new();


typedef struct {
    char *id;
    char *object;
    long created;
    char *model;
    OpenAiChoice **choices;
    OpenAiUsage *usage;
} OpenAiCompletion;
OpenAiCompletion *openai_completion_new();
void openai_completion_free(OpenAiCompletion *completion);
void openai_completion_assign_from_json(OpenAiCompletion *completion, cJSON *completion_json);
void openai_completion_assign_from_json_string(OpenAiCompletion *completion, char *json_string);
OpenAiCompletion *openai_completion_from_json_string(char* json_string);
OpenAiCompletion *openai_completion_from_json(cJSON *completion_json);
cJSON *openai_completion_to_json(OpenAiCompletion *completion);


OPENAI_API *openai_init(const char *url, const char *api_key);
void openai_free(OPENAI_API *openai_api);

long openai_get_models(OPENAI_API *api, OpenAiModelList *model_list);
long openai_get_model(OPENAI_API *api, OpenAiModel *model, const char *model_id);
long openai_create_completion(OPENAI_API *api, OpenAiCompletion *completion, const char *json_body);
size_t curl_write_function(void *ptr, size_t size, size_t nmemb, void *userdata);


#endif //CGPTCHATBOT_OPENAI_H
