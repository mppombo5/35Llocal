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
void* doubleMemSize(void* ptr, size_t* memSize);

int main(int argc, char* argv[]) {
    char const* progName = argv[0];

    // could use big getopt function to check for -f, or we could just do it the easy way
    bool ignoreCase = false;
    if (argc == 2) {
        if (strcmp(argv[1], "-f") == 0)
            ignoreCase = true;
        else {
            fprintf(stderr, "%s: Invalid argument \"%s\"", progName, argv[1]);
            exit(1);
        }
    }
    else if (argc > 2) {
        fprintf(stderr, "%s: Too many arguments", progName);
        exit(1);
    }

    struct stat fileStat;
    if (fstat(STDIN_FILENO, &fileStat) == -1) {
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
    //sleep(10);
    int readBytes = read(STDIN_FILENO, fileBuffer, fSize);
    if (readBytes == -1) {
        fprintf(stderr, "%s: Error reading from standard input\n%s", progName, strerror(errno));
        exit(1);
    }

    /* fileBuffer now has most or all of the contents of stdin.
     * allocate fSize pointers to characters; in the worst case, there will be fSize spaces, and it's
     * fairly cheap to allocate pointers.
     */
    bool didReadBytes = readBytes > 0;
    size_t list1Size = 0;
    size_t list1MemSize = didReadBytes ? ((fSize / sizeof(char)) * sizeof(char*)) : (sizeof(char*) * 8);
    char** wordList1 = (char**) malloc(list1MemSize);
    if (didReadBytes) {
        wordList1[0] = &fileBuffer[0];
        list1Size++;
    }

    //sleep(10);
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
        wordList1[list1Size] = &fileBuffer[i];
        list1Size++;
    }

    if (didReadBytes && fileBuffer[readBytes-1] != ' ') {
        fileBuffer[readBytes] = ' ';
    }

    lseek(STDIN_FILENO, 0, SEEK_CUR);

    //sleep(10);

    // start with 8 words, grow as needed
    size_t list2Size = 0;
    size_t list2MemSize = sizeof(char*) * 8;
    char** wordList2 = (char**) malloc(list2MemSize);
    if (wordList2 == NULL) {
        fprintf(stderr, "%s: Error allocating memory\n%s", progName, strerror(errno));
        exit(1);
    }

    char c;
    int charRead = read(0, &c, sizeof(char));
    if (charRead == -1) {
        fprintf(stderr, "%s: Error reading from stdin\n%s", progName, strerror(errno));
        exit(1);
    }
    while (charRead != 0) {
        // reallocate more memory as we go
        if (((list2Size + 1) * sizeof(char*)) >= list2MemSize) {
            char** temp = (char**) doubleMemSize(wordList2, &list2MemSize);
            if (temp == NULL) {
                fprintf(stderr, "%s: Error allocating memory\n%s", progName, strerror(errno));
                exit(1);
            }
            wordList2 = temp;
        }

        // start with memory size at 8, double the size and reallocate if it exceeds that
        size_t wordSize = 0;
        size_t wordMemSize = sizeof(char) * 8;

        char* word = (char*) malloc(wordMemSize);
        if (word == NULL) {
            free(wordList2);
            fprintf(stderr, "%s: Error allocating memory\n%s", progName, strerror(errno));
            exit(1);
        }

        while (c != ' ' && charRead != 0) {
            // allow for an extra 2 bytes so that we can add a space and newline at the end
            if (((wordSize + 2) * sizeof(char)) >= wordMemSize) {
                char* temp = (char*) doubleMemSize(word, &wordMemSize);
                if (temp == NULL){
                    free(wordList2);
                    fprintf(stderr, "%s: Error allocating memory\n%s", progName, strerror(errno));
                    exit(1);
                }
                word = temp;
            }
            word[wordSize] = c;
            wordSize++;
            charRead = read(0, &c, sizeof(char));
            if (charRead == -1) {
                fprintf(stderr, "%s: Error reading from stdin\n%s", progName, strerror(errno));
                exit(1);
            }
        }
        word[wordSize] = ' ';
        word[wordSize+1] = '\0';

        wordList2[list2Size] = word;
        list2Size++;
        if (charRead != 0)
            charRead = read(0, &c, sizeof(char));
    }

    /* might seem a little hacky, but due to weird circumstances with pointers pointing directly into
     * the file buffer and having different pointers for reading past the file, it's best to have one big array
     * that merges the two together, so we can free the ones that came after the array.
     */
    size_t compositeSize = list1Size + list2Size;
    char** wordList = (char**) malloc(compositeSize * sizeof(char*));
    int listIndex = 0;
    for (size_t j = 0; j < list1Size; j++) {
        wordList[listIndex] = wordList1[j];
        listIndex++;
    }
    for (size_t j = 0; j < list2Size; j++) {
        wordList[listIndex] = wordList2[j];
        listIndex++;
    }

    // sort the composite list of string pointers and output them
    qsort(wordList, compositeSize, sizeof(char*), frobcmp(ignoreCase));

    /* cycle through each word in wordList1, and for each
     * word putchar each character until you hit a space
     */
    for (size_t j = 0; j < compositeSize; j++) {
        for (int k = 0; wordList[j][k] != ' '; k++) {
            if (write(STDOUT_FILENO, &wordList[j][k], sizeof(char)) == -1) {
                fprintf(stderr, "%s: Error printing to standard output\n", progName);
                exit(1);
            }
        }
        if (putchar(' ') == EOF) {
            fprintf(stderr, "%s: Error printing to standard output\n", progName);
            exit(1);
        }
    }

    free(fileBuffer);
    free(wordList1);
    for (size_t j = 0; j < list2Size; j++) {
        free(wordList2[j]);
    }
    free(wordList2);
    free(wordList);

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

void* doubleMemSize(void* ptr, size_t *memSize) {
    *memSize *= 2;
    // double it and add two, make room for space and '\0'
    return realloc(ptr, *memSize);
}