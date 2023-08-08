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
    CgptChatbotAppWindow *win;
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

static void preferences_activated(GSimpleAction *action, GVariant *parameter, gpointer app) {
    g_print("Preferences activated\n");
}

static void quit_activated(GSimpleAction *action, GVariant *parameter, gpointer app) {
    g_application_quit(G_APPLICATION(app));
}

static GActionEntry app_entries[] = {
        {"preferences", preferences_activated, NULL, NULL, NULL},
        {"quit",        quit_activated,        NULL, NULL, NULL}
};

static void cgpt_chatbot_app_startup(GApplication *app) {
    G_APPLICATION_CLASS(cgpt_chatbot_app_parent_class)->startup(app);
    gtk_application_set_accels_for_action(GTK_APPLICATION(app), "app.quit", (const gchar *[]){"<Ctrl>Q", NULL});
    g_action_map_add_action_entries(G_ACTION_MAP(app), app_entries, G_N_ELEMENTS(app_entries), app);
}


static void cgpt_chatbot_app_class_init(CgptChatbotAppClass *class) {
    G_APPLICATION_CLASS(class)->startup = cgpt_chatbot_app_startup;
    G_APPLICATION_CLASS(class)->activate = cgpt_chatbot_app_activate;
    G_APPLICATION_CLASS(class)->open = cgpt_chatbot_app_open;
}

CgptChatbotApp *cgptchatbot_app_new(void) {
    return g_object_new(CGPTCHATBOT_APP_TYPE,
                        "application-id", "org.tracksuitdev.cgptchatbot",
                        "flags", G_APPLICATION_HANDLES_OPEN,
                        NULL);
}



