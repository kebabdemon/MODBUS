#ifndef PORT_H
#define PORT_H

#include <gtk/gtk.h>

// Declare the global variable to hold the baud rate
extern const char *BAUS;

// Declare the functions
void port(GtkWidget *widget, gpointer data);
void update_baud_rate(GtkWidget *entry, gpointer data);
char* find_psu_com();

#endif // PORT_H
