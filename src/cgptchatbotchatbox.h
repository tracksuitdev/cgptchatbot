#ifndef CGPTCHATBOT_CGPTCHATBOTCHATBOX_H
#define CGPTCHATBOT_CGPTCHATBOTCHATBOX_H

#include <gtk/gtk.h>

#define CGPT_CHATBOT_CHAT_BOX_TYPE (cgpt_chatbot_chatbox_get_type ())
G_DECLARE_FINAL_TYPE (CgptChatbotChatbox, cgpt_chatbot_chatbox, CGPTCHATBOT, CHATBOX, GtkBox)

CgptChatbotChatbox *cgpt_chatbot_chatbox_new();

#endif //CGPTCHATBOT_CGPTCHATBOTCHATBOX_H
