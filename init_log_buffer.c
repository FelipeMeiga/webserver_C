#include <string.h>
#include <unistd.h>
#include "log.h"
#include <pthread.h>
#include <stdio.h>

void init_log_buffer(LogBuffer *buffer, const char *filename) {
    buffer->start = 0;
    buffer->end = 0;
    buffer->count = 0;
    strncpy(buffer->log_filename, filename, sizeof(buffer->log_filename) - 1);
    buffer->log_filename[sizeof(buffer->log_filename) - 1] = '\0';
    pthread_mutex_init(&buffer->mutex, NULL);
    pthread_cond_init(&buffer->cond, NULL);
}