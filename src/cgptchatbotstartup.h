#ifndef CGPTCHATBOT_CGPTCHATBOTSTARTUP_H
#define CGPTCHATBOT_CGPTCHATBOTSTARTUP_H

#include <gtk/gtk.h>
#include "cgptchatbotwin.h"

#define CGPT_CHATBOT_STARTUP_TYPE (cgpt_chatbot_startup_get_type ())
G_DECLARE_FINAL_TYPE (CgptChatbotStartup, cgpt_chatbot_startup, CGPTCHATBOT, STARTUP, GtkDialog)

CgptChatbotStartup *cgpt_chatbot_startup_new(CgptChatbotAppWindow *win);

#endif //CGPTCHATBOT_CGPTCHATBOTSTARTUP_H
