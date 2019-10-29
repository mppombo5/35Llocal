#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

int   frobcmp(const void* aVoid, const void* bVoid);
void* doubleMemSize(void* ptr, int* size);
int   allocFailure();

int main() {
    // start with list memory size at 8, double if needed
    int listSize = 0;
    int listMemSize = sizeof(char*) * 8;
    char** wordList = (char**) malloc(listMemSize);
    // handle failure of malloc call
    if (wordList == NULL) return allocFailure();

    char c = (char) getchar();
    while (c != EOF) {
        // realloc more space if listMemSize is used up
        if ((listSize * sizeof(char*)) >= listMemSize) {
            wordList = (char**) doubleMemSize(wordList, &listMemSize);
            if (wordList == NULL){
                free(wordList);
                return allocFailure();
            }
        }

        // start with memory size at 8, double the size and reallocate if it exceeds that
        int wordSize = 0;
        int wordMemSize = sizeof(char) * 8;

        // allocate enough space for characters, space byte and zero byte
        char* word = (char*) malloc(wordMemSize + 2);
        if (word == NULL) {
            free(wordList);
            return allocFailure();
        }

        while (c != '\n' && c != EOF) {
            // realloc more if wordMemSize chars are taken up, because the last byte has to be \0
            if ((wordSize * sizeof(char)) >= wordMemSize) {
                word = (char*) doubleMemSize(word, &wordMemSize);
                if (word == NULL){
                    free(word);
                    free(wordList);
                    return allocFailure();
                }
            }
            word[wordSize] = c;
            wordSize++;
            c = (char) getchar();
        }
        word[wordSize] = ' ';
        // this is safe, because I allocated two more bytes than the word's size.
        // or I did something horribly wrong. that is also a possibility.
        word[wordSize+1] = '\0';

        wordList[listSize] = word;
        listSize++;
        if (c != EOF)
            c = (char) getchar();
    }

    // sort the array of words according to frobcmp
    qsort(wordList, listSize, sizeof(char*), &frobcmp);
    // print out sorted array
    int i;
    for (i = 0; i < listSize; i++)
        printf("%s", wordList[i]);

    assert(frobcmp("*{_CIA\030\031 ", "*`_GZY\v ") > 0);
    assert(frobcmp("* ", "* ") == 0);
    //printf("All tests passed!");
    return 0;
}

// take void* as arguments, cast them to char const* to make qsort happy
int frobcmp(const void* aVoid, const void* bVoid) {
    char const* a = (char const*) aVoid;
    char const* b = (char const*) bVoid;
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

// doubles memSize and doubles memory size of memory pointed to by ptr
void* doubleMemSize(void* ptr, int* memSize) {
    *memSize *= 2;
    // double it and add two, make room for space and '\0'
    return realloc(ptr, *memSize + 2);
}

int allocFailure() {
    fprintf(stderr, "Error allocating memory; program terminated with exit status 1.");
    return 1;
}