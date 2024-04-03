#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include "chat.h"
#include "util.h"
#include "appdata.h"
#include "cJSON.h"
#include "openai.h"

CGptChat *cgpt_chat_new() {
    CGptChat *chat = malloc(sizeof(CGptChat));
    chat->id = 0;
    chat->messages = NULL;
    chat->messages_count = 0;
    chat->completions = NULL;
    chat->completions_count = 0;
    chat->model = NULL;
    return chat;
}

void cgpt_chat_free(CGptChat *chat) {
    if (chat->messages != NULL) {
        for (int i = 0; chat->messages[i] != NULL; i++) {
            openai_message_free(chat->messages[i]);
        }
    }
    if (chat->completions != NULL) {
        for (int i = 0; chat->completions[i] != NULL; i++) {
            openai_completion_free(chat->completions[i]);
        }
    }
    FREE(chat->model)
    FREE(chat)
}

/**
 * Assigns the values from the json object to the chat object.
 * Will allocate memory for the messages and completions from the json object.
 * @param chat The chat object.
 * @param chat_json The json object.
 */
static void cgpt_chat_assign_from_json(CGptChat *chat, cJSON* chat_json) {
    chat->id = cjson_int_at(chat_json, "id");
    chat->model = cjson_string_at(chat_json, "model");

    cJSON *messages = cJSON_GetObjectItemCaseSensitive(chat_json, "messages");
    size_t messages_count = cJSON_GetArraySize(messages);
    if (messages_count > 0) {
        chat->messages = malloc(sizeof(OpenAiMessage *) * (messages_count + 1));
        int i = 0;
        cJSON *message;
        cJSON_ArrayForEach(message, messages) {
            chat->messages[i] = openai_message_from_json(message);
            i++;
        }
        chat->messages[i] = NULL;
    }

    cJSON *completions = cJSON_GetObjectItemCaseSensitive(chat_json, "completions");
    size_t completions_count = cJSON_GetArraySize(completions);
    if (completions_count > 0) {
        chat->completions = malloc(sizeof(OpenAiCompletion *) * (completions_count + 1));
        int i = 0;
        cJSON *completion;
        cJSON_ArrayForEach(completion, completions) {
            chat->completions[i] = openai_completion_from_json(completion);
            i++;
        }
        chat->completions[i] = NULL;
    }

    chat->messages_count = (int) messages_count;
    chat->completions_count = (int) completions_count;
}

static void cgpt_chat_add_completion(CGptChat *chat, OpenAiCompletion *completion) {
    if (chat->completions == NULL) {
        chat->completions = malloc(sizeof(OpenAiCompletion *) * 2);
        chat->completions[0] = completion;
        chat->completions[1] = NULL;
        chat->completions_count = 1;
        return;
    }

    int index = chat->completions_count;
    chat->completions = realloc(chat->completions, sizeof(OpenAiCompletion *) * (index + 2));
    chat->completions[index] = completion;
    chat->completions[index + 1] = NULL;
    chat->completions_count += 1;
}

static void cgpt_chat_add_message(CGptChat *chat, OpenAiMessage *message) {
    if (chat->messages == NULL) {
        chat->messages = malloc(sizeof(OpenAiMessage *) * 2);
        chat->messages[0] = message;
        chat->messages[1] = NULL;
        chat->messages_count = 1;
        return;
    }

    int index = chat->messages_count;
    chat->messages = realloc(chat->messages, sizeof(OpenAiMessage *) * (index + 2));
    chat->messages[index] = message;
    chat->messages[index + 1] = NULL;
    chat->messages_count += 1;
}

void cgpt_chat_assign_from_json_string(CGptChat *chat, char *json_string) {
    cJSON *chat_json = cJSON_Parse(json_string);
    cgpt_chat_assign_from_json(chat, chat_json);
}

CGptChat *cgpt_chat_from_json(cJSON *chat_json) {
    CGptChat *chat = cgpt_chat_new();
    cgpt_chat_assign_from_json(chat, chat_json);
    return chat;
}

CGptChat *cgpt_chat_from_json_string(char *json_string) {
    cJSON *chat_json = cJSON_Parse(json_string);
    CGptChat *chat = cgpt_chat_from_json(chat_json);
    return chat;
}

static FILE *open_chat_file(int id, char *mode) {
    if (!data_dir_exists()) {
        return NULL;
    }

    char chat_file_name[CGPT_MAX_FILE_PATH];
    sprintf(chat_file_name, "%d.json", id);

    char chat_file_path[CGPT_MAX_FILE_PATH];
    file_path_in_data_dir(chat_file_name, chat_file_path);
    FILE *chat_file = fopen(chat_file_path, mode);
    return chat_file;
}

int cgpt_chat_load(int id, CGptChat *chat) {
    FILE *chat_file = open_chat_file(id, "r");

    if (chat_file == NULL) {
        return -1;
    }

    cJSON *parsed = parse_json_file(chat_file);

    if (parsed == NULL) {
        return -1;
    }
    cgpt_chat_assign_from_json(chat, parsed);
    cJSON_Delete(parsed);
    return id;
}

static cJSON *cgpt_chat_completions_to_json(CGptChat *chat) {
    cJSON *completions = cJSON_CreateArray();
    for (int i = 0; i < chat->completions_count; i++) {
        cJSON *completion = openai_completion_to_json(chat->completions[i]);
        cJSON_AddItemToArray(completions, completion);
    }
    return completions;
}

static cJSON *cgpt_chat_messages_to_json(CGptChat *chat) {
    cJSON *messages = cJSON_CreateArray();
    for (int i = 0; i < chat->messages_count; i++) {
        cJSON *message = openai_message_to_json(chat->messages[i]);
        cJSON_AddItemToArray(messages, message);
    }
    return messages;
}

cJSON *cgpt_chat_to_json(CGptChat *chat) {
    cJSON *chat_json = cJSON_CreateObject();
    cJSON_AddNumberToObject(chat_json, "id", chat->id);
    cJSON_AddStringToObject(chat_json, "model", chat->model);

    cJSON *messages = cgpt_chat_messages_to_json(chat);
    cJSON_AddItemToObject(chat_json, "messages", messages);

    cJSON *completions = cgpt_chat_completions_to_json(chat);
    cJSON_AddItemToObject(chat_json, "completions", completions);

    return chat_json;
}

char *cgpt_chat_create_completion_request_json(CGptChat *chat) {
    cJSON *request = cJSON_CreateObject();
    cJSON_AddStringToObject(request, "model", chat->model);

    cJSON *messages = cgpt_chat_messages_to_json(chat);
    cJSON_AddItemToObject(request, "messages", messages);

    char *json = cJSON_Print(request);
    cJSON_Delete(request);
    return json;
}

OpenAiMessage *cgpt_chat_new_message(CGptChat *chat, char *role, char *content) {
    OpenAiCompletion *completion = openai_completion_new();
    OpenAiMessage *message = openai_message_new();
    message->content = strdup(content);
    message->role = strdup(role);
    cgpt_chat_add_message(chat, message);
    char *request_json = cgpt_chat_create_completion_request_json(chat);
    openai_create_completion(cgpt_global_appdata_get_openai_api(), completion, request_json);
    cgpt_chat_add_completion(chat, completion);
    cgpt_chat_add_message(chat, completion->choices[0]->message);
    FREE(request_json)
    return completion->choices[0]->message;
}

bool cgpt_chat_save(CGptChat *chat) {
    FILE *chat_file = open_chat_file(chat->id, "w");
    if (chat_file == NULL) {
        return false;
    }

    cJSON *chat_json = cgpt_chat_to_json(chat);

    char *json = cJSON_Print(chat_json);
    cJSON_Delete(chat_json);

    fputs(json, chat_file);
    fclose(chat_file);
    FREE(json)
    return true;
}

