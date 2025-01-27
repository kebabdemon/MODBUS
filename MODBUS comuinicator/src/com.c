#include <gtk/gtk.h>
#include "../lib/com.h"


void print_hello_to_terminal(GtkWidget *widget, gpointer data) {
    g_print("Hello World (to terminal)\n");
}

void print_to_buffer(GtkTextBuffer *buffer, const char *message) {
    GtkTextIter iter;
    gtk_text_buffer_get_end_iter(buffer, &iter);
    gtk_text_buffer_insert(buffer, &iter, message, -1);
    g_print("%s", message);
}
