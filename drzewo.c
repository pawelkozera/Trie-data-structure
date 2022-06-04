#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <allegro5/allegro5.h>
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_primitives.h>

#include "struktury.h"

/// TWORZENIE DRZEWA
/** Funkcja tworząca drzewo. Alokuje pamięć i przygotowuje węzły.

Zwraca wskaznik na korzen. */
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
/// WSTAWIANIE DO DRZEWA
/**
Przyjmuje 2 argumenty:
- wskaznik na korzen drzewa
- tablice znakow, ktore zostane dodane do drzewa. Ostatni znak zostanie lisciem drzewa.
Funkcja wstawiajaca litery do wezłów w drzewie. */
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

/// ZAMIANA LITER NA MAŁE
/** Przyjmuje 1 argument:
- tablice znakow

Wszystkie znaki w tablicy, zostana zamienione na male znaki.
 */
void zamien_na_male_litery(char slowo[]) {
    for (int i = 0; i < strlen(slowo); i++) {
        slowo[i] = tolower(slowo[i]);
    }
}

/// SPRAWDZENIE CZY JEST KOLEJNY WĘZEŁ
/** Przyjmuje 1 argument:
- wskaznik na galaz drzewa

Zwraca true jesli galaz nie ma zadnych dzieci.
 */
bool nie_ma_dzieci(struct Trie *korzen) {
    for (int i = 0; i < LITERY_ALFABETU; i++) {
        if (korzen->litery[i]) {
            return false;
        }
    }
    return true;
}

/// USUNIĘCIE SŁOWA Z DRZEWA
/**
Przyjmuje 3 argumenty:
- wskaznik na korzen drzewa
- tablice znakow, zawierajaca slowo do usuniecia
- glebokosc drzewa, nalezy podac 0

Usuwanie slowa z drzewa poprzez zerowanie węzłów i usuwaniem ich. */
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

/// USUNIĘCIE DRZEWA PRZED WCZYTANIEM NASTĘPNEGO
/**
Przyjmuje 1 argument:
- wskaznik na korzen drzewa

Usuwa wszystkie elementy z drzewa

Zwraca wartosc NULL
 */
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

/// SZUKANIE SŁÓW W DRZEWIE
/**
Przyjmuje 2 argumenty:
- wskaznik na korzen drzewa
- wskaznik na tablice znakow, zawierajaca szukane slowo

Znaki zostaja zamienione na odpowiednie indeksy tablicy i nastepuje sprawdzenie czy istnieja w danym drzewie.

Zwraca wartosc 1 jesli slowo jest w drzewie, 0 jesli go nie mam. */
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

/// ODCZYTYWANIE SŁÓW Z PLIKU I WSTAWIANIE ICH DO DRZEWA
/**
Przyjmuje 2 argumenty:
- podwojny wskaznik na korzen drzewa
- tablice znakow, zawierajaca nazwe pliku. Nazwa musi zawierac rozszerzenie np. "nazwa.txt"

Odczytuje z pliku zapisane slowa i wstawia je do drzewa.

Zwraca true jeśli operacja się powiedzie.
 */
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

/// ZAPISYWANIE DRZEWA DO PLIKU
/**
Przyjmuje 2 argumenty:
- wskaznik na korzen drzewa
- tablice znakow, zawierajaca nazwe pliku. Nazwa musi zawierac rozszerzenie np. "nazwa.txt"

Tworzy nowy plik i wywoluje funkcje pomocniczna "zapisz_drzewo()" dodajaca drzewo do pliku.

Zwraca true jesli operacja powiedzie sie.
 */
bool zapisz_do_pliku(struct Trie *korzen, char nazwa_pliku[]) {
    FILE *plik;
    plik = fopen(nazwa_pliku, "w");
    fclose(plik);

    char slowa[30];
    zapisz_drzewo(korzen, slowa, 0, nazwa_pliku);
    return 1;
}

/// ZAPISANIE DRZEWA
/**
Przyjmuje 4 argumenty:
- wskaznik na korzen drzewa
- pusta tablice znakow
- index tablicy, nalezy podac 0
- tablice znakow, zawierajaca nazwe pliku. Nazwa musi zawierac rozszerzenie np. "nazwa.txt"

Funkcja pomocniczna do dodania slow z drzew do pliku.
Funkcja nie tworzy nowego pliku ale dodaje slowa do juz istniejacego.
 */
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
