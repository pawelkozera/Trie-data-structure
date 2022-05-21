#ifndef DRZEWO_H_INCLUDED
#define DRZEWO_H_INCLUDED

struct Trie* stworz_nowe_drzewo_trie();
void wstaw_do_drzewa(struct Trie *korzen, char slowo[]);
int wyszukaj_z_drzewa(struct Trie *korzen, char *slowo);
void zamien_na_male_litery(char slowo[]);
bool nie_ma_dzieci(struct Trie *korzen);
struct Trie* usun_z_drzewa(struct Trie *korzen, char slowo[], int glebokosc);
struct Trie* usun_przed_wczytaniem(struct Trie *korzen);
bool odczytaj_z_pliku_i_wstaw_do_drzewa(struct Trie **korzen, char nazwa_pliku[]);
bool zapisz_do_pliku(struct Trie *korzen, char nazwa_pliku[]);
void zapisz_drzewo(struct Trie *korzen, char slowa[], int index, char nazwa_pliku[]);

#endif // DRZEWO_H_INCLUDED
