#include "cgptchatbotwin.h"

struct _CgptChatbotAppWindow {
    GtkApplicationWindow parent;
    GtkStack *stack;
};

G_DEFINE_TYPE(CgptChatbotAppWindow, cgpt_chatbot_app_window, GTK_TYPE_APPLICATION_WINDOW)

static void cgpt_chatbot_app_window_init(CgptChatbotAppWindow *win) {
    gtk_widget_init_template(GTK_WIDGET(win));
}

static void cgpt_chatbot_app_window_class_init(CgptChatbotAppWindowClass *class) {
    gtk_widget_class_set_template_from_resource(GTK_WIDGET_CLASS(class), "/org/tracksuitdev/cgptchatbot/ui/window.ui");
    gtk_widget_class_bind_template_child(GTK_WIDGET_CLASS(class), CgptChatbotAppWindow, stack);
}

CgptChatbotAppWindow *cgpt_chatbot_app_window_new(CgptChatbotApp *app) {
    return g_object_new(CGPT_CHATBOT_APP_WINDOW_TYPE,"application", app, NULL);
}

GtkStack *cgpt_chatbot_app_window_get_stack(CgptChatbotAppWindow *win) {
    return win->stack;
}

void cgpt_chatbot_app_window_open(CgptChatbotAppWindow *win, GFile *file) {

}
