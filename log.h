#ifndef LOG_H
#define LOG_H

#include <pthread.h>
#include <stdio.h> 

#define MAX_LOG_ENTRIES 100

typedef struct {
    char entries[MAX_LOG_ENTRIES][1024];
    int start;
    int end;
    int count;
    char log_filename[256];
    pthread_mutex_t mutex;
    pthread_cond_t cond;
} LogBuffer;

void init_log_buffer(LogBuffer *buffer, const char *filename);
void add_log_entry(LogBuffer *buffer, const char *entry, const char *client_ip, const char *status);
void *log_writer(void *arg);

extern FILE *logfile;
extern LogBuffer log_buffer;

#endif
