#include "fonts.h"

#include "smallFont.h"
#include "grotesk.h"
#include "moyen.h"

Font *fonts[3];

Font::Font(int l, int h, int nb, uint8_t *d)
{
    larg = l;
    haut = h;
    nbCarac = nb;
    data = d;
}

uint8_t Font::octet(int carac, int i, int j)
{
    carac = carac - ' ';
    if (carac>nbCarac) return 0;
    return data[carac*larg*haut+i*larg+j];
}

void initFonts()
{
    fonts[0] = new Font(2, 16, 94, (uint8_t *)franklingothic_normal);    
    fonts[1] = new Font(2, 32, 94, (uint8_t *)Grotesk16x32);    
    fonts[2] = new Font(4, 64, 94, (uint8_t *)grotesk32x64);    
}
    