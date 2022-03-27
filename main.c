#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <allegro5/allegro5.h>
#include <string.h>

#define LITERY_ALFABETU 26

/*
    Dokumentacja Allegro5
    https://github.com/liballeg/allegro_wiki/wiki/Allegro-Vivace%3A-Basic-game-structure
*/

struct Trie {
    int jest_lisciem; // 1 gdy jest lisciem, 0 gdy nie jest
    struct Trie* litery[LITERY_ALFABETU];
};

//Prototypy funkcji
struct Trie* stworz_nowe_drzewo_trie();
void wstaw_do_drzewa(struct Trie *korzen, char slowo[]);
int wyszukaj_z_drzewa(struct Trie *korzen, char *slowo);
void zamien_na_male_litery(char slowo[]);
int usun_z_drzewa(struct Trie **aktualny_wezel, char *slowo);
int ma_pod_wezly(struct Trie *aktualny_wezel);
void odczytaj_z_pliku_i_wstaw_do_drzewa(struct Trie *korzen);
void zapisz_do_pliku(struct Trie *korzen);
void zapisz_drzewo(struct Trie *korzen, char slowa[], int index);
void wypisz_drzewo(struct Trie *korzen, char slowa[], int index);
void wybor_akcji(struct Trie *korzen);

int main()
{
    struct Trie* korzen = stworz_nowe_drzewo_trie();
    wybor_akcji(korzen);

    char slowa[30];
    wypisz_drzewo(korzen, slowa, 0);

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

void odczytaj_z_pliku_i_wstaw_do_drzewa(struct Trie *korzen) {
    FILE *plik;
    plik = fopen("slownik.txt", "r");

    if (plik == NULL) {
        exit(1);
    }

    char linia[40];
    while (fgets(linia, sizeof(linia), plik)) {
        linia[strcspn(linia, "\n")] = 0; //usuwa znak nowej lini z ciagu znakow
        wstaw_do_drzewa(korzen, linia);
    }

    fclose(plik);
}

void zapisz_do_pliku(struct Trie *korzen) {
    FILE *plik;
    plik = fopen("slownik.txt", "w");
    fclose(plik);

    char slowa[30];
    zapisz_drzewo(korzen, slowa, 0);
}

void zapisz_drzewo(struct Trie *korzen, char slowa[], int index) {
    if (korzen != NULL) {
        if (korzen->jest_lisciem) {
            slowa[index] = '\0';
            FILE *plik;
            plik = fopen("slownik.txt", "a");
            fprintf(plik, "%s\n", slowa);

            fclose(plik);
        }
        for (int i = 0; i < LITERY_ALFABETU; i++) {
            if (korzen->litery[i] != NULL) {
                slowa[index] = 'a' + i;
                zapisz_drzewo(korzen->litery[i], slowa, index + 1);
            }
        }
    }
}

void wypisz_drzewo(struct Trie *korzen, char slowa[], int index) {
    if (korzen != NULL) {
        if (korzen->jest_lisciem) {
            slowa[index] = '\0';
            printf("%s\n", slowa);
        }
        for (int i = 0; i < LITERY_ALFABETU; i++) {
            if (korzen->litery[i] != NULL) {
                slowa[index] = 'a' + i;
                wypisz_drzewo(korzen->litery[i], slowa, index + 1);
            }
        }
    }
}

void wybor_akcji(struct Trie *korzen)
{
    int a;
    do
    {
        puts("Wybierz akcje: \n1 - Wstaw do drzewa\n2 - Usun z drzewa\n3 - Wyszukaj w drzewie\n4 - Odczytaj drzewo z pliku\n5 - Zapisz drzewo do pliku");
        scanf("%d", &a);
    }
    while((a < 0) && (a > 6));
    switch(a)
    {
        case 1:
            {
                char z[20];
                puts("Napisz co wstawic: ");
                scanf("%s", &z);
                wstaw_do_drzewa(korzen, z);
                break;
            }/*
        case 2:
            {
                usun_z_drzewa(korzen, z);
                break;
            }
        case 3:
            {
                wyszukaj_z_drzewa(korzen, z);
                break;
            }
        case 4:
            {
                odczytaj_z_pliku_i_wstaw_do_drzewa(korzen);
                break;
            }
        case 5:
            {
                zapisz_do_pliku(korzen);
                break;
            }*/
        default:
            puts("Blad wyboru akcji");
            break;
    }
}
