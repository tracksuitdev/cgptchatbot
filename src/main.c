#include <stdio.h>
#include <stdlib.h>
#include <gtk/gtk.h>
#include "appdata.h"

static const char APP_TITLE[] = "cgptchatbot";

static void handle_save_api_key(GtkEntry *entry) {
    GtkEntryBuffer *buffer = gtk_entry_get_buffer(entry);
    const char *api_key = gtk_entry_buffer_get_text(buffer);
    if (save_api_key(api_key)) {
        g_print("API key saved successfully\n");
    } else {
        g_print("Error saving API key\n");
    }
}

static void activate(GtkApplication* app, gpointer user_data) {
    GtkWidget *window;
    window = gtk_application_window_new(app);
    gtk_window_set_title(GTK_WINDOW (window), APP_TITLE);
    gtk_window_set_default_size(GTK_WINDOW (window), 1000, 1000);
    if (!data_dir_exists()) {
        create_data_dir();
    }
    APPDATA *appdata = read_data_file();
    if (appdata == NULL) {
        GtkWidget  *box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
        gtk_widget_set_halign(box, GTK_ALIGN_CENTER);
        gtk_widget_set_valign(box, GTK_ALIGN_CENTER);

        gtk_window_set_child(GTK_WINDOW(window) , box);

        GtkWidget *entry = gtk_entry_new();
        gtk_entry_set_placeholder_text(GTK_ENTRY(entry), "Enter your API key here");
        gtk_widget_set_halign(entry, GTK_ALIGN_CENTER);
        gtk_widget_set_valign(entry, GTK_ALIGN_CENTER);
        gtk_widget_set_margin_top(entry, 10);
        gtk_widget_set_margin_bottom(entry, 10);
        gtk_widget_set_margin_start(entry, 10);
        gtk_widget_set_margin_end(entry, 10);
        gtk_widget_set_size_request(entry, 300, 30);
        gtk_box_append(GTK_BOX(box), entry);

        GtkWidget *button = gtk_button_new_with_label("Save");
        gtk_widget_set_halign(button, GTK_ALIGN_CENTER);
        gtk_widget_set_valign(button, GTK_ALIGN_CENTER);
        gtk_widget_set_margin_top(button, 10);
        gtk_widget_set_margin_bottom(button, 10);
        gtk_widget_set_margin_start(button, 10);
        gtk_widget_set_margin_end(button, 10);
        gtk_widget_set_size_request(button, 300, 30);
        gtk_box_append(GTK_BOX(box), button);
        g_signal_connect_swapped(button, "clicked", G_CALLBACK(handle_save_api_key), entry);

        appdata = read_data_file();
    }
    gtk_widget_set_visible(window, TRUE);
}

int main(int argc, char **argv) {
    GtkApplication *app;
    int status;
    app = gtk_application_new("org.tracksuitdev.cgptchatbot", G_APPLICATION_DEFAULT_FLAGS);
    g_signal_connect(app, "activate", G_CALLBACK(activate), NULL);
    status = g_application_run(G_APPLICATION (app), argc, argv);
    g_object_unref(app);
    return status;
}
