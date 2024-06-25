#include <string.h>
#include <unistd.h>
#include "log.h"
#include <pthread.h>
#include <stdio.h>
#include <time.h>
#include "start_server.h"

void add_log_entry(LogBuffer *buffer, const char *entry, const char *client_ip, const char *status) {
    if (!log_enabled) {
        return;
    }

    pthread_mutex_lock(&buffer->mutex);
    if (buffer->count < MAX_LOG_ENTRIES) {
        time_t now = time(NULL);
        struct tm *t = localtime(&now);
        char time_str[100];
        strftime(time_str, sizeof(time_str) - 1, "%Y-%m-%d %H:%M:%S", t);
        snprintf(buffer->entries[buffer->end], sizeof(buffer->entries[buffer->end]), "%s (%s) %s: %s\n", time_str, client_ip, status, entry);
        buffer->end = (buffer->end + 1) % MAX_LOG_ENTRIES;
        buffer->count++;
        pthread_cond_signal(&buffer->cond);
    } else {
        printf("[LOG] - Log buffer full, entry discarded.\n");
    }
    pthread_mutex_unlock(&buffer->mutex);
}
