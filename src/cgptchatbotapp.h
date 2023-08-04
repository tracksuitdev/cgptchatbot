#ifndef CGPTCHATBOT_CGPTCHATBOTAPP_H
#define CGPTCHATBOT_CGPTCHATBOTAPP_H

#include <gtk/gtk.h>
#define CGPTCHATBOT_APP_TYPE (cgpt_chatbot_app_get_type ())
G_DECLARE_FINAL_TYPE (CgptChatbotApp, cgpt_chatbot_app, CGPTCHATBOT, APP, GtkApplication)


CgptChatbotApp    *cgptchatbot_app_new(void);


#endif //CGPTCHATBOT_CGPTCHATBOTAPP_H
