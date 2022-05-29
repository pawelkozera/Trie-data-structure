/// Biblioteki Allegro 5
#include <allegro5/allegro5.h>
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_primitives.h>
#include "struktury.h"

/// Stale uzywane w kodzie
#define SREDNICA_OKREGU 30

#define SZEROKOSC_EKRANU 800
#define WYSOKOSC_EKRANU 600

/// Funkcja rysujaca okregi wezlow
int narysuj_drzewo(struct Trie *korzen, int szerokosc, int wysokosc, ALLEGRO_FONT* font, int glebokosc, struct Kamera kamera) {
    if (korzen != NULL) {
        char litera[] = "a";
        int szerokosci[LITERY_ALFABETU] = {0};
        int n_szerokosci = 0;
        /// Petla rysujaca wezly
        for (int i = 0; i < LITERY_ALFABETU; i++) {
            if (korzen->litery[i] != NULL) {
                /// Rekurencyjne ustawienie wezla w odpowiedniej pozycji
                szerokosc = narysuj_drzewo(korzen->litery[i], szerokosc, wysokosc + 100, font, glebokosc+1, kamera);
                litera[0] = 'a' + i;

                korzen->litery[i]->x = szerokosc + kamera.x;
                korzen->litery[i]->y = wysokosc + kamera.y;
                /// Rysowanie wezla
                al_draw_circle(korzen->litery[i]->x, korzen->litery[i]->y, SREDNICA_OKREGU, al_map_rgb(0, 255, 0), 2);
                al_draw_textf(font, al_map_rgb(255, 255, 255), szerokosc + kamera.x, wysokosc + kamera.y, 0, litera);

                szerokosc += SREDNICA_OKREGU + 40;
            }
        }
        ///
        if (glebokosc == 0) {
            al_draw_circle(szerokosc/2 + kamera.x, SREDNICA_OKREGU + kamera.y, SREDNICA_OKREGU, al_map_rgb(0, 255, 0), 2);
            al_draw_textf(font, al_map_rgb(255, 255, 255), szerokosc/2 + kamera.x, SREDNICA_OKREGU + kamera.y, 0, "#");
            /// Petla do rysowania okregu
            for (int i = 0; i < LITERY_ALFABETU; i++) {
                if (korzen->litery[i] != NULL) {
                    /// Rysowanie okregu
                    al_draw_line(szerokosc/2 + kamera.x, SREDNICA_OKREGU*2 + kamera.y, korzen->litery[i]->x, korzen->litery[i]->y - SREDNICA_OKREGU, al_map_rgb_f(1, 0, 0), 1);
                }
            }
        }
    }

    return szerokosc;
}

/// Funkcja rysujaca linie laczace wezly
void narysuj_linie_drzewa(struct Trie *korzen) {
    if (korzen == NULL) {
        return;
    }
    /// Petle laczace odpowiednie wezly
    for (int i = 0; i < LITERY_ALFABETU; i++) {
        if (korzen->litery[i] != NULL) {
            for (int j = 0; j < LITERY_ALFABETU; j++) {
                if (korzen->litery[i]->litery[j] != NULL) {
                    /// Rysowanie linii
                    al_draw_line(korzen->litery[i]->x, korzen->litery[i]->y + SREDNICA_OKREGU, korzen->litery[i]->litery[j]->x, korzen->litery[i]->litery[j]->y - SREDNICA_OKREGU, al_map_rgb_f(1, 0, 0), 1);
                }
            }
            /// Rekurencja
            narysuj_linie_drzewa(korzen->litery[i]);
        }
    }
}

/// Funkcja rysujaca menu
void narysuj_menu(ALLEGRO_FONT* font, struct Przycisk przycisk) {
    /// Rysowanie przyciskow i tekstu
    al_draw_filled_rectangle(przycisk.x, przycisk.y, przycisk.x + przycisk.szerokosc, przycisk.y + przycisk.wysokosc, al_map_rgb(112, 110, 104));
    al_draw_textf(font, al_map_rgb(255, 255, 255), przycisk.x + 10, przycisk.y + (przycisk.wysokosc/2), 0, przycisk.napis);
}

/// Funkcja rysujaca komunikaty
void narysuj_komunikat(ALLEGRO_FONT*font, struct Komunikaty komunikaty, struct Przycisk przycisk_wczytaj) {
    al_draw_textf(font, al_map_rgb(255, 255, 255), przycisk_wczytaj.x + przycisk_wczytaj.szerokosc + 20, przycisk_wczytaj.y + (przycisk_wczytaj.wysokosc/2), 0, komunikaty.komunikat);
}

/// Funkcja rysujaca wpisywane litery
void narysuj_wpisywanie(ALLEGRO_FONT* font, char wpisywane_slowo[]) {
    al_draw_textf(font, al_map_rgb(255, 255, 255), SZEROKOSC_EKRANU/3, WYSOKOSC_EKRANU/3, 0, "Wpisz slowo");
    al_draw_textf(font, al_map_rgb(255, 255, 255), SZEROKOSC_EKRANU/3, WYSOKOSC_EKRANU/2, 0, wpisywane_slowo);
    al_draw_textf(font, al_map_rgb(255, 255, 255), SZEROKOSC_EKRANU/3, WYSOKOSC_EKRANU/1.5, 0, "Zaakceptuj enterem");
}
