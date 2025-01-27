

#ifndef COM_H
#define COM_H

#include <gtk/gtk.h>


void print_hello_to_terminal(GtkWidget *widget, gpointer data);
void print_to_buffer(GtkTextBuffer *buffer, const char *message);

#endif
