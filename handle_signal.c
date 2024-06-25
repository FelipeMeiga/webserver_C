#include <stdio.h>
#include <signal.h>
#include "start_server.h"
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>

extern FILE* logfile;

void handle_signal(int signal) {
    FILE *stats_file;
    if (signal == SIGINT || signal == SIGTERM || signal == SIGUSR1) {
        server_running = 0;
        if (logfile) {
            fclose(logfile);
        }
        close(sockfd);
        printf("[SERVER] - Shutting down server...\n");

        if (stats_enabled && file_name) {
            stats_file = fopen(file_name, "w");
            if (stats_file) {
                fprintf(stats_file, "IMAGE REQUEST NUMBER - %d\n", image_count);
                fprintf(stats_file, "TEXT REQUEST NUMBER - %d\n", text_count);
                fprintf(stats_file, "HTML REQUEST NUMBER - %d\n", html_count);
                fclose(stats_file);
            } else {
                perror("[SERVER] - Failed to open stats file");
            }
        }
    }
}