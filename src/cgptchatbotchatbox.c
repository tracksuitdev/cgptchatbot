#include <gtk/gtk.h>
#include <string.h>
#include "cgptchatbotchatbox.h"
#include "chat.h"
#include "openai.h"


struct _CgptChatbotChatbox {
    GtkBox parent;
    GtkTextView *chat_entry;
    GtkButton *send_button;
    GtkBox *messages;
    CGptChat *chat;
};

G_DEFINE_TYPE(CgptChatbotChatbox, cgpt_chatbot_chatbox, GTK_TYPE_BOX)

static void cgpt_chatbot_chatbox_init(CgptChatbotChatbox *self) {
    gtk_widget_init_template(GTK_WIDGET(self));
}

static void cgpt_chatbot_chatbox_dispose(GObject *object) {
    CgptChatbotChatbox *self = CGPTCHATBOT_CHATBOX(object);
    if (self->chat != NULL) {
        cgpt_chat_free(self->chat);
    }
    G_OBJECT_CLASS(cgpt_chatbot_chatbox_parent_class)->dispose(object);
}


static void cgpt_chatbot_chatbox_add_message(CgptChatbotChatbox *self, OpenAiMessage *message) {
    GtkWidget *label = gtk_label_new(message->content);
    if (strcmp(message->role, "user") == 0) {
        gtk_widget_set_halign(label, GTK_ALIGN_END);
    } else {
        gtk_widget_set_halign(label, GTK_ALIGN_START);
    }
    gtk_widget_set_valign(label, GTK_ALIGN_START);
    gtk_box_append(self->messages, label);
    gtk_widget_show(label);
}


static void send_button_clicked(GtkButton *button, CgptChatbotChatbox *self) {
    GtkTextBuffer *buffer = gtk_text_view_get_buffer(self->chat_entry);
    GtkTextIter start, end;
    gtk_text_buffer_get_start_iter(buffer, &start);
    gtk_text_buffer_get_end_iter(buffer, &end);
    char *text = gtk_text_buffer_get_text(buffer, &start, &end, FALSE);
    if (strlen(text) > 0) {
        cgpt_chatbot_chatbox_add_message(self, cgpt_chat_new_message(self->chat, "user", text));
    }
    g_free(text);
}

static void cgpt_chatbot_chatbox_class_init(CgptChatbotChatboxClass *class) {
    gtk_widget_class_set_template_from_resource(GTK_WIDGET_CLASS(class), "/org/tracksuitdev/cgptchatbot/ui/chatbox.ui");
    G_OBJECT_CLASS(class)->dispose = cgpt_chatbot_chatbox_dispose;
    gtk_widget_class_bind_template_child(GTK_WIDGET_CLASS(class), CgptChatbotChatbox, chat_entry);
    gtk_widget_class_bind_template_child(GTK_WIDGET_CLASS(class), CgptChatbotChatbox, send_button);
    gtk_widget_class_bind_template_child(GTK_WIDGET_CLASS(class), CgptChatbotChatbox, messages);
    gtk_widget_class_bind_template_callback(GTK_WIDGET_CLASS(class), send_button_clicked);
}


void cgpt_chatbot_chatbox_load_chat(CgptChatbotChatbox *self, CGptChat *chat) {
    if (chat == NULL) {
        return;
    }
    self->chat = chat;
    for (int i = 0; i < chat->messages_count; i++) {
        cgpt_chatbot_chatbox_add_message(self, chat->messages[i]);
    }
}

CgptChatbotChatbox *cgpt_chatbot_chatbox_new() {
    return g_object_new(CGPT_CHATBOT_CHAT_BOX_TYPE, NULL);
}
