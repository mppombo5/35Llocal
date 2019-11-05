// this is here solely for referential purposes

#include <stdio.h>
#include <stdlib.h>

int   frobcmp(const void *a, const void *b);
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
        if (((listSize + 1) * sizeof(char*)) >= listMemSize) {
            char** temp = (char**) doubleMemSize(wordList, &listMemSize);
            if (temp == NULL){
                return allocFailure();
            }
            else
                wordList = temp;
        }

        // start with memory size at 8, double the size and reallocate if it exceeds that
        int wordSize = 0;
        int wordMemSize = sizeof(char) * 8;

        char* word = (char*) malloc(wordMemSize);
        if (word == NULL) {
            free(wordList);
            return allocFailure();
        }

        while (c != ' ' && c != EOF) {
            // allow for an extra 2 bytes so that we can add a space and newline at the end
            if (((wordSize + 2) * sizeof(char)) >= wordMemSize) {
                char* temp = (char*) doubleMemSize(word, &wordMemSize);
                if (temp == NULL){
                    free(wordList);
                    return allocFailure();
                }
                else
                    word = temp;
            }
            word[wordSize] = c;
            wordSize++;
            c = (char) getchar();
        }
        word[wordSize] = ' ';
        word[wordSize+1] = '\0';

        wordList[listSize] = word;
        listSize++;
        if (c != EOF)
            c = (char) getchar();
    }

    // sort the array of words according to frobcmp
    qsort(wordList, listSize, sizeof(char*), frobcmp);
    // print out sorted array
    for (int i = 0; i < listSize; i++) {
        printf("%s", wordList[i]);
        // the omission of this one statement caused me large amounts of pain.
        free(wordList[i]);
    }
    free(wordList);

    return 0;
}

// take void* as arguments, cast them to *(char const**) to make qsort happy
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

// doubles memSize and doubles memory size of memory pointed to by ptr
void* doubleMemSize(void* ptr, int* memSize) {
    *memSize *= 2;
    // double it and add two, make room for space and '\0'
    return realloc(ptr, *memSize);
}

int allocFailure() {
    fprintf(stderr, "Error allocating memory; program terminated with exit status 1.");
    return 1;
}