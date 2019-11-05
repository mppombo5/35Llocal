#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>
#include <sys/stat.h>
#include <errno.h>
#include <string.h>
#include <unistd.h>

int   frobcmp(const void *a, const void *b);
void* doubleMemSize(void* ptr, int* memSize);

int main(int argc, char* argv[]) {
    // file descriptor for stdin is 0
    int stdinFD = 0;
    struct stat fileStat;
    if (fstat(stdinFD, &fileStat) == -1) {
        fprintf(stderr, "%s: Error reading stdin\n%s", argv[0], strerror(errno));
        exit(1);
    }
    // get the size of stdin in bytes
    size_t fSize = fileStat.st_size;
    char* fileBuf = (char*) malloc(fSize);
    if (fileBuf == NULL) {
        fprintf(stderr, "%s: Error allocating memory\n%s", argv[0], strerror(errno));
        exit(1);
    }
    int readResult = read(stdinFD, fileBuf, fSize);
    if (readResult == -1) {
        fprintf(stderr, "%s: Error allocating memory\n%s", argv[0], strerror(errno));
        exit(1);
    }
    // fileBuf now has most or all of the contents of stdin.
    // allocate fSize pointers to characters; in the worst case, there will be fSize spaces, and it's
    // fairly cheap to allocate pointers.
    char** wordList = (char**) malloc(fSize / sizeof(char));


    return 0;
}

int frobcmp(const void *aVoid, const void *bVoid) {
    char const* a = *(char const**) aVoid;
    char const* b = *(char const**) bVoid;
    int i = 0;
    while (a[i] != ' ' && b[i] != ' ') {
        // unfrobnicate by XOR-ing a[i] and b[i] with 0x2A (42 in decimal)
        char aFrob = (char) (a[i] ^ 0x2A);
        char bFrob = (char) (b[i] ^ 0x2A);

        if (aFrob > bFrob)
            return 1;
        if (aFrob < bFrob)
            return -1;
        i++;
    }
    // checks for if one is longer than the other.
    // they'll either be the same, or one will be a prefix
    if (a[i] == ' ') {
        if (b[i] == ' ')
            return 0;
        else
            return 1;
    }
    if (b[i] == ' ')
        return -1;
}

void* doubleMemSize(void* ptr, int* memSize) {
    *memSize *= 2;
    // double it and add two, make room for space and '\0'
    return realloc(ptr, *memSize);
}