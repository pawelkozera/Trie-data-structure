#include "struktury.h"
#include <allegro5/allegro5.h>
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_primitives.h>
#include <string.h>
#include <sys/time.h>
#include "windows.h"

#define ILOSC_PRZYCISKOW 5

//struct timeval begin, end;
LARGE_INTEGER begin, end;

void czas_start() {
    QueryPerformanceCounter(&begin);
}

double czas_stop() {
    QueryPerformanceCounter(&end);
    LARGE_INTEGER freq;
    QueryPerformanceFrequency(&freq);

    return ((end.QuadPart - begin.QuadPart) * 1000.0) / freq.QuadPart;
}

void wybor_akcji(struct Trie **korzen, int nacisniety_przycisk, char wpisywane_slowo[], struct Komunikaty *komunikaty) {
    switch(nacisniety_przycisk)
    {
        case 0:
            {
                wstaw_do_drzewa(*korzen, wpisywane_slowo);
                czas_start();
                char bufor[50] = "Dodano slowo: ";
                strncat(bufor, wpisywane_slowo, strlen(wpisywane_slowo));
                char napis[10] = " Czas: ";
                strncat(bufor, napis, 8);
                char e[10];
                double elapsed = czas_stop();
                sprintf(e, "%lf", elapsed);
                strncat(bufor, e, 10);
                strcpy(komunikaty->komunikat, bufor);
                break;
            }
        case 1:
            {
                usun_z_drzewa(*korzen, wpisywane_slowo, 0);
                czas_start();
                char bufor[50] = "Usunieto slowo: ";
                strncat(bufor, wpisywane_slowo, strlen(wpisywane_slowo));
                char napis[10] = " Czas: ";
                strncat(bufor, napis, 8);
                char e[10];
                double elapsed = czas_stop();
                sprintf(e, "%lf", elapsed);
                strncat(bufor, e, 10);
                strcpy(komunikaty->komunikat, bufor);
                break;
            }
        case 2:
            {
                if (wyszukaj_z_drzewa(*korzen, wpisywane_slowo)) {
                    czas_start();
                    char bufor[50] = "Znaleziono slowo: ";
                    strncat(bufor, wpisywane_slowo, strlen(wpisywane_slowo));
                    char napis[10] = " Czas: ";
                    strncat(bufor, napis, 8);
                    char e[10];
                    double elapsed = czas_stop();
                    sprintf(e, "%lf", elapsed);
                    strncat(bufor, e, 10);
                    strcpy(komunikaty->komunikat, bufor);
                }
                break;
            }
        case 3:
            {
                if(zapisz_do_pliku(*korzen, wpisywane_slowo) == true) {
                    czas_start();
                    char bufor[50] = "Zapisano do: ";
                    strncat(bufor, wpisywane_slowo, strlen(wpisywane_slowo));
                    char napis[10] = " Czas: ";
                    strncat(bufor, napis, 8);
                    char e[10];
                    double elapsed = czas_stop();
                    sprintf(e, "%lf", elapsed);
                    strncat(bufor, e, 10);
                    strcpy(komunikaty->komunikat, bufor);
                }
                else
                    puts("Operacja zakonczona niepowodzeniem");
                break;
            }
        case 4:
            {
                usun_przed_wczytaniem(*korzen);
                *korzen = stworz_nowe_drzewo_trie();
                if(odczytaj_z_pliku_i_wstaw_do_drzewa(korzen, wpisywane_slowo) == true) {
                    czas_start();
                    char bufor[50] = "Wczytano z: ";
                    char napis[10] = " Czas: ";
                    strncat(bufor, napis, 8);
                    char e[10];
                    double elapsed = czas_stop();
                    sprintf(e, "%lf", elapsed);
                    strncat(bufor, e, 10);
                    strncat(bufor, wpisywane_slowo, strlen(wpisywane_slowo));
                    strcpy(komunikaty->komunikat, bufor);
                }
                else
                    puts("Operacja zakonczona niepowodzeniem");
                break;
            }
        default:
            puts("Brak akcji dla przycisku.");
            break;
    }
}

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

void sterowanie_kamera(struct Kamera *kamera, char znak) {
    int szybkosc = 10;

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
