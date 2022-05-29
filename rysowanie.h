///
/// Prototypy funkcji rysowania
///
#ifndef RYSOWANIE_H_INCLUDED
#define RYSOWANIE_H_INCLUDED

int narysuj_drzewo(struct Trie *korzen, int szerokosc, int wysokosc, ALLEGRO_FONT* font, int glebokosc, struct Kamera kamera);
void narysuj_linie_drzewa(struct Trie *korzen);
void narysuj_menu(ALLEGRO_FONT* font, struct Przycisk przycisk);
void narysuj_komunikat(ALLEGRO_FONT*font, struct Komunikaty komunikaty, struct Przycisk przycisk_wczytaj);
void narysuj_wpisywanie(ALLEGRO_FONT* font, char wpisywane_slowo[]);

#endif // RYSOWANIE_H_INCLUDED
