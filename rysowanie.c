#include <allegro5/allegro5.h>
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_primitives.h>
#include "struktury.h"

#define SREDNICA_OKREGU 30

#define SZEROKOSC_EKRANU 800
#define WYSOKOSC_EKRANU 600

/// RYSOWANIE OKRĘGÓW WĘZŁÓW
/**
Przyjmuje 6 argumentow:
 - wskaznik na korzen drzewa
 - szerokosc rozpoczecia rysowania (pozycja x)
 - wysokosc rozpoczecia rysowania (pozycja y)
 - wskaznik na Allegro font
 - glebokosc drzewa, nalezy podac 0
 - strukture kamera

Funkcja rekurencyjna, ktora kazdemu elementowi drzewa poczynajac od lewego dolnego,
przyporzadkowuje pozycje x i y oraz rysuje w tej pozycji przyporzadkowana litere z drzewa i okrag dookola niej.
Dla wierzcholka drzewa rysowany jest symbol "#".

Zwraca polozenie x ostatniego elementu.
*/
int narysuj_drzewo(struct Trie *korzen, int szerokosc, int wysokosc, ALLEGRO_FONT* font, int glebokosc, struct Kamera kamera) {
    if (korzen != NULL) {
        char litera[] = "a";
        int szerokosci[LITERY_ALFABETU] = {0};
        int n_szerokosci = 0;
        for (int i = 0; i < LITERY_ALFABETU; i++) {
            if (korzen->litery[i] != NULL) {
                szerokosc = narysuj_drzewo(korzen->litery[i], szerokosc, wysokosc + 100, font, glebokosc+1, kamera);
                litera[0] = 'a' + i;

                korzen->litery[i]->x = szerokosc + kamera.x;
                korzen->litery[i]->y = wysokosc + kamera.y;
                al_draw_circle(korzen->litery[i]->x, korzen->litery[i]->y, SREDNICA_OKREGU, al_map_rgb(0, 255, 0), 2);
                al_draw_textf(font, al_map_rgb(255, 255, 255), szerokosc + kamera.x, wysokosc + kamera.y, 0, litera);

                szerokosc += SREDNICA_OKREGU + 40;
            }
        }
        if (glebokosc == 0) {
            al_draw_circle(szerokosc/2 + kamera.x, SREDNICA_OKREGU + kamera.y, SREDNICA_OKREGU, al_map_rgb(0, 255, 0), 2);
            al_draw_textf(font, al_map_rgb(255, 255, 255), szerokosc/2 + kamera.x, SREDNICA_OKREGU + kamera.y, 0, "#");
            for (int i = 0; i < LITERY_ALFABETU; i++) {
                if (korzen->litery[i] != NULL) {
                    al_draw_line(szerokosc/2 + kamera.x, SREDNICA_OKREGU*2 + kamera.y, korzen->litery[i]->x, korzen->litery[i]->y - SREDNICA_OKREGU, al_map_rgb_f(1, 0, 0), 1);
                }
            }
        }
    }

    return szerokosc;
}

/// Funkcja rysujaca linie laczace wezly
/**
Przyjmuje 1 argument:
- wskaznik na korzen drzewa

Funkcja rekurencyjna rysujaca linie laczaca element drzewa z kazdym jej dzieckiem.
 */
void narysuj_linie_drzewa(struct Trie *korzen) {
    if (korzen == NULL) {
        return;
    }
    for (int i = 0; i < LITERY_ALFABETU; i++) {
        if (korzen->litery[i] != NULL) {
            for (int j = 0; j < LITERY_ALFABETU; j++) {
                if (korzen->litery[i]->litery[j] != NULL) {
                    al_draw_line(korzen->litery[i]->x, korzen->litery[i]->y + SREDNICA_OKREGU, korzen->litery[i]->litery[j]->x, korzen->litery[i]->litery[j]->y - SREDNICA_OKREGU, al_map_rgb_f(1, 0, 0), 1);
                }
            }
            narysuj_linie_drzewa(korzen->litery[i]);
        }
    }
}

/// RYSOWANIE MENU
/**
Przyjmuje 2 argumenty:
- wskaznik na Allegro font
- strukture przycisk

Rysowanie przycisków i tekstu na nich. */
void narysuj_menu(ALLEGRO_FONT* font, struct Przycisk przycisk) {
    al_draw_filled_rectangle(przycisk.x, przycisk.y, przycisk.x + przycisk.szerokosc, przycisk.y + przycisk.wysokosc, al_map_rgb(112, 110, 104));
    al_draw_textf(font, al_map_rgb(255, 255, 255), przycisk.x + 10, przycisk.y + (przycisk.wysokosc/2), 0, przycisk.napis);
}

/// RYSOWANIE KOMUNIKATÓW
/**
Przyjmuje 3 argumenty:
- wskaznik na Allegro font
- strukture komunikaty
- strukture przycisk

Rysuje komunikat.
 */
void narysuj_komunikat(ALLEGRO_FONT* font, struct Komunikaty komunikaty, struct Przycisk przycisk_wczytaj) {
    al_draw_textf(font, al_map_rgb(255, 255, 255), przycisk_wczytaj.x + przycisk_wczytaj.szerokosc + 20, przycisk_wczytaj.y + (przycisk_wczytaj.wysokosc/2), 0, komunikaty.komunikat);
    al_draw_textf(font, al_map_rgb(255, 255, 255), przycisk_wczytaj.x + przycisk_wczytaj.szerokosc + 20, przycisk_wczytaj.y + (przycisk_wczytaj.wysokosc/2) + 20, 0, komunikaty.komunikat_czas);
}

/// RYSOWANIE LITER
/**
Przyjmuje 2 argumenty:
- wskaznik na Allegro font
- tablice znakow wpisywanych przez uzytkownika

Rysowanie liter wpisywanych przez użytkownika. */
void narysuj_wpisywanie(ALLEGRO_FONT* font, char wpisywane_slowo[]) {
    al_draw_textf(font, al_map_rgb(255, 255, 255), SZEROKOSC_EKRANU/3, WYSOKOSC_EKRANU/3, 0, "Wpisz slowo");
    al_draw_textf(font, al_map_rgb(255, 255, 255), SZEROKOSC_EKRANU/3, WYSOKOSC_EKRANU/2, 0, wpisywane_slowo);
    al_draw_textf(font, al_map_rgb(255, 255, 255), SZEROKOSC_EKRANU/3, WYSOKOSC_EKRANU/1.5, 0, "Zaakceptuj enterem");
}
