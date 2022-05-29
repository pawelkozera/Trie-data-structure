/// Podstawowe biblioteki jezyka C ęóąśłżźćń
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>

/// Biblioteki Allegro 5
#include <allegro5/allegro5.h>
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_primitives.h>

/// Biblioteki funkcji i struktur
#include "struktury.h"
#include "rysowanie.h"
#include "drzewo.h"
#include "komunikacja.h"

/// Stale uzywane w kodzie
#define SZEROKOSC_EKRANU 800
#define WYSOKOSC_EKRANU 600
#define SREDNICA_OKREGU 30
#define KEY_SEEN     1
#define KEY_RELEASED 2
#define DLUGOSC_WPISANEGO_SLOWA 50

/*
    Dokumentacja Allegro5
    https://github.com/liballeg/allegro_wiki/wiki/Allegro-Vivace%3A-Basic-game-structure
*/

/// Funkcja glowna
int main()
{
    /// Utworzenie drzewa trie
    struct Trie* korzen = stworz_nowe_drzewo_trie();

    /// Dodanie kamery na pozycji 0, 0
    struct Kamera kamera = {0, 0};

    /// Dodanie komunikatow
    struct Komunikaty komunikaty = {0, ""};

    /// Dodanie przyciskow akcji
    struct Przycisk przycisk_dodaj = {20, WYSOKOSC_EKRANU - 60, 80, 40, "Dodaj"};
    struct Przycisk przycisk_usun = {120, WYSOKOSC_EKRANU - 60, 80, 40, "Usun"};
    struct Przycisk przycisk_znajdz = {220, WYSOKOSC_EKRANU - 60, 80, 40, "Znajdz"};
    struct Przycisk przycisk_zapisz = {320, WYSOKOSC_EKRANU - 60, 80, 40, "Zapisz"};
    struct Przycisk przycisk_wczytaj = {420, WYSOKOSC_EKRANU - 60, 80, 40, "Wczytaj"};

    struct Przycisk przyciski[] = {przycisk_dodaj, przycisk_usun, przycisk_znajdz, przycisk_zapisz, przycisk_wczytaj};

    /// Inicjalizacja elementow Allegro 5: biblioteki, klawiatury, myszki
    al_init();
    al_install_keyboard();
    al_install_mouse();

    /// Aktywacja sesji
    ALLEGRO_TIMER* timer = al_create_timer(1.0 / 288.0);
    ALLEGRO_EVENT_QUEUE* queue = al_create_event_queue();
    /// Aktywacja i ustawienie okna aplikacji oraz czcionki
    ALLEGRO_DISPLAY* disp = al_create_display(SZEROKOSC_EKRANU, WYSOKOSC_EKRANU);
    ALLEGRO_FONT* font = al_create_builtin_font();

    /// Rejestracja wydarzen
    al_register_event_source(queue, al_get_keyboard_event_source());
    al_register_event_source(queue, al_get_display_event_source(disp));
    al_register_event_source(queue, al_get_timer_event_source(timer));
    al_register_event_source(queue, al_get_mouse_event_source());

    /// Zmienna odswiezajaca
    bool redraw = true;
    /// Aktywacja wydarzenia
    ALLEGRO_EVENT event;

    /// Inicjalizacja prostokatow
    al_init_primitives_addon();
    /// Inicjalizacja licznika czasu
    al_start_timer(timer);

    /// Ustawienie klucza
    unsigned char key[ALLEGRO_KEY_MAX];
    memset(key, 0, sizeof(key));

    /// Deklaracja zmiennych wpisywanego slowa i nacisnietego przycisku
    char wpisywane_slowo[DLUGOSC_WPISANEGO_SLOWA] = "";
    int nacisniety_przycisk = -1;

    /// Deklaracja zmiennych wyswietlania drzewa i konca programu
    bool wyswietlanie_drzewa = true;
    bool zakoncz_program = false;

    /// Glowna petla sesji
    while(1) {
        /// Funkcja Allegro czekajaca na dzialanie uzytkownika
        al_wait_for_event(queue, &event);

        /// Funkcje aktywujace sie w zaleznosci od dzialan uzytkownika
        switch(event.type) {
            /// Wydarzenie wcisniecia Escape i klawiszy strzalek: dol, gora, prawo, lewo
            case ALLEGRO_EVENT_TIMER:
                if(key[ALLEGRO_KEY_UP])
                    sterowanie_kamera(&kamera, 'd');
                if(key[ALLEGRO_KEY_DOWN])
                    sterowanie_kamera(&kamera, 'g');
                if(key[ALLEGRO_KEY_LEFT])
                    sterowanie_kamera(&kamera, 'p');
                if(key[ALLEGRO_KEY_RIGHT])
                    sterowanie_kamera(&kamera, 'l');
                if(key[ALLEGRO_KEY_ESCAPE])
                    zakoncz_program = true;

                ///
                for(int i = 0; i < ALLEGRO_KEY_MAX; i++)
                    key[i] &= KEY_SEEN;
                /// Odswiezenie okna
                redraw = true;
                break;

            /// Wydarzenie wcisniecia przycisku
            case ALLEGRO_EVENT_KEY_DOWN:
                key[event.keyboard.keycode] = KEY_SEEN | KEY_RELEASED;
                break;

            /// Wydarzenie puszczenia przycisku
            case ALLEGRO_EVENT_KEY_UP:
                key[event.keyboard.keycode] &= KEY_RELEASED;
                break;

            /// Wydarzenie wpisywania slowa
            case ALLEGRO_EVENT_KEY_CHAR:
                if (!wyswietlanie_drzewa) {
                    if (key[ALLEGRO_KEY_ENTER]) {
                        if (nacisniety_przycisk > -1) {
                            ///
                            wybor_akcji(&korzen, nacisniety_przycisk, wpisywane_slowo, &komunikaty);
                        }
                        wyswietlanie_drzewa = true;
                        wpisywane_slowo[0] = '\0';
                        break;
                    }
                    /// Usuwanie wpisanych liter
                    else if (key[ALLEGRO_KEY_BACKSPACE]) {
                        wpisywane_slowo[strlen(wpisywane_slowo) - 1] = '\0';
                        printf("%s\n", wpisywane_slowo);
                        break;
                    }
                    /// Zabezpieczenie wpisywania odpowiednich znakow ASCII (litery)
                    else {
                        int znak_kod = event.keyboard.unichar;
                        if (znak_kod >= 97 && znak_kod <= 122 || znak_kod == 46) {
                            char znak = znak_kod;
                            strncat(wpisywane_slowo, &znak, 1);
                            printf("%s\n", wpisywane_slowo);
                        }
                        break;
                    }
                }
                break;
            /// Wydarzenie wcisniecia przycisku
            case ALLEGRO_EVENT_MOUSE_BUTTON_DOWN:
                if (event.mouse.button == 1 && wyswietlanie_drzewa) {
                    nacisniety_przycisk = sprawdz_nacisniecie_przycisku(event.mouse.x, event.mouse.y, przyciski);
                    if (nacisniety_przycisk != -1) {
                        wyswietlanie_drzewa = false;
                    }
                }
                break;
            /// Wydarzenie zamykajace okno
            case ALLEGRO_EVENT_DISPLAY_CLOSE:
                zakoncz_program = true;
                break;
        }
        /// Zakonczenie sesji/dzialania programu
        if (zakoncz_program) {
            break;
        }

        /// Wyczyszczenie okna (pomalowanie calego okna na czarno)
        if (redraw && al_is_event_queue_empty(queue))
        {
            al_clear_to_color(al_map_rgb(0, 0, 0));
            /// Wyswietlenie drzewa, przyciskow i komunikatow
            if (wyswietlanie_drzewa) {
                narysuj_drzewo(korzen, SREDNICA_OKREGU + 10, SREDNICA_OKREGU + 120, font, 0, kamera);
                narysuj_linie_drzewa(korzen);
                narysuj_menu(font, przycisk_dodaj);
                narysuj_menu(font, przycisk_usun);
                narysuj_menu(font, przycisk_znajdz);
                narysuj_menu(font, przycisk_wczytaj);
                narysuj_menu(font, przycisk_zapisz);
                narysuj_komunikat(font, komunikaty, przycisk_wczytaj);
            }
            /// Wyswietlenie okna do wpisywania
            else {
                narysuj_wpisywanie(font, wpisywane_slowo);
            }
            ///
            al_flip_display();

            redraw = false;
        }
    }

    /// Zniszczenie funkcji Allegro 5
    al_destroy_font(font);
    al_destroy_display(disp);
    al_destroy_timer(timer);
    al_destroy_event_queue(queue);

    return 0;
}
