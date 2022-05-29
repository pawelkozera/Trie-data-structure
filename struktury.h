///
/// Struktury
///
#ifndef STRUKTURY_H_INCLUDED
#define STRUKTURY_H_INCLUDED

#define LITERY_ALFABETU 26

struct Trie {
    int jest_lisciem; // 1 gdy jest lisciem, 0 gdy nie jest
    int x, y;
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

#endif // STRUKTURY_H_INCLUDED
