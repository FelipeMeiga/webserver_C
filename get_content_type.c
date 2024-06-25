#include <string.h>
#include "handle_client.h"
#include "start_server.h"

const char *get_content_type(const char *path) {
    const char *ext = strrchr(path, '.');
    if (ext != NULL) {
        if (strcmp(ext, ".html") == 0 || strcmp(ext, ".htm") == 0) {
            if (stats_enabled) html_count++;
            return "text/html";
        } else if (strcmp(ext, ".jpg") == 0 || strcmp(ext, ".jpeg") == 0) {
            if (stats_enabled) image_count++;
            return "image/jpeg";
        } else if (strcmp(ext, ".png") == 0) {
            if (stats_enabled) image_count++;
            return "image/png";
        } else if (strcmp(ext, ".gif") == 0) {
            if (stats_enabled) image_count++;
            return "image/gif";
        } else if (strcmp(ext, ".txt") == 0) {
            if (stats_enabled) text_count++;
            return "text/plain";
        }
    }
    return "application/octet-stream";
}
