#ifndef UTL_H
#define UTL_H

#include <gtk/gtk.h>
#include <pthread.h>
#include <stdbool.h>

typedef struct {
    GtkWidget *packet_info_label;
    GtkWidget *output_label;
    GtkWidget *packet_label;
    GtkWidget *website_entry;
    GtkWidget *udp_toggle;
    int packets_sent;
    int packets_received;
    char website[256];
    int thread_count;
    pthread_t *threads;
    bool use_udp;
} AttackData;
void *send_attack(void *arg);

#endif 

