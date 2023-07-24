#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include "chat.h"
#include "util.h"
#include "appdata.h"
#include "cJSON.h"

CGptChat *cgpt_chat_new() {
    CGptChat *chat = malloc(sizeof(CGptChat));
    chat->id = 0;
    chat->messages = NULL;
    chat->messages_count = 0;
    chat->completions = NULL;
    chat->completions_count = 0;
    return chat;
}

void cgpt_chat_free(CGptChat *chat) {
    free_array((void **) chat->messages);
    free_array((void **) chat->completions);
    FREE(chat)
}

void cgpt_chat_assign_from_json(CGptChat *chat, cJSON* chat_json) {
    chat->id = cjson_int_at(chat_json, "id");

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

FILE *open_chat_file(int id, char *mode) {
    if (!data_dir_exists()) {
        return NULL;
    }

    char chat_file_path[CGPT_MAX_FILE_PATH];
    data_dir_path(chat_file_path);
    snprintf(chat_file_path, CGPT_MAX_FILE_PATH, "%s/%d.json", chat_file_path, id);
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

cJSON *cgpt_chat_to_json(CGptChat *chat) {
    cJSON *chat_json = cJSON_CreateObject();
    cJSON_AddNumberToObject(chat_json, "id", chat->id);

    cJSON *messages = cJSON_CreateArray();
    cJSON_AddItemToObject(chat_json, "messages", messages);
    for (int i = 0; i < chat->messages_count; i++) {
        cJSON *message = openai_message_to_json(chat->messages[i]);
        cJSON_AddItemToArray(messages, message);
    }

    cJSON *completions = cJSON_CreateArray();
    cJSON_AddItemToObject(chat_json, "completions", completions);
    for (int i = 0; i < chat->completions_count; i++) {
        cJSON *completion = openai_completion_to_json(chat->completions[i]);
        cJSON_AddItemToArray(completions, completion);
    }

    return chat_json;
}

bool cgpt_chat_save(CGptChat *chat) {
    FILE *chat_file = open_chat_file(chat->id, "w");
    if (chat_file == NULL) {
        return false;
    }

    cJSON *chat_json = cJSON_CreateObject();
    cJSON_AddNumberToObject(chat_json, "id", chat->id);

    cJSON *messages = cJSON_CreateArray();
    cJSON_AddItemToObject(chat_json, "messages", messages);
    for (int i = 0; i < chat->messages_count; i++) {
        cJSON *message = openai_message_to_json(chat->messages[i]);
        cJSON_AddItemToArray(messages, message);
    }

    cJSON *completions = cJSON_CreateArray();
    cJSON_AddItemToObject(chat_json, "completions", completions);
    for (int i = 0; i < chat->completions_count; i++) {
        cJSON *completion = openai_completion_to_json(chat->completions[i]);
        cJSON_AddItemToArray(completions, completion);
    }

    char *json = cJSON_Print(chat_json);
    cJSON_Delete(chat_json);

    fputs(json, chat_file);
    fclose(chat_file);
    FREE(json)
    return true;
}

bool cgpt_chat_add_completion(CGptChat *chat, OpenAiCompletion *completion) {
    if (chat->completions == NULL) {
        chat->completions = malloc(sizeof(OpenAiCompletion *) * 2);
        chat->completions[0] = completion;
        chat->completions[1] = NULL;
        chat->completions_count = 1;
        return true;
    }

    int index = chat->completions_count;
    chat->completions = realloc(chat->completions, sizeof(OpenAiCompletion *) * (index + 2));
    chat->completions[index] = completion;
    chat->completions[index + 1] = NULL;
    chat->completions_count += 1;
    return true;
}

bool cgpt_chat_add_message(CGptChat *chat, OpenAiMessage *message) {
    if (chat->messages == NULL) {
        chat->messages = malloc(sizeof(OpenAiMessage *) * 2);
        chat->messages[0] = message;
        chat->messages[1] = NULL;
        chat->messages_count = 1;
        return true;
    }

    int index = chat->messages_count;
    chat->messages = realloc(chat->messages, sizeof(OpenAiMessage *) * (index + 2));
    chat->messages[index] = message;
    chat->messages[index + 1] = NULL;
    chat->messages_count += 1;
    return true;
}
