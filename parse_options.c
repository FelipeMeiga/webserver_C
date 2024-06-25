#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <getopt.h>
#include "parse_options.h"

Options *parse(int argc, char *argv[]) {
    int opt;

    Options *options = (Options *)malloc(sizeof(Options));
    if (options == NULL) {
        perror("[CONSOLE] - Error trying to malloc options.\n");
        exit(EXIT_FAILURE);
    }

    options->port = 0;
    options->logFilename[0] = '\0';
    options->statsFilename[0] = '\0';
    options->rootDir[0] = '\0';
    options->background = 0;

    if (argc < 2) {
        printf("[CONSOLE] - Please start the server using the following usage below.\n");
        printf("Usage: %s -p <port> -r <rootdir> [-l <logfile>] [-s <statsfile>] [-b]\n", argv[0]);
        free(options);
        exit(EXIT_FAILURE);
    }

    struct option long_options[] = {
        {"port", required_argument, 0, 'p'},
        {"log", required_argument, 0, 'l'},
        {"statistics", required_argument, 0, 's'},
        {"background", no_argument, 0, 'b'},
        {"root", required_argument, 0, 'r'},
        {0, 0, 0, 0}
    };

    int option_index = 0;
    while ((opt = getopt_long(argc, argv, "p:l:s:r:b", long_options, &option_index)) != -1) {
        switch (opt) {
            case 'p':
                options->port = atoi(optarg);
                break;
            case 'l':
                printf("[CONSOLE] - Log file name is %s\n", optarg);
                strncpy(options->logFilename, optarg, sizeof(options->logFilename) - 1);
                options->logFilename[sizeof(options->logFilename) - 1] = '\0';
                break;
            case 's':
                printf("[CONSOLE] - Stats file name is %s\n", optarg);
                strncpy(options->statsFilename, optarg, sizeof(options->statsFilename) - 1);
                options->statsFilename[sizeof(options->statsFilename) - 1] = '\0';
                break;
            case 'r':
                printf("[CONSOLE] - Root directory is %s\n", optarg);
                strncpy(options->rootDir, optarg, sizeof(options->rootDir) - 1);
                options->rootDir[sizeof(options->rootDir) - 1] = '\0';
                break;
            case 'b':
                printf("[CONSOLE] - Running with background flag == 1\n");
                options->background = 1;
                break;
            default:
                printf("[CONSOLE] - Unknown command.\n");
                free(options);
                exit(EXIT_FAILURE);
        }
    }

    if (options->port == 0) {
        fprintf(stderr, "[CONSOLE] - Missing required option: -p <port>\n");
        free(options);
        exit(EXIT_FAILURE);
    }
    if (options->rootDir[0] == '\0') {
        fprintf(stderr, "[CONSOLE] - Missing required option: -r <rootdir>\n");
        free(options);
        exit(EXIT_FAILURE);
    }

    return options;
}
