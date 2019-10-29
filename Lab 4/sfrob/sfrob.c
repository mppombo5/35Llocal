#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

int frobcmp(char const* a, char const* b);
void* doubleMemSize(void* ptr, int* size);

int main() {
    char** wordList = (char**) malloc(0);
    char*  wordOld = (char*) malloc(0);
    char c = (char) getchar();
    while (c != EOF) {
        // start with memory size at 8, double the size and reallocate if it exceeds that
        int wordSize = 0;
        int memSize = 8;

        // allocate enough space for characters, space byte and zero byte
        char* word = (char*) malloc(sizeof(char) * memSize + 2);

        while (c != '\n') {
            // realloc more if memSize chars are taken up, because the last byte has to be \0
            if (wordSize >= memSize) {
                word = (char*) doubleMemSize(word, &memSize);
                if (word == NULL) {
                    fprintf(stderr, "Error allocating memory; program terminated.");
                    return 1;
                }
            }
            word[wordSize] = c;
            wordSize++;
            c = (char) getchar();
        }
        word[wordSize] = ' ';
        // this is safe, because I allocated two more bytes than the word's size
        // or I did something horribly wrong. that is also a possibility.
        word[wordSize+1] = '\0';
        c = (char) getchar();
    }

    assert(frobcmp("*{_CIA\030\031 ", "*`_GZY\v ") > 0);
    assert(frobcmp("* ", "* ") == 0);
    printf("All tests passed!");
    return 0;
}

int frobcmp(char const* a, char const* b) {
    int i = 0;
    while (a[i] != ' ' && b[i] != ' ') {
        // unfrobnicate by XOR-ing a[i] and b[i] with 0x2A (42 in decimal)
        const char aFrob = a[i] ^ 0x2A;
        const char bFrob = b[i] ^ 0x2A;

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
            return -1;
    }
    if (b[i] == ' ')
        return 1;
}

void* doubleMemSize(void* ptr, int* memSize) {
    *memSize *= 2;
    // double it and add two, make room for space and '\0'
    return realloc(ptr, *memSize + 2);
}