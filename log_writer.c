#include "log.h"
#include "start_server.h"
#include <string.h>
#include <stdio.h>
#include <signal.h>
#include <stdlib.h>

FILE *logfile;

LogBuffer log_buffer;

void *log_writer(void *arg) {
    if (!log_enabled) {
        return NULL;
    }

    LogBuffer *buffer = (LogBuffer *)arg;
    logfile = fopen(buffer->log_filename, "a");
    if (logfile == NULL) {
        perror("Failed to open log file");
        return NULL;
    }

    while (1) {
        pthread_mutex_lock(&buffer->mutex);
        while (buffer->count == 0) {
            pthread_cond_wait(&buffer->cond, &buffer->mutex);
        }
        fputs(buffer->entries[buffer->start], logfile);
        fflush(logfile);
        printf("[LOG] - Wrote log entry: %s\n", buffer->entries[buffer->start]);
        buffer->start = (buffer->start + 1) % MAX_LOG_ENTRIES;
        buffer->count--;
        pthread_mutex_unlock(&buffer->mutex);
    }

    return NULL;
}
