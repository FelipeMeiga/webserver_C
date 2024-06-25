#include "handle_client.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include "log.h"
#include "start_server.h"
#include <arpa/inet.h>
#include <time.h>

extern LogBuffer log_buffer;

extern char default_dir[256];

void *handle_client(void *client_sockfd) {
    int sock = *(int *)client_sockfd;
    free(client_sockfd);

    char buffer[1024] = {0};

    struct sockaddr_in addr;
    socklen_t addr_size = sizeof(struct sockaddr_in);
    getpeername(sock, (struct sockaddr *)&addr, &addr_size);
    char *client_ip = inet_ntoa(addr.sin_addr);

    int read_size = read(sock, buffer, sizeof(buffer) - 1);
    if (read_size > 0) {
        buffer[read_size] = '\0';
    } else {
        perror("[SERVER] - Read failed");
        close(sock);
        return NULL;
    }

    time_t now = time(NULL);
    struct tm *t = localtime(&now);
    char time_str[100];
    strftime(time_str, sizeof(time_str) - 1, "%Y-%m-%d %H:%M:%S", t);

    printf("\033[31m");
    printf("\n%s - %s\n", time_str, client_ip);
    printf("%s\n", buffer);
    printf("\033[0m");

    char method[10] = {0}, path[255] = {0}, protocol[10] = {0};
    sscanf(buffer, "%s %s %s", method, path, protocol);

    char default_archive[256] = "/index.html";

    char default_path [256] = {0};
    strcat(default_path, default_dir);
    strcat(default_path, default_archive);
    
    char full_path[512] = {0};
    if (strcmp(path, "/") == 0) {
        sprintf(full_path, "%s/index.html", default_dir);
    } else {
        sprintf(full_path, "%s%s", default_dir, path);
    }

    FILE *f = fopen(full_path, "rb");
    if (f == NULL) {
        char *error_message = "HTTP/1.1 404 Not Found\r\nContent-Type: text/plain\r\n\r\nFile not found.\n";
        int write_ret = write(sock, error_message, strlen(error_message));
        if (write_ret < 0) {
            printf("[CONSOLE] - Error trying to write on socket.\n");
            return NULL;
        }
        add_log_entry(&log_buffer, full_path, client_ip, "404 Not Found");
        close(sock);
        return NULL;
    }

    const char *content_type = get_content_type(full_path);
    char header[256];
    snprintf(header, sizeof(header), "HTTP/1.1 200 OK\r\nContent-Type: %s\r\n\r\n", content_type);
    if (write(sock, header, strlen(header)) < 0) {
        perror("[SERVER] - Failed to send header");
        fclose(f);
        close(sock);
        return NULL;
    }

    printf("\033[34m");
    printf("HTTP/1.1 200 OK\nContent-Type: %s\n\n", content_type);

    if (strncmp(content_type, "image/", 6) == 0) {
        printf("[SERVER] - Image content will be displayed in the browser.\n");
        char file_buffer[1024] = {0};
        int bytes_read;
        while ((bytes_read = fread(file_buffer, 1, sizeof(file_buffer), f)) > 0) {
            if (write(sock, file_buffer, bytes_read) < 0) {
                perror("[SERVER] - Failed to send file data");
                fclose(f);
                close(sock);
                return NULL;
            }
        }
    } else {
        char file_buffer[1024] = {0};
        int bytes_read;
        while ((bytes_read = fread(file_buffer, 1, sizeof(file_buffer), f)) > 0) {
            if (write(sock, file_buffer, bytes_read) < 0) {
                perror("[SERVER] - Failed to send file data");
                fclose(f);
                close(sock);
                return NULL;
            }
            fwrite(file_buffer, 1, bytes_read, stdout);
        }
    }

    printf("\n");
    printf("\033[0m");

    add_log_entry(&log_buffer, full_path, client_ip, "200 OK");
    fclose(f);
    close(sock);

    return NULL;
}
