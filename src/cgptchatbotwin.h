#ifndef CGPTCHATBOT_CGPTCHATBOTWIN_H
#define CGPTCHATBOT_CGPTCHATBOTWIN_H

#include <gtk/gtk.h>
#include "cgptchatbotapp.h"

#define CGPT_CHATBOT_APP_WINDOW_TYPE (cgpt_chatbot_app_window_get_type ())
G_DECLARE_FINAL_TYPE (CgptChatbotAppWindow, cgpt_chatbot_app_window, CGPTCHATBOT, APP_WINDOW, GtkApplicationWindow)


CgptChatbotAppWindow *cgpt_chatbot_app_window_new(CgptChatbotApp *app);
void cgpt_chatbot_app_window_open(CgptChatbotAppWindow *win, GFile *file);

GtkStack *cgpt_chatbot_app_window_get_stack(CgptChatbotAppWindow *win);

#endif //CGPTCHATBOT_CGPTCHATBOTWIN_H
