#ifndef CGPTCHATBOT_CHAT_H
#define CGPTCHATBOT_CHAT_H
#include <stdbool.h>
#include "openai.h"

typedef struct {
    int id;
    OpenAiMessage **messages;
    int messages_count;
    OpenAiCompletion **completions;
    int completions_count;
    char *model;
} CGptChat;


CGptChat *cgpt_chat_new();
void cgpt_chat_free(CGptChat *chat);
cJSON *cgpt_chat_completions_to_json(CGptChat *chat);
cJSON *cgpt_chat_messages_to_json(CGptChat *chat);
cJSON *cgpt_chat_to_json(CGptChat *chat);
/**
 * Creates a request for openai chat completion api. All the messages in the chat will be included in the request.
 * Make sure to add the user message to chat before calling this function.
 * <pre>
 *  {
 *    "model": chat->model,
 *    "messages": [...chat->messages, {"role": role, "content": text}]
 *  }
 * </pre>
 * @param chat The chat object.
 * @return The json string of the request. The caller should free the memory.
 */
char *cgpt_chat_create_completion_request_json(CGptChat *chat);

/**
 * Creates a new message in the chat and sends it to openai chat completion api.
 * The completion will be added to the chat and the response message will be added to the chat.
 * @param chat The chat object.
 * @param role The role of the message ("user", "assistant", "system").
 * @param content The content of the message.
 * @return The response message. Message is added to the chat so it will be freed when the chat is freed.
 */
OpenAiMessage *cgpt_chat_new_message(CGptChat *chat, char *role, char *content);

int cgpt_chat_load(int id, CGptChat *chat);
bool cgpt_chat_save(CGptChat *chat);

bool cgpt_chat_add_completion(CGptChat *chat, OpenAiCompletion *completion);
bool cgpt_chat_add_message(CGptChat *chat, OpenAiMessage *message);


#endif //CGPTCHATBOT_CHAT_H
