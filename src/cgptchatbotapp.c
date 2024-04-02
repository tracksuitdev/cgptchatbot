#include <gtk/gtk.h>
#include <stdio.h>
#include "cgptchatbotapp.h"
#include "cgptchatbotwin.h"
#include "appdata.h"
#include "cgptchatbotstartup.h"
#include "cgptchatbotchatbox.h"


struct _CgptChatbotApp {
    GtkApplication parent_instance;
};

G_DEFINE_TYPE(CgptChatbotApp, cgpt_chatbot_app, GTK_TYPE_APPLICATION)

static void cgpt_chatbot_app_init(CgptChatbotApp *app) {

}

static void load_css() {
    GtkCssProvider *provider = gtk_css_provider_new();
    gtk_css_provider_load_from_resource(provider, "/org/tracksuitdev/cgptchatbot/ui/style.css");
    gtk_style_context_add_provider_for_display(gdk_display_get_default(),
                                               GTK_STYLE_PROVIDER(provider),
                                               GTK_STYLE_PROVIDER_PRIORITY_APPLICATION);
}

static void cgpt_chatbot_app_activate(GApplication *app) {
    CgptChatbotAppWindow *win = cgpt_chatbot_app_window_new(CGPTCHATBOT_APP(app));
    gtk_window_present(GTK_WINDOW(win));
    load_css();
    if (!cgpt_global_appdata_has_api_key()) {
        CgptChatbotStartup *startup = cgpt_chatbot_startup_new(win);
        gtk_window_present(GTK_WINDOW(startup));
    }
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
    if (!cgpt_global_appdata_has_api_key()) {
        CgptChatbotStartup *startup = cgpt_chatbot_startup_new(win);
        gtk_window_present(GTK_WINDOW(startup));
    }
    load_css();
}

static void new_chat(GSimpleAction *action, GVariant *parameter, gpointer app) {
    CgptChatbotChatbox *chatbox = cgpt_chatbot_chatbox_new();
    CgptChatbotAppWindow *win = CGPTCHATBOT_APP_WINDOW(gtk_application_get_active_window(GTK_APPLICATION(app)));
    gtk_stack_add_titled(GTK_STACK(cgpt_chatbot_app_window_get_stack(win)),
                         GTK_WIDGET(chatbox), "new_chat", "New Chat");
}

static GActionEntry app_entries[] = {
        {"new", new_chat, NULL, NULL, NULL},
};

static void cgpt_chatbot_app_startup(GApplication *app) {
    G_APPLICATION_CLASS(cgpt_chatbot_app_parent_class)->startup(app);
    g_action_map_add_action_entries(G_ACTION_MAP(app), app_entries, G_N_ELEMENTS(app_entries), app);
    if (!data_dir_exists()) {
        create_data_dir();
    } else {
        init_appdata();
    }
}

static void cgpt_chatbot_app_shutdown(GApplication *app) {
    G_APPLICATION_CLASS(cgpt_chatbot_app_parent_class)->shutdown(app);
    cgpt_global_appdata_free();
}


static void cgpt_chatbot_app_class_init(CgptChatbotAppClass *class) {
    G_APPLICATION_CLASS(class)->startup = cgpt_chatbot_app_startup;
    G_APPLICATION_CLASS(class)->activate = cgpt_chatbot_app_activate;
    G_APPLICATION_CLASS(class)->open = cgpt_chatbot_app_open;
    G_APPLICATION_CLASS(class)->shutdown = cgpt_chatbot_app_shutdown;
}

CgptChatbotApp *cgptchatbot_app_new(void) {
    return g_object_new(CGPTCHATBOT_APP_TYPE,
                        "application-id", "org.tracksuitdev.cgptchatbot",
                        "flags", G_APPLICATION_HANDLES_OPEN,
                        NULL);
}



