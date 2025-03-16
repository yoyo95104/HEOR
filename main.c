#include <unistd.h>
#include <stdio.h>
#include <gtk/gtk.h>
#include "utl.h"
#include "threads.h"

void on_attack_button_clicked(GtkWidget *widget, gpointer data) { 
AttackData *attack_data = (AttackData *)data;
    const char *website = gtk_entry_get_text(GTK_ENTRY(attack_data->website_entry));
    strncpy(attack_data->website, website, sizeof(attack_data->website) - 1);
    start_thread_pool(attack_data);
}

int main(int argc, char *argv[]) {
    gtk_init(&argc, &argv);

    GtkWidget *window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_window_set_title(GTK_WINDOW(window), "HEOR");
    gtk_window_set_default_size(GTK_WINDOW(window), 400, 300);
    g_signal_connect(window, "destroy", G_CALLBACK(gtk_main_quit), NULL);

    GtkWidget *vbox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 5);
    gtk_container_add(GTK_CONTAINER(window), vbox);

    GtkWidget *entry = gtk_entry_new();
    gtk_entry_set_placeholder_text(GTK_ENTRY(entry), "Enter website (e.g., example.com)");
    gtk_box_pack_start(GTK_BOX(vbox), entry, FALSE, FALSE, 5);

    GtkWidget *button = gtk_button_new_with_label("Start Attack");
    gtk_box_pack_start(GTK_BOX(vbox), button, FALSE, FALSE, 5);

    GtkWidget *output_label = gtk_label_new("Status: Waiting...");
    gtk_box_pack_start(GTK_BOX(vbox), output_label, FALSE, FALSE, 5);

    GtkWidget *packet_label = gtk_label_new("Packets Sent: 0 | Packets Received: 0");
    gtk_box_pack_start(GTK_BOX(vbox), packet_label, FALSE, FALSE, 5);

    AttackData attack_data = {0};
    attack_data.output_label = output_label;
    attack_data.packet_label = packet_label;
    attack_data.website_entry = entry;

    g_signal_connect(button, "clicked", G_CALLBACK(on_attack_button_clicked), &attack_data);
    gtk_widget_show_all(window);
    gtk_main();
    return 0;
}
