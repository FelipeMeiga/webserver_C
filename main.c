#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <pthread.h>
#include <signal.h>
#include <assert.h>
#include <sys/socket.h>
#include "parse_options.h"
#include "start_server.h"
#include <fcntl.h>

//valgrind --leak-check=full --show-leak-kinds=all --track-origins=yes --verbose 

int main(int argc, char *argv[]) {
    Options *options = parse(argc, argv);
    if (options->background) {
        pid_t pid, sid;

        pid = fork();
        if (pid < 0) {
            exit(EXIT_FAILURE);
        }

        if (pid > 0) {
            printf("[CONSOLE] - Server started on port %d rooted @%s\n\n", options->port, options->rootDir);
            printf("[CONSOLE] - To close server connection type kill -SIGUSR1 %d\n", pid);
            exit(EXIT_SUCCESS);
        }

        sid = setsid();
        if (sid < 0) {
            exit(EXIT_FAILURE);
        }

        if ((chdir("/")) < 0) {
            exit(EXIT_FAILURE);
        }

        close(STDIN_FILENO);
        close(STDOUT_FILENO);
        close(STDERR_FILENO);

        open("/dev/null", O_RDWR); // stdin
        if (!dup(0)) {
            return 0;
        } // stdout

        if (!dup(0)) {
            return 0;
        } // stderr
    }

    start_server(options);
    free(options);
    return 0;
}
