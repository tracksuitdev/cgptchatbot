#include <gtk/gtk.h>
#include "cgptchatbotapp.h"

int main(int argc, char **argv) {
    return g_application_run (G_APPLICATION (cgptchatbot_app_new ()), argc, argv);
}
