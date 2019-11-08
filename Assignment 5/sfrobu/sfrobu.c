#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>
#include <sys/stat.h>
#include <errno.h>
#include <string.h>
#include <unistd.h>
#include <stdbool.h>

int frobcmpRegular(const void* a, const void* b);
int frobcmpIgnore(const void* aVoid, const void* bVoid);
int (*frobcmp(bool ignoreCase))(const void* a, const void* b);
void* doubleMemSize(void* ptr, int* memSize);

int main(int argc, char* argv[]) {
    char const* progName = argv[0];

    // could use big getopt function to check for -f, or we could just do it the easy way
    bool ignoreCase = false;
    if (argc == 2) {
        if (strcmp(argv[1], "-f") == 0)
            ignoreCase = true;
        else {
            fprintf(stderr, "%s: Invalid argument %s", progName, argv[1]);
            exit(1);
        }
    }
    else if (argc > 2) {
        fprintf(stderr, "%s: Too many arguments", progName);
        exit(1);
    }

    struct stat fileStat;
    // file descriptor for stdin is 0
    if (fstat(0, &fileStat) == -1) {
        fprintf(stderr, "%s: Error reading stdin\n%s", progName, strerror(errno));
        exit(1);
    }

    size_t fSize = fileStat.st_size;
    // one more than fSize so we can add a space if needed
    char* fileBuffer = (char*) malloc(fSize+1);
    if (fileBuffer == NULL) {
        fprintf(stderr, "%s: Error allocating memory\n%s", progName, strerror(errno));
        exit(1);
    }
    int readBytes = read(0, fileBuffer, fSize);
    if (readBytes == -1) {
        fprintf(stderr, "%s: Error reading from standard input\n%s", progName, strerror(errno));
        exit(1);
    }

    // debug statement so I can attach to process from my IDE
    //sleep(20);

    /* fileBuffer now has most or all of the contents of stdin.
     * allocate fSize pointers to characters; in the worst case, there will be fSize spaces, and it's
     * fairly cheap to allocate pointers.
     */
    char** wordList = (char**) malloc((readBytes > 0) ? fSize / sizeof(char) : 1);
    int listSize = 0;
    if (readBytes > 0) {
        wordList[0] = &fileBuffer[0];
        listSize++;
    }

    // fileBuffer only has readBytes bytes in it, don't read past the buffer
    size_t i = 0;
    while (i < readBytes) {
        char c = fileBuffer[i];
        // have to also check that i isn't reading past readBytes
        while (c != ' ' && i < readBytes) {
            i++;
            c = fileBuffer[i];
        }
        // don't want to add another string if we're at the end of readBytes
        if (i >= readBytes)
            break;

        // can assume the last char was a space
        i++;
        wordList[listSize] = &fileBuffer[i];
        listSize++;
    }
    // stick a space on the end if fileBuffer doesn't already end with one
    if (fileBuffer[readBytes-1] != ' ') {
        fileBuffer[readBytes] = ' ';
    }

    // keep allocating and do more if the file has grown since the first array was allocated

    // once the parsing through stdin is done, sort the pointers to the strings and output them
    qsort(wordList, listSize, sizeof(char*), frobcmp(ignoreCase));

    /* cycle through each word in wordList, and for each
     * word putchar each character until you hit a space
     */
    for (int j = 0; j < listSize; j++) {
        for (int k = 0; wordList[j][k] != ' '; k++) {
            if (putchar(wordList[j][k]) == EOF) {
                fprintf(stderr, "%s: Error printing to standard output\n", progName);
                exit(1);
            }
        }
        if (putchar(' ') == EOF) {
            fprintf(stderr, "%s: Error printing to standard output\n", progName);
            exit(1);
        }
    }

    /*for (int j = 0; j < listSize; j++)
        free(wordList[j]);
    free(wordList);*/

    return 0;
}

int frobcmpRegular(const void *aVoid, const void *bVoid) {
    char const* a = *(char const**) aVoid;
    char const* b = *(char const**) bVoid;
    int i = 0;
    while (a[i] != ' ' && b[i] != ' ') {
        // unfrobnicate by XOR-ing a[i] and b[i] with 0x2A (42 in decimal)
        char aFrob = (char) (a[i] ^ 42);
        char bFrob = (char) (b[i] ^ 42);

        if (aFrob > bFrob)
            return 1;
        if (aFrob < bFrob)
            return -1;
        i++;
    }
    // checks for if one is longer than the other.
    // they'll either be the same, or one will be a prefix
    if (a[i] == ' ') {
        return (b[i] == ' ') ? 0 : 1;
    }
    if (b[i] == ' ')
        return -1;
}

// exactly the same as frobcmpRegular, but it makes the characters uppercase before comparing
int frobcmpIgnore(const void* aVoid, const void* bVoid) {
    char const* a = *(char const**) aVoid;
    char const* b = *(char const**) bVoid;
    int i = 0;
    while (a[i] != ' ' && b[i] != ' ') {
        char aFrob = (char) (a[i] ^ 42);
        char bFrob = (char) (b[i] ^ 42);

        // make them unsigned and uppercase them
        // has to be an unsigned char, otherwise behavior is undefined
        aFrob = (char) toupper((unsigned char) aFrob);
        bFrob = (char) toupper((unsigned char) bFrob);

        if (aFrob > bFrob)
            return 1;
        if (aFrob < bFrob)
            return -1;
        i++;
    }
    if (a[i] == ' ') {
        return (b[i] == ' ') ? 0 : 1;
    }
    if (b[i] == ' ')
        return -1;
}

// pass this into qsort, and this function will return the corresponding comparing function
int (*frobcmp(bool ignoreCase))(const void*, const void*) {
    return ignoreCase ? &frobcmpIgnore : &frobcmpRegular;
}

void* doubleMemSize(void* ptr, int* memSize) {
    *memSize *= 2;
    // double it and add two, make room for space and '\0'
    return realloc(ptr, *memSize);
}