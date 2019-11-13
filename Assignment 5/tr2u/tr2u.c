#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int main(int argc, char* argv[]) {
    char* progName = argv[0];
    if (argc != 3) {
        fprintf(stderr, "%s: Incorrect number of arguments.", progName);
        exit(1);
    }

    char* from = argv[1];
    char* to = argv[2];

    // make sure the two arguments are the same size, and check for duplicates in from
    int argSize = 0;
    while(from[argSize] != '\0' && to[argSize] != '\0') {
        for (int i = 0; i < argSize; i++) {
            if (from[i] == from[argSize]) {
                fprintf(stderr, "%s: Invalid argument; first argument cannot have duplicate bytes.", progName);
                exit(1);
            }
        }
        argSize++;
    }

    if ((from[argSize] == '\0' && to[argSize] != '\0') || (from[argSize] != '\0' && to[argSize] == '\0')) {
        fprintf(stderr, "%s: Invalid arguments; must be the same length.", progName);
        exit(1);
    }

    char c;
    ssize_t readResult = read(STDIN_FILENO, &c, sizeof(char));
    while (readResult != 0 && readResult != -1) {
        for (int i = 0; i < argSize; i++) {
            if (from[i] == c) {
                c = to[i];
                 break;
            }
        }
        if (write(STDOUT_FILENO, &c, sizeof(char)) == -1) {
            fprintf(stderr, "%s: Error writing to standard output.", progName);
            exit(1);
        }
        readResult = read(STDIN_FILENO, &c, sizeof(char));
    }
    if (readResult == -1) {
        fprintf(stderr, "%s: Error reading from standard input.", progName);
        exit(1);
    }

    return 0;
}