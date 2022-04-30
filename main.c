#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <allegro5/allegro5.h>
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_primitives.h>
#include <string.h>

#define LITERY_ALFABETU 26
#define SZEROKOSC_EKRANU 800
#define WYSOKOSC_EKRANU 600
#define SREDNICA_OKREGU 30
#define KEY_SEEN     1
#define KEY_RELEASED 2
#define ILOSC_PRZYCISKOW 5
#define DLUGOSC_WPISANEGO_SLOWA 50

/*
    Dokumentacja Allegro5
    https://github.com/liballeg/allegro_wiki/wiki/Allegro-Vivace%3A-Basic-game-structure
*/

struct Trie {
    int jest_lisciem; // 1 gdy jest lisciem, 0 gdy nie jest
    struct Trie* litery[LITERY_ALFABETU];
};

struct Kamera {
    int x, y;
};

struct Przycisk {
    int x, y, szerokosc, wysokosc;
    char napis[10];
};

struct Komunikaty {
    char komunikat[50];
};

//Prototypy funkcji
struct Trie* stworz_nowe_drzewo_trie();
void wstaw_do_drzewa(struct Trie *korzen, char slowo[]);
int wyszukaj_z_drzewa(struct Trie *korzen, char *slowo);
void zamien_na_male_litery(char slowo[]);
struct Trie* usun_z_drzewa(struct Trie *korzen, char slowo[], int glebokosc);
bool nie_ma_dzieci(struct Trie *korzen);
bool odczytaj_z_pliku_i_wstaw_do_drzewa(struct Trie *korzen, char nazwa_pliku[]);
bool zapisz_do_pliku(struct Trie *korzen, char nazwa_pliku[]);
void zapisz_drzewo(struct Trie *korzen, char slowa[], int index, char nazwa_pliku[]);
void wybor_akcji(struct Trie *korzen, int nacisniety_przycisk, char wpisywane_slowo[], struct Komunikaty *komunikaty);
int narysuj_drzewo(struct Trie *korzen, int szerokosc, int wysokosc, ALLEGRO_FONT* font, int glebokosc, struct Kamera kamera);
int ilosc_galezi(struct Trie *korzen);
void narysuj_menu(ALLEGRO_FONT* font, struct Przycisk przycisk);
void narysuj_komunikat(ALLEGRO_FONT*font, struct Komunikaty komunikaty, struct Przycisk przycisk_wczytaj);
void narysuj_wpisywanie(ALLEGRO_FONT* font, char wpisywane_slowo[]);
int sprawdz_nacisniecie_przycisku(int mysz_x, int mysz_y, struct Przycisk przyciski[]);

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

    // testowanie działania
    char z[] = "dog";
    char x[] = "kot";
    char c[] = "koala";

    wstaw_do_drzewa(korzen, z);
    wstaw_do_drzewa(korzen, x);
    wstaw_do_drzewa(korzen, c);

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
                    kamera.y += 10;
                if(key[ALLEGRO_KEY_DOWN])
                    kamera.y -= 10;
                if(key[ALLEGRO_KEY_LEFT])
                    kamera.x += 10;
                if(key[ALLEGRO_KEY_RIGHT])
                    kamera.x -= 10;
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
                            wybor_akcji(korzen, nacisniety_przycisk, wpisywane_slowo, &komunikaty);
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

bool nie_ma_dzieci(struct Trie *korzen) {
    for (int i = 0; i < LITERY_ALFABETU; i++) {
        if (korzen->litery[i]) {
            return false;
        }
    }
    return true;
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

bool odczytaj_z_pliku_i_wstaw_do_drzewa(struct Trie *korzen, char nazwa_pliku[]) {
    FILE *plik;
    plik = fopen(nazwa_pliku, "r");

    if (plik == NULL) {
        exit(1);
    }

    char linia[40];
    while (fgets(linia, sizeof(linia), plik)) {
        linia[strcspn(linia, "\n")] = 0; //usuwa znak nowej lini z ciagu znakow
        wstaw_do_drzewa(korzen, linia);
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

void wybor_akcji(struct Trie *korzen, int nacisniety_przycisk, char wpisywane_slowo[], struct Komunikaty *komunikaty) {
    switch(nacisniety_przycisk)
    {
        case 0:
            {
                wstaw_do_drzewa(korzen, wpisywane_slowo);
                char bufor[50] = "Dodano slowo: ";
                strncat(bufor, wpisywane_slowo, strlen(wpisywane_slowo));
                strcpy(komunikaty->komunikat, bufor);
                break;
            }
        case 1:
            {
                usun_z_drzewa(korzen, wpisywane_slowo, 0);
                char bufor[50] = "Usunieto slowo: ";
                strncat(bufor, wpisywane_slowo, strlen(wpisywane_slowo));
                strcpy(komunikaty->komunikat, bufor);
                break;
            }
        case 2:
            {
                if (wyszukaj_z_drzewa(korzen, wpisywane_slowo)) {
                    char bufor[50] = "Znaleziono slowo: ";
                    strncat(bufor, wpisywane_slowo, strlen(wpisywane_slowo));
                    strcpy(komunikaty->komunikat, bufor);
                }
                break;
            }
        case 3:
            {
                if(zapisz_do_pliku(korzen, wpisywane_slowo) == true) {
                    char bufor[50] = "Zapisano do: ";
                    strncat(bufor, wpisywane_slowo, strlen(wpisywane_slowo));
                    strcpy(komunikaty->komunikat, bufor);
                }
                else
                    puts("Operacja zakonczona niepowodzeniem");
                break;
            }
        case 4:
            {
                if(odczytaj_z_pliku_i_wstaw_do_drzewa(korzen, wpisywane_slowo) == true) {// nie dziala, trzeba wyczyscisc drzewo przed dodaniem elementow z pliku
                    char bufor[50] = "Wczytano z: ";
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

int narysuj_drzewo(struct Trie *korzen, int szerokosc, int wysokosc, ALLEGRO_FONT* font, int glebokosc, struct Kamera kamera) {
    if (korzen != NULL) {
        int ilosc_g = ilosc_galezi(korzen);
        int ilosc_g_bufor = ilosc_g;
        char litera[] = "a";
        int szerokosci[LITERY_ALFABETU] = {0};
        int n_szerokosci = 0;

        for (int i = 0; i < LITERY_ALFABETU; i++) {
            if (korzen->litery[i] != NULL) {
                szerokosc = narysuj_drzewo(korzen->litery[i], szerokosc, wysokosc + 100, font, glebokosc+1, kamera);
                litera[0] = 'a' + i;

                al_draw_circle(szerokosc + kamera.x, wysokosc + kamera.y, SREDNICA_OKREGU, al_map_rgb(0, 255, 0), 2);
                al_draw_textf(font, al_map_rgb(255, 255, 255), szerokosc + kamera.x, wysokosc + kamera.y, 0, litera);

                if (glebokosc != 0) {
                    al_draw_line(szerokosc + kamera.x, wysokosc - SREDNICA_OKREGU + kamera.y, szerokosc + (SREDNICA_OKREGU + 40)*ilosc_g + kamera.x, wysokosc - 100 + SREDNICA_OKREGU + kamera.y, al_map_rgb_f(1, 0, 0), 1);
                }

                if (glebokosc == 0) {
                    szerokosci[n_szerokosci] = szerokosc;
                    n_szerokosci++;
                }

                szerokosc += SREDNICA_OKREGU + 40;
                ilosc_g--;
            }
        }

        if (glebokosc == 0) {
            al_draw_circle(szerokosc/2 + kamera.x, SREDNICA_OKREGU + kamera.y, SREDNICA_OKREGU, al_map_rgb(0, 255, 0), 2);
            al_draw_textf(font, al_map_rgb(255, 255, 255), szerokosc/2 + kamera.x, SREDNICA_OKREGU + kamera.y, 0, "#");
            for (int i = 0; i < ilosc_g_bufor; i++) {
                al_draw_line(szerokosc/2 + kamera.x, SREDNICA_OKREGU*2 + kamera.y, szerokosci[i] + kamera.x, wysokosc-SREDNICA_OKREGU + kamera.y, al_map_rgb_f(1, 0, 0), 1);
            }
        }
    }

    return szerokosc;
}

int ilosc_galezi(struct Trie *korzen) {
    int ilosc = 0;
    for (int i = 0; i < LITERY_ALFABETU; i++) {
        if (korzen->litery[i] != NULL) {
            ilosc++;
        }
    }

    return ilosc;
}

void narysuj_menu(ALLEGRO_FONT* font, struct Przycisk przycisk) {
    al_draw_filled_rectangle(przycisk.x, przycisk.y, przycisk.x + przycisk.szerokosc, przycisk.y + przycisk.wysokosc, al_map_rgb(112, 110, 104));
    al_draw_textf(font, al_map_rgb(255, 255, 255), przycisk.x + 10, przycisk.y + (przycisk.wysokosc/2), 0, przycisk.napis);
}

void narysuj_komunikat(ALLEGRO_FONT*font, struct Komunikaty komunikaty, struct Przycisk przycisk_wczytaj) {
    al_draw_textf(font, al_map_rgb(255, 255, 255), przycisk_wczytaj.x + przycisk_wczytaj.szerokosc + 20, przycisk_wczytaj.y + (przycisk_wczytaj.wysokosc/2), 0, komunikaty.komunikat);
}

void narysuj_wpisywanie(ALLEGRO_FONT* font, char wpisywane_slowo[]) {
    al_draw_textf(font, al_map_rgb(255, 255, 255), SZEROKOSC_EKRANU/3, WYSOKOSC_EKRANU/3, 0, "Wpisz slowo");
    al_draw_textf(font, al_map_rgb(255, 255, 255), SZEROKOSC_EKRANU/3, WYSOKOSC_EKRANU/2, 0, wpisywane_slowo);
    al_draw_textf(font, al_map_rgb(255, 255, 255), SZEROKOSC_EKRANU/3, WYSOKOSC_EKRANU/1.5, 0, "Zaakceptuj enterem");
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
