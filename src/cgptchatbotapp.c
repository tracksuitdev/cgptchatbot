#include <gtk/gtk.h>
#include "cgptchatbotapp.h"
#include "cgptchatbotwin.h"
#include "appdata.h"

struct _CgptChatbotApp {
    GtkApplication parent_instance;
    APPDATA *appdata;
};

G_DEFINE_TYPE(CgptChatbotApp, cgpt_chatbot_app, GTK_TYPE_APPLICATION)

static void cgpt_chatbot_app_init(CgptChatbotApp *app) {

}

static void cgpt_chatbot_app_activate(GApplication *app) {
    CgptChatbotAppWindow *win = cgpt_chatbot_app_window_new(CGPTCHATBOT_APP(app));
    gtk_window_present(GTK_WINDOW(win));
}

static void cgpt_chatbot_app_open(GApplication *app, GFile **files, gint n_files, const gchar *hint) {
    CgptChatbotAppWindow *win = cgpt_chatbot_app_window_new(CGPTCHATBOT_APP(app));
    GList *windows = gtk_application_get_windows(GTK_APPLICATION(app));
    if (windows) {
        win = CGPTCHATBOT_APP_WINDOW(windows->data);
    } else {
        win = cgpt_chatbot_app_window_new(CGPTCHATBOT_APP(app));
    }
    for (int i = 0; i < n_files; i++) {
        cgpt_chatbot_app_window_open(win, files[i]);
    }
    gtk_window_present(GTK_WINDOW(win));
}

static void cgpt_chatbot_app_class_init(CgptChatbotAppClass *class) {
    G_APPLICATION_CLASS(class)->activate = cgpt_chatbot_app_activate;
    G_APPLICATION_CLASS(class)->open = cgpt_chatbot_app_open;
}

CgptChatbotApp *cgptchatbot_app_new(void) {
    return g_object_new(CGPTCHATBOT_APP_TYPE,
                        "application-id", "org.gtk.cgptchatbot",
                        "flags", G_APPLICATION_HANDLES_OPEN,
                        NULL);
}


