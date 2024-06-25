#include "start_server.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <pthread.h>
#include <arpa/inet.h>
#include <signal.h>
#include <errno.h>
#include "parse_options.h"
#include "handle_client.h"
#include "log.h"
#include <sys/time.h>

int server_running = 1;
int sockfd;

extern FILE* logfile;

int image_count = 0;
int text_count = 0;
int html_count = 0;
char *file_name;
int log_enabled = 0;
int stats_enabled = 0;

pthread_t log_thread;

char default_dir[256];

void start_server(Options *options) {
    if (options == NULL) {
        fprintf(stderr, "Options are NULL\n");
        exit(EXIT_FAILURE);
    }

    if (strlen(options->logFilename) > 0) {
        log_enabled = 1;
        init_log_buffer(&log_buffer, options->logFilename);
    }

    if (strlen(options->statsFilename) > 0) {
        stats_enabled = 1;
        file_name = options->statsFilename;
    }

    strcpy(default_dir, options->rootDir);

    signal(SIGINT, handle_signal);
    signal(SIGUSR1, handle_signal);

    if (log_enabled) {
        if (pthread_create(&log_thread, NULL, log_writer, &log_buffer) != 0) {
            perror("[CONSOLE] - Failed to create log writer thread");
            exit(EXIT_FAILURE);
        }
    }

    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) {
        perror("[CONSOLE] - Error creating socket\n");
        free(options);
        exit(EXIT_FAILURE);
    }

    struct sockaddr_in server_addr;
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(options->port);

    if (bind(sockfd, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        perror("[CONSOLE] - Bind failed\n");
        free(options);
        exit(EXIT_FAILURE);
    }

    if (listen(sockfd, 5) < 0) {
        perror("[CONSOLE] - Listen failed\n");
        free(options);
        exit(EXIT_FAILURE);
    }

    printf("[CONSOLE] - Server started on port %d rooted @%s\n\n", options->port, options->rootDir);
    printf("[CONSOLE] - To close server connection type kill -SIGUSR1 %d\n", getpid());

    while (server_running) {
        fd_set read_fds;
        FD_ZERO(&read_fds);
        FD_SET(sockfd, &read_fds);

        struct timeval timeout;
        timeout.tv_sec = 1;
        timeout.tv_usec = 0;

        int activity = select(sockfd + 1, &read_fds, NULL, NULL, &timeout);
        if (activity < 0 && errno != EINTR) {
            perror("[CONSOLE] - Select error\n");
            break;
        }

        if (activity > 0 && FD_ISSET(sockfd, &read_fds)) {
            struct sockaddr_in client_addr;
            socklen_t client_len = sizeof(client_addr);
            int *new_sockfd = (int*)malloc(sizeof(int));
            if (new_sockfd == NULL) {
                perror("[CONSOLE] - Failed to allocate memory for new socket\n");
                continue;
            }
            *new_sockfd = accept(sockfd, (struct sockaddr *)&client_addr, &client_len);
            if (*new_sockfd < 0) {
                perror("[CONSOLE] - Accept failed\n");
                free(new_sockfd);
                continue;
            }

            printf("[SERVER] - Connection accepted from %s:%d\n", inet_ntoa(client_addr.sin_addr), ntohs(client_addr.sin_port));

            pthread_t thread_id;
            if (pthread_create(&thread_id, NULL, handle_client, new_sockfd) != 0) {
                perror("[CONSOLE] - Failed to create thread\n");
                close(*new_sockfd);
                free(new_sockfd);
                continue;
            }
            pthread_detach(thread_id);
        }
    }

    if (log_enabled) {
        if (pthread_cancel(log_thread) != 0) {
            perror("[CONSOLE] - Failed to cancel log writer thread\n");
        }

        if (pthread_join(log_thread, NULL) != 0) {
            perror("[CONSOLE] - Failed to join log writer thread\n");
        }
    }
}
