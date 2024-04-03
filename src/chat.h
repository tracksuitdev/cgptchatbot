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

/**
 * Creates a new chat object with NULL values.
 * @return The chat object.
 */
CGptChat *cgpt_chat_new();

/**
 * Frees the chat object and all the messages and completions in it. Model is also freed.
 * @param chat The chat object.
 */
void cgpt_chat_free(CGptChat *chat);

/**
 * Converts the chat object to a cJSON object.
 * @param chat chat struct
 * @return cJSON object
 */
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

/**
 * Load chat from the file storage with given id. The chats are stored as json files in the data directory with the id
 * as the filename, (e.g. 1.json).
 * @param id chat id
 * @param chat container for the chat
 * @return chat id if the chat was loaded successfully, -1 otherwise
 */
int cgpt_chat_load(int id, CGptChat *chat);

/**
 * Save the chat to the file storage with the id. The chats are stored as json files in the data directory with the id
 * as the filename, (e.g. 1.json).
 * @param chat chat to save
 * @return true if the chat was saved successfully, false otherwise
 */
bool cgpt_chat_save(CGptChat *chat);


#endif //CGPTCHATBOT_CHAT_H
