#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <allegro5/allegro5.h>
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_primitives.h>

#include "struktury.h"

struct Trie* stworz_nowe_drzewo_trie() {
    struct Trie* drzewo = (struct Trie*)malloc(sizeof(struct Trie));
    drzewo->jest_lisciem = 0;
    drzewo->x = 0;
    drzewo->y = 0;

    for (int i = 0; i < LITERY_ALFABETU; i++) {
        drzewo->litery[i] = NULL;
    }

    return drzewo;
};


/*
'a' == 97
'b' == 98
'a' - 'a' = 0
'b' - 'a' = 1
odejmujac 'a' otrzymujemy indeksy od 0 do 25, dla alfabetu od a do z bez Polskich znakow
*/
void wstaw_do_drzewa(struct Trie *korzen, char slowo[]) {
    struct Trie *aktualny_wezel = korzen;
    zamien_na_male_litery(slowo);

    while (*slowo) {
        if (aktualny_wezel->litery[*slowo - 'a'] == NULL) {
            aktualny_wezel->litery[*slowo - 'a'] = stworz_nowe_drzewo_trie();
        }
        aktualny_wezel = aktualny_wezel->litery[*slowo - 'a'];
        slowo++;
    }

    aktualny_wezel->jest_lisciem = 1;
}

void zamien_na_male_litery(char slowo[]) {
    for (int i = 0; i < strlen(slowo); i++) {
        slowo[i] = tolower(slowo[i]);
    }
}

bool nie_ma_dzieci(struct Trie *korzen) {
    for (int i = 0; i < LITERY_ALFABETU; i++) {
        if (korzen->litery[i]) {
            return false;
        }
    }
    return true;
}

struct Trie* usun_z_drzewa(struct Trie *korzen, char slowo[], int glebokosc) {
    if (korzen == NULL) {
        return NULL;
    }

    if (strlen(slowo) == glebokosc) {
        if (korzen->jest_lisciem) {
            korzen->jest_lisciem = 0;
        }

        if (nie_ma_dzieci(korzen)) {
            free(korzen);
            korzen = NULL;
        }

        return korzen;
    }

    int index = slowo[glebokosc] - 'a';
    korzen->litery[index] = usun_z_drzewa(korzen->litery[index], slowo, glebokosc + 1);

    if (nie_ma_dzieci(korzen) && korzen->jest_lisciem == 0) {
        free(korzen);
        korzen = NULL;
    }

    return korzen;
}

struct Trie* usun_przed_wczytaniem(struct Trie *korzen) {
    for (int i = 0; i < LITERY_ALFABETU; i++) {
        if (korzen->litery[i]) {
            usun_przed_wczytaniem(korzen->litery[i]);
            printf("%c", i+'a');
        }
    }
    free(korzen);
    return NULL;
};

// zwraca 1 jesli slowo jest w drzewie, 0 jesli go nie ma
int wyszukaj_z_drzewa(struct Trie *korzen, char *slowo) {
    if (korzen == NULL) {
        return 0; // drzewo jest puste
    }

    struct Trie* aktualny_wezel = korzen;
    while (*slowo) {
        aktualny_wezel = aktualny_wezel->litery[*slowo - 'a'];
        if (aktualny_wezel == NULL) {
            return 0;
        }
        slowo++;
    }

    return aktualny_wezel->jest_lisciem;
}

bool odczytaj_z_pliku_i_wstaw_do_drzewa(struct Trie **korzen, char nazwa_pliku[]) {
    FILE *plik;
    plik = fopen(nazwa_pliku, "r");

    if (plik == NULL) {
        exit(1);
    }

    char linia[40];
    memset(linia, 0, 40);
    while (fgets(linia, sizeof(linia), plik)) {
        linia[strcspn(linia, "\n")] = 0; //usuwa znak nowej lini z ciagu znakow
        wstaw_do_drzewa(*korzen, linia);
        printf("\n%s", linia);
        memset(linia, 0, 40);
    }

    fclose(plik);
    return 1;
}

bool zapisz_do_pliku(struct Trie *korzen, char nazwa_pliku[]) {
    FILE *plik;
    plik = fopen(nazwa_pliku, "w");
    fclose(plik);

    char slowa[30];
    zapisz_drzewo(korzen, slowa, 0, nazwa_pliku);
    return 1;
}

void zapisz_drzewo(struct Trie *korzen, char slowa[], int index, char nazwa_pliku[]) {
    if (korzen != NULL) {
        if (korzen->jest_lisciem) {
            slowa[index] = '\0';
            FILE *plik;
            plik = fopen(nazwa_pliku, "a");
            fprintf(plik, "%s\n", slowa);

            fclose(plik);
        }
        for (int i = 0; i < LITERY_ALFABETU; i++) {
            if (korzen->litery[i] != NULL) {
                slowa[index] = 'a' + i;
                zapisz_drzewo(korzen->litery[i], slowa, index + 1, nazwa_pliku);
            }
        }
    }
}
