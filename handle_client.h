#ifndef HANDLE_CLIENT_H
#define HANDLE_CLIENT_H

#include <stdio.h>
#include "log.h"

void *handle_client(void *client_sockfd);
const char *get_content_type(const char *path);

extern LogBuffer log_buffer;

#endif
