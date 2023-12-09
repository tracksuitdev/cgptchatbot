#include <gtk/gtk.h>
#include "cgptchatbotstartup.h"
#include "appdata.h"

struct _CgptChatbotStartup {
    GtkDialog parent;
    GtkEntry *api_key_entry;
};

G_DEFINE_TYPE (CgptChatbotStartup, cgpt_chatbot_startup, GTK_TYPE_DIALOG)

static void save_button_clicked(GtkButton *button, CgptChatbotStartup *self) {
    const char *api_key = gtk_entry_buffer_get_text(gtk_entry_get_buffer(self->api_key_entry));
    if (strlen(api_key) > 0) {
        save_api_key(api_key);
        init_appdata();
        gtk_window_close(GTK_WINDOW (self));
    }
}

static void cgpt_chatbot_startup_init(CgptChatbotStartup *self) {
    gtk_widget_init_template(GTK_WIDGET (self));
}

static void cgpt_chatbot_startup_class_init(CgptChatbotStartupClass *class) {
    gtk_widget_class_set_template_from_resource(GTK_WIDGET_CLASS (class),
                                                "/org/tracksuitdev/cgptchatbot/ui/startup.ui");
    gtk_widget_class_bind_template_child(GTK_WIDGET_CLASS(class), CgptChatbotStartup, api_key_entry);

    gtk_widget_class_bind_template_callback(GTK_WIDGET_CLASS(class), save_button_clicked);
}

CgptChatbotStartup *cgpt_chatbot_startup_new(CgptChatbotAppWindow *win) {
    return g_object_new(CGPT_CHATBOT_STARTUP_TYPE, "transient-for", win, "use-header-bar",
                        TRUE, NULL);
}