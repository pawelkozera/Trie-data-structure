#ifndef STRUKTURY_H_INCLUDED
#define STRUKTURY_H_INCLUDED

#define LITERY_ALFABETU 26

/// DRZEWO TRIE
/** Zmienne węzła jego pozycji x, y oraz tablica liter alfabetu. */
struct Trie {
    int jest_lisciem; // 1 gdy jest lisciem, 0 gdy nie jest
    int x, y;
    struct Trie* litery[LITERY_ALFABETU];
};

/// KAMERA
/** Pozycja kamery x, y. */
struct Kamera {
    int x, y;
};

/// PRZYCISK
/** Pozycja i wymiary przycisku, a także jego napis. */
struct Przycisk {
    int x, y, szerokosc, wysokosc;
    char napis[10];
};

/// KOMUNIKAT
/**  */
struct Komunikaty {
    char komunikat[50];
    char komunikat_czas[50];
};

#endif // STRUKTURY_H_INCLUDED
