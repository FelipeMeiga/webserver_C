#ifndef PARSE_OPTIONS_H
#define PARSE_OPTIONS_H

typedef struct options {
    int port;
    char logFilename[256];
    char statsFilename[256];
    char rootDir[256];
    int background;
} Options;

Options *parse(int argc, char *argv[]);

#endif