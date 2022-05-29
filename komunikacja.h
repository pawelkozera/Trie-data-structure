///
/// Prototypy funkcji komunikacji z uzytkownikiem
///
#ifndef KOMUNIKACJA_H_INCLUDED
#define KOMUNIKACJA_H_INCLUDED

void czas_start();
double czas_stop();
void wybor_akcji(struct Trie **korzen, int nacisniety_przycisk, char wpisywane_slowo[], struct Komunikaty *komunikaty);
int sprawdz_nacisniecie_przycisku(int mysz_x, int mysz_y, struct Przycisk przyciski[]);
void sterowanie_kamera(struct Kamera *kamera, char znak);

#endif // KOMUNIKACJA_H_INCLUDED
