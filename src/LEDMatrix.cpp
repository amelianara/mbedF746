/**
 * LED Matrix library for http://www.seeedstudio.com/depot/ultrathin-16x32-red-led-matrix-panel-p-1582.html
 * The LED Matrix panel has 32x16 pixels. Several panel can be combined together as a large screen.
 *
 * Coordinate & Connection (mbed -> panel 0 -> panel 1 -> ...)
 *   (0, 0)                                     (0, 0)
 *     +--------+--------+--------+               +--------+--------+
 *     |   5    |    3   |    1   |               |    1   |    0   |
 *     |        |        |        |               |        |        |<----- mbed
 *     +--------+--------+--------+               +--------+--------+
 *     |   4    |    2   |    0   |                              (64, 16)
 *     |        |        |        |<----- mbed
 *     +--------+--------+--------+
 *                             (96, 32)
 *  Copyright (c) 2013 Seeed Technology Inc.
 *  @auther     Yihui Xiong
 *  @date       Nov 8, 2013
 *  @license    Apache
 */

#include "LEDMatrix.h"
#include "mbed.h"

#if 0
#define ASSERT(e)   if (!(e)) { Serial.println(#e); while (1); }
#else
#define ASSERT(e)
#endif

LEDMatrix::LEDMatrix(PinName pinA, PinName pinB, PinName pinC, PinName pinD, PinName pinOE,
                     PinName pinR1, PinName pinR2, PinName pinB1, PinName pinB2, PinName pinG1, PinName pinG2,
                     PinName pinSTB, PinName pinCLK) :
    a(pinA), b(pinB), c(pinC), d(pinD), oe(pinOE), r1(pinR1), r2(pinR2), b1(pinB1), b2(pinB2), g1(pinG1), g2(pinG2), stb(pinSTB), clk(pinCLK)
{
    oe = 1;
    clk = 1;
    stb = 1;
    mask = 0xff;
    state = 0;
    bufferIndex = 0;
    flagSwap = 0;
}

void LEDMatrix::begin(uint8_t *displaybuf, uint16_t width, uint16_t height)
{
    this->displaybuf = displaybuf;
    memoBuf = displaybuf;
    drawBuf = memoBuf + width * height;
    this->width = width;
    this->height = height;

    state = 1;
}

void LEDMatrix::drawPoint(uint16_t x, uint16_t y, uint8_t pixel)
{
    if (x>=width) return;
    if (y>=height) return;
    drawBuf[x+ width * y] = pixel & 0x07;
}

void LEDMatrix::drawRect(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, uint8_t pixel)
{
    for (uint16_t x = x1; x < x2; x++) {
        for (uint16_t y = y1; y < y2; y++) {
            drawPoint(x, y, pixel);
        }
    }
}

int LEDMatrix::drawChar(uint16_t x, uint16_t y, char c, uint8_t pixel, Font *font)
{
    int i, j,k, col, max = 0;
    for (i=0; i<font->hauteur(); i++) {
        for (k=0; k<font->largeur(); k++) {
            col = font->octet(c, i, k);
            for (j=8; j>=0; j--) {
                if (col & 0x01 != 0) {
                    int dx = j+8*k;
                    if (dx > max) max = dx;
                    drawPoint(x + dx, y+i, pixel);
                }
                col = col >> 1;
            }
        }
    }
    if (c==' ') {
        return 5*font->largeur();
    } else {
        return max + font->largeur();
    }
}


void LEDMatrix::drawCharString (uint16_t x, uint16_t y, char *c, uint8_t pixel,Font *font)
{
    int i=0;
    while(c[i] != '\0') {
        int dx = drawChar(x,y,c[i],pixel,font);
        i++;
        x += dx;
    }
}


void LEDMatrix::drawImage(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, uint8_t *image)
{
    ASSERT(0 == ((x2 - x1) % 8));

    for (uint16_t x = x1; x < x2; x++) {
        for (uint16_t y = y1; y < y2; y++) {
            uint8_t *byte = image + x * 8 + y / 8;
            uint8_t  bit = 7 - (y % 8);
            uint8_t  pixel = (*byte >> bit) & 1;

            drawPoint(x, y, pixel);
        }
    }
}

void LEDMatrix::clear()
{
    uint8_t *ptr = drawBuf;
    for (uint16_t i = 0; i < (width * height); i++) {
        *ptr = 0x00;
        ptr++;
    }
}

void LEDMatrix::reverse()
{
    mask = ~mask;
}

uint8_t LEDMatrix::isReversed()
{
    return mask;
}

void LEDMatrix::scan()
{
    static uint8_t row = 0;

    if (!state) {
        return;
    }

    int debut1 = 0 + row * width ;
    int debut2 = 1024 + row * width ;

    for (uint8_t i = 0; i <64 ; i++) {
        clk = 0;

        r1 = displaybuf[debut1+i] & 0x01;
        r2 = displaybuf[debut2+i] & 0x01;
        g1 = (displaybuf[debut1+i] & 0x02) >> 1;
        g2 = (displaybuf[debut2+i] & 0x02) >> 1;
        b1 = (displaybuf[debut1+i] & 0x04) >> 2;
        b2 = (displaybuf[debut2+i] & 0x04) >> 2;

        /*r1=0;
        r2=0;
        g1=0;
        g2=0;
        b1=0;
        b2=0;*/
        clk = 1;
    }
/*
    debut1 = 0 + row * width;
    debut2 = 1024 + row * width;

    for (uint8_t i = 0; i <64 ; i++) {
        clk = 0;

        r1 = displaybuf[debut1+i] & 0x01;
        r2 = displaybuf[debut2+i] & 0x01;
        g1 = (displaybuf[debut1+i] & 0x02) >> 1;
        g2 = (displaybuf[debut2+i] & 0x02) >> 1;
        b1 = (displaybuf[debut1+i] & 0x04) >> 2;
        b2 = (displaybuf[debut2+i] & 0x04) >> 2;

        r1=1;
        r2=1;
        g1=0;
        g2=0;
        b1=0;
        b2=0;

        clk = 1;
    }
*/
    oe = 1;              // disable display

    // select row
    a = (row & 0x01);
    b = (row & 0x02);
    c = (row & 0x04);
    d = (row & 0x08);

    // latch data
    stb = 0;
    stb = 1;

    oe = 0;              // enable display

    row=(row + 1) & 0x0F;

    if (row==0) {
        if (flagSwap == 1) {
            if (bufferIndex==0) {
                bufferIndex = 1;
                displaybuf = memoBuf + width*height;
                drawBuf = memoBuf;
            } else {
                bufferIndex = 0;
                displaybuf = memoBuf;
                drawBuf = memoBuf + width*height;
            }
            flagSwap = 0;
        }
    }
}

void LEDMatrix::on()
{
    state = 1;
}

void LEDMatrix::off()
{
    state = 0;
    oe = 1;
}

void LEDMatrix::swap()
{
    flagSwap = 1;
}

int LEDMatrix::synchro()
{
    return !flagSwap;
}
