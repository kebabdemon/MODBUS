#include <gtk/gtk.h>
#include "lib/com.h"
#include "lib/port.h"
#include "lib/listen_write.h"
#include <glib.h>

const char *BAUS = NULL;
const char *psuPort = NULL;
GtkWidget *command_entry;

void print_to_console(GtkTextBuffer *buffer, const char *message) {
    GtkTextIter iter;
    gtk_text_buffer_get_end_iter(buffer, &iter);
    gtk_text_buffer_insert(buffer, &iter, message, -1);
    g_print("%s", message);
}

void port(GtkWidget *widget, gpointer data) {
    GtkTextBuffer *buffer = GTK_TEXT_BUFFER(data);
    const char *baudRate = BAUS;
    if (baudRate == NULL) {
        baudRate = "not set";
    }

    psuPort = find_psu_com(buffer);

    char message[512];
    if (psuPort) {
        snprintf(message, sizeof(message), "PSU found on: %s with baud rate: %s\n", psuPort, baudRate);
    } else {
        snprintf(message, sizeof(message), "PSU not found. Baud rate: %s\n", baudRate);
    }

    print_to_console(buffer, message);
}

void update_baud_rate(GtkWidget *entry, gpointer data) {
    GtkEntryBuffer *buffer = gtk_entry_get_buffer(GTK_ENTRY(entry));
    BAUS = gtk_entry_buffer_get_text(buffer);
}

int hexstr_to_bytes(const char *hexstr, unsigned char *bytes, size_t max_bytes) {
    size_t hexstr_len = strlen(hexstr);
    size_t bytes_len = 0;
    char hex_byte[3] = {0};

    for (size_t i = 0; i < hexstr_len && bytes_len < max_bytes; ++i) {
        if (hexstr[i] == ' ') {
            continue; // Skip spaces
        }

        hex_byte[0] = hexstr[i];
        hex_byte[1] = hexstr[++i];

        if (sscanf(hex_byte, "%2hhx", &bytes[bytes_len]) != 1) {
            return -1; // Error in conversion
        }
        bytes_len++;
    }

    return bytes_len;
}

void on_listen_write_button_clicked(GtkWidget *widget, gpointer data) {
    GtkTextBuffer *buffer = GTK_TEXT_BUFFER(data);
    const char *command_str = gtk_entry_buffer_get_text(gtk_entry_get_buffer(GTK_ENTRY(command_entry)));

    if (psuPort && BAUS && *BAUS && command_str && *command_str) {
        unsigned char cmd_bytes[256];
        int cmd_length = hexstr_to_bytes(command_str, cmd_bytes, sizeof(cmd_bytes));

        if (cmd_length > 0) {
            print_to_console(buffer, "Sending command...\n");
            listen_write(psuPort, (const char *)cmd_bytes, cmd_length, atoi(BAUS), buffer);
        } else {
            print_to_console(buffer, "Error: Command conversion failed.\n");
        }
    } else {
        print_to_console(buffer, "Error: Port, baud rate, or command is not set properly.\n");
    }
}

void load_css() {
    GtkCssProvider *provider = gtk_css_provider_new();
    char css_path[512];
    snprintf(css_path, sizeof(css_path), "%s/../styles/style.css", g_get_current_dir());
    g_print("Loading CSS from: %s\n", css_path);

    // Load the CSS file
    gtk_css_provider_load_from_path(provider, css_path);

    // Add the provider to the default display
    gtk_style_context_add_provider_for_display(
        gdk_display_get_default(),
        GTK_STYLE_PROVIDER(provider),
        GTK_STYLE_PROVIDER_PRIORITY_APPLICATION
    );

    g_object_unref(provider);
}



static void activate(GtkApplication *app, gpointer user_data) {
    GtkWidget *window = gtk_application_window_new(app);
    gtk_window_set_title(GTK_WINDOW(window), "Communicator");
    gtk_window_set_default_size(GTK_WINDOW(window), 800, 600);

    load_css();

    GtkWidget *grid = gtk_grid_new();
    gtk_window_set_child(GTK_WINDOW(window), grid);

    GtkWidget *console = gtk_text_view_new();
    GtkTextBuffer *buffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(console));
    gtk_text_view_set_editable(GTK_TEXT_VIEW(console), FALSE);
    gtk_text_view_set_wrap_mode(GTK_TEXT_VIEW(console), GTK_WRAP_WORD);

    GtkWidget *scrolled_window = gtk_scrolled_window_new();
    gtk_scrolled_window_set_child(GTK_SCROLLED_WINDOW(scrolled_window), console);

    gtk_widget_set_size_request(console, 400, 1);
    gtk_widget_set_vexpand(console, TRUE);
    gtk_widget_set_hexpand(console, TRUE);

    gtk_grid_attach(GTK_GRID(grid), scrolled_window, 0, 0, 2, 1);

    GtkWidget *button = gtk_button_new_with_label("Scan PSU Port");
    g_signal_connect(button, "clicked", G_CALLBACK(port), buffer);
    gtk_grid_attach(GTK_GRID(grid), button, 0, 1, 1, 1);

    GtkWidget *entry = gtk_entry_new();
    gtk_entry_set_placeholder_text(GTK_ENTRY(entry), "Enter baud rate");
    gtk_entry_set_alignment(GTK_ENTRY(entry), 0.5);
    g_signal_connect(entry, "changed", G_CALLBACK(update_baud_rate), entry);
    gtk_grid_attach(GTK_GRID(grid), entry, 1, 1, 1, 1);

    command_entry = gtk_entry_new();
    gtk_entry_set_placeholder_text(GTK_ENTRY(command_entry), "Enter command");
    gtk_entry_set_alignment(GTK_ENTRY(command_entry), 0.5);
    gtk_grid_attach(GTK_GRID(grid), command_entry, 0, 2, 2, 1);

    button = gtk_button_new_with_label("Send Command");
    g_signal_connect(button, "clicked", G_CALLBACK(on_listen_write_button_clicked), buffer);
    gtk_grid_attach(GTK_GRID(grid), button, 0, 3, 2, 1);

    button = gtk_button_new_with_label("Quit");
    g_signal_connect_swapped(button, "clicked", G_CALLBACK(gtk_window_destroy), window);
    gtk_grid_attach(GTK_GRID(grid), button, 0, 4, 2, 1);

    gtk_window_present(GTK_WINDOW(window));
}

int main(int argc, char *argv[]) {
    GtkApplication *app = gtk_application_new("com.example.GtkApp", G_APPLICATION_DEFAULT_FLAGS);
    g_signal_connect(app, "activate", G_CALLBACK(activate), NULL);

    const int status = g_application_run(G_APPLICATION(app), argc, argv);
    g_object_unref(app);

    return status;
}
