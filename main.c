#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <allegro5/allegro.h>
#include <string.h>

#define LITERY_ALFABETU 26

struct Trie {
    int jest_lisciem; // 1 gdy jest lisciem, 0 gdy nie jest
    struct Trie* litery[LITERY_ALFABETU];
};

//Porotypy funkcji
struct Trie* stworz_nowe_drzewo_trie();
void wstaw_do_drzewa(struct Trie *korzen, char slowo[]);
int wyszukaj_z_drzewa(struct Trie *korzen, char *slowo);
void zamien_na_male_litery(char slowo[]);
int usun_z_drzewa(struct Trie **aktualny_wezel, char *slowo);
int ma_pod_wezly(struct Trie *aktualny_wezel);

int main()
{
    struct Trie* korzen = stworz_nowe_drzewo_trie();

    char z[] = "kot";
    wstaw_do_drzewa(korzen, z);
    printf("%d\n", wyszukaj_z_drzewa(korzen, "kot"));
    usun_z_drzewa(&korzen, "kot");
    printf("%d\n", wyszukaj_z_drzewa(korzen, "kot"));

    al_init();

    return 0;
}

//Funkcje
struct Trie* stworz_nowe_drzewo_trie() {
    struct Trie* drzewo = (struct Trie*)malloc(sizeof(struct Trie));
    drzewo->jest_lisciem = 0;

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
    struct Trie* aktualny_wezel = korzen;
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

// zwraca 1 jestli slowo jest w drzewie, 0 jesli go nie ma
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

int ma_pod_wezly(struct Trie *aktualny_wezel) {
    for (int i = 0; i < LITERY_ALFABETU; i++) {
        if (aktualny_wezel->litery[i]) {
            return 1; // znaleziono
        }
    }

    return 0;
}

int usun_z_drzewa(struct Trie **aktualny_wezel, char *slowo) {
    if (*aktualny_wezel == NULL) {
        return 0; // jeœli drzewo jest puste zwracamy 0
    }

    if (*slowo) {
        if ((*aktualny_wezel)->litery[*slowo - 'a'] != NULL && usun_z_drzewa(&((*aktualny_wezel)->litery[*slowo - 'a']), slowo + 1) && (*aktualny_wezel)->jest_lisciem == 0) {
            if (!ma_pod_wezly(*aktualny_wezel)) {
                free(*aktualny_wezel);
                (*aktualny_wezel) == NULL;
                return 1;
            }
            else {
                return 0;
            }
        }
    }

    if (*slowo == '\0' && (*aktualny_wezel)->jest_lisciem) {
        if (!ma_pod_wezly(*aktualny_wezel)) {
            free(*aktualny_wezel);
            (*aktualny_wezel) = NULL;
            return 1;
        }
        else {
            (*aktualny_wezel)->jest_lisciem = 0;
            return 0;
        }
    }

    return 0;
}
