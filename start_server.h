#ifndef START_SERVER_H
#define START_SERVER_H

#include "parse_options.h"
#include "log.h"
#include <stdio.h>

typedef struct args {
    int *new_sockfd;
    Options *options;
} Args;

void start_server(Options *options);
void handle_signal(int signal);

extern int server_running;
extern int sockfd;

extern int image_count;
extern int text_count;
extern int html_count;
extern char *file_name;
extern int log_enabled;
extern int stats_enabled;

extern pthread_t log_thread;

extern char default_dir[256];

#endif
