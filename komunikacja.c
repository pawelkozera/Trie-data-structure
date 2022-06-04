#include <string.h>
#include <sys/time.h>
#include "windows.h"
#include <allegro5/allegro5.h>
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_primitives.h>

#include "struktury.h"

#define ILOSC_PRZYCISKOW 5

//struct timeval begin, end;
LARGE_INTEGER begin, end;

/// ROZPOCZĘCIE LICZENIA CZASU
/**  */
void czas_start() {
    QueryPerformanceCounter(&begin);
}

/// ZATRZYMANIE LICZENIA CZASU
/**  */
double czas_stop() {
    QueryPerformanceCounter(&end);
    LARGE_INTEGER freq;
    QueryPerformanceFrequency(&freq);

    return ((end.QuadPart - begin.QuadPart) * 1000.0) / freq.QuadPart;
}

/// AKTYWACJA AKCJI
/**
Przyjmuje 4 argumenty:
- podwojny wskaznik na korzen
- indeks okreslajacy ktory przycisk zostal nacisniety
- tablice znakow, zawierajaca wpisywane slowo
- wskaznik na strukture komunikaty

W zależności od wciśniętego przycisku aktywują się funkcje: wstawiania, usunięcia, wyszukania słowa z drzewa
 oraz wstawienia lub usunięcia pliku z drzewem. */
void wybor_akcji(struct Trie **korzen, int nacisniety_przycisk, char wpisywane_slowo[], struct Komunikaty *komunikaty) {
    switch(nacisniety_przycisk)
    {
        case 0:
            {
                czas_start();
                wstaw_do_drzewa(*korzen, wpisywane_slowo);
                char bufor[50] = "Dodano slowo: ";
                strncat(bufor, wpisywane_slowo, strlen(wpisywane_slowo));
                strcpy(komunikaty->komunikat, bufor);

                char bufor2[50] = "";
                char napis[10] = "Czas: ";
                strncat(bufor2, napis, 8);
                char e[10];
                double elapsed = czas_stop();
                sprintf(e, "%lf", elapsed);
                strncat(bufor2, e, 10);
                strcpy(komunikaty->komunikat_czas, bufor2);
                break;
            }
        case 1:
            {
                czas_start();
                korzen = usun_z_drzewa(*korzen, wpisywane_slowo, 0);
                if (korzen == NULL) {
                    stworz_nowe_drzewo_trie(korzen);
                }
                char bufor[50] = "Usunieto slowo: ";
                strncat(bufor, wpisywane_slowo, strlen(wpisywane_slowo));
                strcpy(komunikaty->komunikat, bufor);

                char bufor2[50] = "";
                char napis[10] = "Czas: ";
                strncat(bufor2, napis, 8);
                char e[10];
                double elapsed = czas_stop();
                sprintf(e, "%lf", elapsed);
                strncat(bufor2, e, 10);
                strcpy(komunikaty->komunikat_czas, bufor2);
                break;
            }
        case 2:
            {
                czas_start();
                bool wyszukano = wyszukaj_z_drzewa(*korzen, wpisywane_slowo);
                if (wyszukano) {
                    double elapsed = czas_stop();
                    char bufor[50] = "Znaleziono slowo: ";
                    strncat(bufor, wpisywane_slowo, strlen(wpisywane_slowo));
                    strcpy(komunikaty->komunikat, bufor);

                    char bufor2[50] = "";
                    char napis[10] = "Czas: ";
                    strncat(bufor2, napis, 8);
                    char e[10];
                    sprintf(e, "%lf", elapsed);
                    strncat(bufor2, e, 10);
                    strcpy(komunikaty->komunikat_czas, bufor2);
                }
                else {
                    double elapsed = czas_stop();
                    char bufor[50] = "Nie znaleziono slowa: ";
                    strncat(bufor, wpisywane_slowo, strlen(wpisywane_slowo));
                    strcpy(komunikaty->komunikat, bufor);

                    char bufor2[50] = "";
                    char napis[10] = "Czas: ";
                    strncat(bufor2, napis, 8);
                    char e[10];
                    sprintf(e, "%lf", elapsed);
                    strncat(bufor2, e, 10);
                    strcpy(komunikaty->komunikat_czas, bufor2);
                }
                break;
            }
        case 3:
            {
                czas_start();
                if(zapisz_do_pliku(*korzen, wpisywane_slowo) == true) {
                    double elapsed = czas_stop();
                    char bufor[50] = "Zapisano do: ";
                    strncat(bufor, wpisywane_slowo, strlen(wpisywane_slowo));
                    strcpy(komunikaty->komunikat, bufor);

                    char bufor2[50] = "";
                    char napis[10] = "Czas: ";
                    strncat(bufor2, napis, 8);
                    char e[10];
                    sprintf(e, "%lf", elapsed);
                    strncat(bufor2, e, 10);
                    strcpy(komunikaty->komunikat_czas, bufor2);
                }
                else {
                    double elapsed = czas_stop();
                    puts("Operacja zakonczona niepowodzeniem");
                }
                break;
            }
        case 4:
            {
                czas_start();
                usun_przed_wczytaniem(*korzen);
                *korzen = stworz_nowe_drzewo_trie();
                if(odczytaj_z_pliku_i_wstaw_do_drzewa(korzen, wpisywane_slowo) == true) {
                    double elapsed = czas_stop();
                    char bufor[50] = "Wczytano z: ";
                    strncat(bufor, wpisywane_slowo, strlen(wpisywane_slowo));
                    strcpy(komunikaty->komunikat, bufor);

                    char bufor2[50] = "";
                    char napis[10] = "Czas: ";
                    strncat(bufor2, napis, 8);
                    char e[10];
                    sprintf(e, "%lf", elapsed);
                    strncat(bufor2, e, 10);
                    strcpy(komunikaty->komunikat_czas, bufor2);
                }
                else {
                    double elapsed = czas_stop();
                    puts("Operacja zakonczona niepowodzeniem");
                }
                break;
            }
        default:
            puts("Brak akcji dla przycisku.");
            break;
    }
}

/// SPRAWDZENIE WCIŚNIĘCIA PRZYCISKU MYSZKĄ
/**
Przyjmuje 3 argumenty:
- pozycje x myszy
- pozycje y myszy
- tablice struktur przyciski

Sprawdza czy przycisk zostal nacisniety.

Zwraca numer nacisnietego przycisku, jesli nie zostal nacisniety zaden to zwraca -1.
 */
int sprawdz_nacisniecie_przycisku(int mysz_x, int mysz_y, struct Przycisk przyciski[]) {
    bool myszka_w_zakresie_x = false;
    bool myszka_w_zakresie_y = false;

    for (int i = 0; i < ILOSC_PRZYCISKOW; i++) {
        myszka_w_zakresie_x = (mysz_x >= przyciski[i].x && mysz_x <= przyciski[i].x + przyciski[i].szerokosc);
        myszka_w_zakresie_y = (mysz_y >= przyciski[i].y && mysz_y <= przyciski[i].y + przyciski[i].wysokosc);
        if (myszka_w_zakresie_x && myszka_w_zakresie_y) {
            return i;
        }
    }

    return -1;
}

/// STEROWANIE KAMERĄ PO OKNIE
/**
Przyjmuje 2 argumenty:
- wskaznik na strukture kamera
- znak, okreslajacy w ktora strone nalezy przesunac kamere (g = gora, d = dol, l = lewo, p = prawo)

Przesuwa kamere o 5 pikseli w danym kierunku.
 */
void sterowanie_kamera(struct Kamera *kamera, char znak) {
    int szybkosc = 5;

    if (znak == 'g') {
        kamera->y -= szybkosc;
    }
    else if (znak == 'd') {
        kamera->y += szybkosc;
    }
    else if (znak == 'l') {
        kamera->x -= szybkosc;
    }
    else if (znak == 'p') {
        kamera->x += szybkosc;
    }
}
