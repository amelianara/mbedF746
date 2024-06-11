#ifndef __FONTS_H__
#define __FONTS_H__

#include "mbed.h"

class Font {
private:
    int larg, haut;
    int nbCarac;
    uint8_t *data;
public:
    Font(int l, int h, int nb, uint8_t *d);
    int largeur() { return larg; }
    int hauteur() { return haut; }
    uint8_t octet(int carac, int i, int j);
};

void initFonts();

extern Font *fonts[];

#endif
