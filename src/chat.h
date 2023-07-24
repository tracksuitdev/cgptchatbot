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
} CGptChat;


CGptChat *cgpt_chat_new();
void cgpt_chat_free(CGptChat *chat);
cJSON *cgpt_chat_to_json(CGptChat *chat);

int cgpt_chat_load(int id, CGptChat *chat);
bool cgpt_chat_save(CGptChat *chat);

bool cgpt_chat_add_completion(CGptChat *chat, OpenAiCompletion *completion);
bool cgpt_chat_add_message(CGptChat *chat, OpenAiMessage *message);


#endif //CGPTCHATBOT_CHAT_H
