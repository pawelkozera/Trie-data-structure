#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <allegro5/allegro5.h>
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_primitives.h>

#include "struktury.h"
#include "rysowanie.h"
#include "drzewo.h"
#include "komunikacja.h"

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

int main()
{
    struct Trie* korzen = stworz_nowe_drzewo_trie();

    struct Kamera kamera = {0, 0};

    struct Komunikaty komunikaty = {0, ""};

    struct Przycisk przycisk_dodaj = {20, WYSOKOSC_EKRANU - 60, 80, 40, "Dodaj"};
    struct Przycisk przycisk_usun = {120, WYSOKOSC_EKRANU - 60, 80, 40, "Usun"};
    struct Przycisk przycisk_znajdz = {220, WYSOKOSC_EKRANU - 60, 80, 40, "Znajdz"};
    struct Przycisk przycisk_zapisz = {320, WYSOKOSC_EKRANU - 60, 80, 40, "Zapisz"};
    struct Przycisk przycisk_wczytaj = {420, WYSOKOSC_EKRANU - 60, 80, 40, "Wczytaj"};

    struct Przycisk przyciski[] = {przycisk_dodaj, przycisk_usun, przycisk_znajdz, przycisk_zapisz, przycisk_wczytaj};

    //Allegro5
    al_init();
    al_install_keyboard();
    al_install_mouse();

    ALLEGRO_TIMER* timer = al_create_timer(1.0 / 60.0);
    ALLEGRO_EVENT_QUEUE* queue = al_create_event_queue();
    ALLEGRO_DISPLAY* disp = al_create_display(SZEROKOSC_EKRANU, WYSOKOSC_EKRANU);
    ALLEGRO_FONT* font = al_create_builtin_font();

    al_register_event_source(queue, al_get_keyboard_event_source());
    al_register_event_source(queue, al_get_display_event_source(disp));
    al_register_event_source(queue, al_get_timer_event_source(timer));
    al_register_event_source(queue, al_get_mouse_event_source());

    bool redraw = true;
    ALLEGRO_EVENT event;

    al_init_primitives_addon();

    al_start_timer(timer);

    unsigned char key[ALLEGRO_KEY_MAX];
    memset(key, 0, sizeof(key));

    char wpisywane_slowo[DLUGOSC_WPISANEGO_SLOWA] = "";
    int nacisniety_przycisk = -1;

    bool wyswietlanie_drzewa = true;
    bool zakoncz_program = false;
    while(1) {
        al_wait_for_event(queue, &event);

        switch(event.type) {
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

                for(int i = 0; i < ALLEGRO_KEY_MAX; i++)
                    key[i] &= KEY_SEEN;

                redraw = true;
                break;

            case ALLEGRO_EVENT_KEY_DOWN:
                key[event.keyboard.keycode] = KEY_SEEN | KEY_RELEASED;
                break;

            case ALLEGRO_EVENT_KEY_UP:
                key[event.keyboard.keycode] &= KEY_RELEASED;
                break;

            case ALLEGRO_EVENT_KEY_CHAR: // wpisywanie szukanej wartosci
                if (!wyswietlanie_drzewa) {
                    if (key[ALLEGRO_KEY_ENTER]) {
                        if (nacisniety_przycisk > -1) {
                            wybor_akcji(&korzen, nacisniety_przycisk, wpisywane_slowo, &komunikaty);
                        }
                        wyswietlanie_drzewa = true;
                        wpisywane_slowo[0] = '\0';
                        break;
                    }
                    else if (key[ALLEGRO_KEY_BACKSPACE]) {
                        wpisywane_slowo[strlen(wpisywane_slowo) - 1] = '\0';
                        printf("%s\n", wpisywane_slowo);
                        break;
                    }
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

            case ALLEGRO_EVENT_MOUSE_BUTTON_DOWN:
                if (event.mouse.button == 1 && wyswietlanie_drzewa) {
                    nacisniety_przycisk = sprawdz_nacisniecie_przycisku(event.mouse.x, event.mouse.y, przyciski);
                    if (nacisniety_przycisk != -1) {
                        wyswietlanie_drzewa = false;
                    }
                }
                break;

            case ALLEGRO_EVENT_DISPLAY_CLOSE:
                zakoncz_program = true;
                break;
        }

        if (zakoncz_program) {
            break;
        }

        if (redraw && al_is_event_queue_empty(queue))
        {
            al_clear_to_color(al_map_rgb(0, 0, 0));

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
            else {
                narysuj_wpisywanie(font, wpisywane_slowo);
            }

            al_flip_display();

            redraw = false;
        }
    }

    al_destroy_font(font);
    al_destroy_display(disp);
    al_destroy_timer(timer);
    al_destroy_event_queue(queue);

    return 0;
}
