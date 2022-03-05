#include <stdio.h>
#include <stdlib.h>

#define LITERY_ALFABETU 32

struct Trie {
    int jest_lisciem; // 1 gdy jest lisciem, 0 gdy nie jest
    struct Trie* litery[LITERY_ALFABETU];
};


struct Trie* stworz_nowe_drzewo_trie();

int main()
{
    struct Trie* korzen = stworz_nowe_drzewo_trie();

    return 0;
}

struct Trie* stworz_nowe_drzewo_trie() {
    struct Trie* drzewo = (struct Trie*)malloc(sizeof(struct Trie));
    drzewo->jest_lisciem = 0;

    for (int i = 0; i < LITERY_ALFABETU; i++) {
        drzewo->litery[i] = NULL;
    }

    return drzewo;
};
