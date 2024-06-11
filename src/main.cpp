#include <mbed.h>
#include <threadLvgl.h>

#include "demos/lv_demos.h"
#include <cstdio>
////////////// INCLUSION DES LIBRAIRIES (L1-L18)//////////////
#include "mbed.h"
#include "LEDMatrix.h"
#include "smallFont.h"
/////////////////FIN INCLUSION DES LIBRAIRIES/////////////////
#include "threadLvgl.h"
#include "Drivers/BSP/STM32746G-Discovery/stm32746g_discovery_lcd.h"
#include "Drivers/BSP/STM32746G-Discovery/stm32746g_discovery_ts.h"
#include "misc/lv_color.h"
#include "draw/stm32_dma2d/lv_gpu_stm32_dma2d.h"

//////DEFINITION LARGEUR ET LONGUEUR DE L ECRAN LED //////
#define WIDTH 64
#define HEIGHT 32
//////// FIN DEFINITION LARGEUR ET LONGUEUR DE L ECRAN LED ////////
ThreadLvgl threadLvgl(30);

// stb <=> latch
// LEDMatrix    (a, b,  c, d,  oe_1, r1, r2, b1, b2, g1, g2, stb_1, clk_1);
LEDMatrix matrix(A1, A2, A0, A3, D4, D9, D7, D8, D6, D10, D11, A4, D5);
Ticker scanner;
lv_indev_drv_t *indev_drv;
lv_indev_data_t *data;
lv_indev_t *indevTouchpad;

int couleur = 0;
int fond=0;
// Display Buffer
uint8_t displaybuf[2][WIDTH * HEIGHT] = {
    0x00};

/// FONCTION DE SCAN DE L'ECRAN LED//////
void scan()
{
    matrix.scan();
}
/// FIN FONCTION DE SCAN DE L'ECRAN LED//////

void touchpadRead(lv_indev_drv_t *indev_drv, lv_indev_data_t *data)
{
    static lv_coord_t last_x = 0;
    static lv_coord_t last_y = 0;

    /*Save the pressed coordinates and the state*/
    TS_StateTypeDef TS_State;

    BSP_TS_GetState(&TS_State);
    if (TS_State.touchDetected)
    {
        last_x = TS_State.touchX[0];
        last_y = TS_State.touchY[0];
        data->state = LV_INDEV_STATE_PR;
    }

    else
    {
        data->state = LV_INDEV_STATE_REL;
    }

    data->point.x = last_x;
    data->point.y = last_y;

    if (TS_State.touchDetected && fond==0)
    {
        matrix.drawPoint((data->point.x) / 6, (data->point.y) / 6, couleur);
        matrix.swap();
    }

    if (TS_State.touchDetected && fond==1)
    {
        matrix.drawRect(0, 0, 64, 64, couleur); // Allumage d'une zone rectangulaire sur l'écran LED
        matrix.swap();
    }
}

void touchpadInit()
{
    static lv_indev_drv_t indev_drv;

    /*------------------
     * Touchpad
     * -----------------*/

    /*Initialize your touchpad if you have*/
    BSP_TS_Init(480, 272);

    /*Register a touchpad input device*/
    lv_indev_drv_init(&indev_drv);
    indev_drv.type = LV_INDEV_TYPE_POINTER;
    indev_drv.read_cb = touchpadRead;
    indevTouchpad = lv_indev_drv_register(&indev_drv);
}

void btn_event_clean_matrice(lv_event_t *e)
{
    lv_event_code_t code = lv_event_get_code(e);
    lv_obj_t *btn = lv_event_get_target(e);

    matrix.begin((uint8_t *)displaybuf, WIDTH, HEIGHT); // Sauv en mémoire la taille de la matrice LED

    scanner.attach(scan, 0.0003);

    if (code == LV_EVENT_CLICKED)
    {
        matrix.drawRect(0, 0, 64, 64, 0); // Allumage d'une zone rectangulaire sur l'écran LED
        matrix.swap();
        while (!matrix.synchro());
        ThisThread::sleep_for(10ms);
        matrix.clear();
    }
}

void btn_event_Start(lv_event_t *e)
{
    lv_event_code_t code = lv_event_get_code(e);
    lv_obj_t *btn = lv_event_get_target(e);

    matrix.begin((uint8_t *)displaybuf, WIDTH, HEIGHT); // Sauv en mémoire la taille de la matrice LED

    scanner.attach(scan, 0.0003);

    if (code == LV_EVENT_CLICKED)
    {
        for (int i = 200; i > -400; i -= 4)
        {
            matrix.clear();
            matrix.drawCharString(i, 0, "Matrice LED BUT 3 ESE INTERFACAGE", 5, fonts[0]); // Affichage de texte sur la matrice
            matrix.swap();
            ThisThread::sleep_for(50ms);
            while (!matrix.synchro());
        }
        matrix.clear();
    }
}

void btn_event_Change_Bleu(lv_event_t *e)
{
    lv_event_code_t code = lv_event_get_code(e);
    lv_obj_t *btn = lv_event_get_target(e);

    if (code == LV_EVENT_CLICKED)
    {
        couleur = 4;
    }
}

void btn_event_Change_Jaune(lv_event_t *e)
{
    lv_event_code_t code = lv_event_get_code(e);
    lv_obj_t *btn = lv_event_get_target(e);

    if (code == LV_EVENT_CLICKED)
    {
        couleur = 3;
    }
}

void btn_event_Change_Rouge(lv_event_t *e)
{
    lv_event_code_t code = lv_event_get_code(e);
    lv_obj_t *btn = lv_event_get_target(e);

    if (code == LV_EVENT_CLICKED)
    {
        couleur = 1;
    }
}

void btn_event_Change_Blanc(lv_event_t *e)
{
    lv_event_code_t code = lv_event_get_code(e);
    lv_obj_t *btn = lv_event_get_target(e);

    if (code == LV_EVENT_CLICKED)
    {
        couleur = 7;
    }
}

void btn_event_Change_Vert(lv_event_t *e)
{
    lv_event_code_t code = lv_event_get_code(e);
    lv_obj_t *btn = lv_event_get_target(e);

    if (code == LV_EVENT_CLICKED)
    {
        couleur = 2;
    }
}

void btn_event_Change_Noir(lv_event_t *e)
{
    lv_event_code_t code = lv_event_get_code(e);
    lv_obj_t *btn = lv_event_get_target(e);

    if (code == LV_EVENT_CLICKED)
    {
        couleur = 0;
    }
}

void btn_event_Change_Rose(lv_event_t *e)
{
    lv_event_code_t code = lv_event_get_code(e);
    lv_obj_t *btn = lv_event_get_target(e);

    if (code == LV_EVENT_CLICKED)
    {
        couleur = 5;
    }
}

void btn_event_Change_Turquoise(lv_event_t *e)
{
    lv_event_code_t code = lv_event_get_code(e);
    lv_obj_t *btn = lv_event_get_target(e);

    if (code == LV_EVENT_CLICKED)
    {
        couleur = 6;
    }
}

void btn_event_Font(lv_event_t *e)
{
    lv_event_code_t code = lv_event_get_code(e);
    lv_obj_t *btn = lv_event_get_target(e);

    if (code == LV_EVENT_CLICKED)
    {
        fond = 1;
    }
}

void btn_event_Dessin(lv_event_t *e)
{
    lv_event_code_t code = lv_event_get_code(e);
    lv_obj_t *btn = lv_event_get_target(e);

    if (code == LV_EVENT_CLICKED)
    {
        fond = 0;
    }
}

///////////////////////// DEBUT DU MAIN ////////////////////////////////////////
int main()
{
    initFonts();

    lv_obj_t *text;
    lv_obj_t *btnStop;
    lv_obj_t *btnSart;
    lv_obj_t *btnBleu;
    lv_obj_t *btnJaune;
    lv_obj_t *btnRouge;
    lv_obj_t *btnBlanc;
    lv_obj_t *btnVert;
    lv_obj_t *btnNoir;
    lv_obj_t *btnRose;
    lv_obj_t *btnTurquoise;
    lv_obj_t *btnFond;
    lv_obj_t *btnLigne;

    int largeur_bouton = 70;

    threadLvgl.lock();
    text = lv_label_create(lv_scr_act());
    lv_label_set_text(text, "Couleurs :");
    lv_obj_set_x(text, 5);
    lv_obj_set_y(text, 200);

    btnStop = lv_btn_create(lv_scr_act());
    lv_obj_set_size(btnStop, largeur_bouton, 100);
    lv_obj_set_x(btnStop, 400);
    lv_obj_set_y(btnStop, 10);

    lv_obj_t *label = lv_label_create(btnStop);
    lv_label_set_text(label, "Effacer");
    lv_obj_center(label);

    btnSart = lv_btn_create(lv_scr_act());
    lv_obj_set_size(btnSart, largeur_bouton, 100);
    lv_obj_set_x(btnSart, 400);
    lv_obj_set_y(btnSart, 120);

    lv_obj_t *label2 = lv_label_create(btnSart);
    lv_label_set_text(label2, "Sart");
    lv_obj_center(label2);

    btnBleu = lv_btn_create(lv_scr_act());
    lv_obj_set_size(btnBleu, largeur_bouton, 30);
    lv_obj_set_x(btnBleu, (largeur_bouton + 5) * 2 + 5);
    lv_obj_set_y(btnBleu, 230);

    lv_obj_t *label3 = lv_label_create(btnBleu);
    lv_label_set_text(label3, "Bleu");
    lv_obj_center(label3);

    btnJaune = lv_btn_create(lv_scr_act());
    lv_obj_set_size(btnJaune, largeur_bouton, 30);
    lv_obj_set_x(btnJaune, (largeur_bouton + 5) * 3 + 5);
    lv_obj_set_y(btnJaune, 230);

    lv_obj_t *label4 = lv_label_create(btnJaune);
    lv_label_set_text(label4, "Jaune");
    lv_obj_center(label4);

    btnRouge = lv_btn_create(lv_scr_act());
    lv_obj_set_size(btnRouge, largeur_bouton, 30);
    lv_obj_set_x(btnRouge, (largeur_bouton + 5) * 4 + 5);
    lv_obj_set_y(btnRouge, 230);

    lv_obj_t *label5 = lv_label_create(btnRouge);
    lv_label_set_text(label5, "Rouge");
    lv_obj_center(label5);

    btnBlanc = lv_btn_create(lv_scr_act());
    lv_obj_set_size(btnBlanc, largeur_bouton, 30);
    lv_obj_set_x(btnBlanc, 5);
    lv_obj_set_y(btnBlanc, 230);

    lv_obj_t *label6 = lv_label_create(btnBlanc);
    lv_label_set_text(label6, "Blanc");
    lv_obj_center(label6);

    btnVert = lv_btn_create(lv_scr_act());
    lv_obj_set_size(btnVert, largeur_bouton, 30);
    lv_obj_set_x(btnVert, (largeur_bouton + 5) * 5 + 5);
    lv_obj_set_y(btnVert, 230);

    lv_obj_t *label7 = lv_label_create(btnVert);
    lv_label_set_text(label7, "Vert");
    lv_obj_center(label7);

    btnNoir = lv_btn_create(lv_scr_act());
    lv_obj_set_size(btnNoir, largeur_bouton, 30);
    lv_obj_set_x(btnNoir, largeur_bouton + 10);
    lv_obj_set_y(btnNoir, 230);

    lv_obj_t *label8 = lv_label_create(btnNoir);
    lv_label_set_text(label8, "Noir");
    lv_obj_center(label8);

    /*btnRose = lv_btn_create(lv_scr_act());
    lv_obj_set_size(btnRose, largeur_bouton, 30);
    lv_obj_set_x(btnRose, 5);
    lv_obj_set_y(btnRose, 270);

    lv_obj_t *label9 = lv_label_create(btnRose);
    lv_label_set_text(label9, "Rose");
    lv_obj_center(label9);

    btnTurquoise = lv_btn_create(lv_scr_act());
    lv_obj_set_size(btnTurquoise, largeur_bouton + 10, 30);
    lv_obj_set_x(btnTurquoise, largeur_bouton + 10);
    lv_obj_set_y(btnTurquoise, 270);

    lv_obj_t *label10 = lv_label_create(btnTurquoise);
    lv_label_set_text(label10, "Turquoise");
    lv_obj_center(label10);*/

    btnFond = lv_btn_create(lv_scr_act());
    lv_obj_set_size(btnFond, largeur_bouton, 30);
    lv_obj_set_x(btnFond, largeur_bouton*2);
    lv_obj_set_y(btnFond, 195);

    lv_obj_t *label11 = lv_label_create(btnFond);
    lv_label_set_text(label11, "Fond");
    lv_obj_center(label11);

    btnLigne = lv_btn_create(lv_scr_act());
    lv_obj_set_size(btnLigne, largeur_bouton, 30);
    lv_obj_set_x(btnLigne, largeur_bouton*3 +5);
    lv_obj_set_y(btnLigne, 195);

    lv_obj_t *label12 = lv_label_create(btnLigne);
    lv_label_set_text(label12, "Dessin");
    lv_obj_center(label12);


    lv_obj_add_event_cb(btnStop, btn_event_clean_matrice, LV_EVENT_ALL, NULL);
    lv_obj_add_event_cb(btnSart, btn_event_Start, LV_EVENT_ALL, NULL);
    lv_obj_add_event_cb(btnBleu, btn_event_Change_Bleu, LV_EVENT_ALL, NULL);
    lv_obj_add_event_cb(btnJaune, btn_event_Change_Jaune, LV_EVENT_ALL, NULL);
    lv_obj_add_event_cb(btnRouge, btn_event_Change_Rouge, LV_EVENT_ALL, NULL);
    lv_obj_add_event_cb(btnBlanc, btn_event_Change_Blanc, LV_EVENT_ALL, NULL);
    lv_obj_add_event_cb(btnVert, btn_event_Change_Vert, LV_EVENT_ALL, NULL);
    lv_obj_add_event_cb(btnNoir, btn_event_Change_Noir, LV_EVENT_ALL, NULL);
    //lv_obj_add_event_cb(btnRose, btn_event_Change_Rose, LV_EVENT_ALL, NULL);
    //lv_obj_add_event_cb(btnTurquoise, btn_event_Change_Turquoise, LV_EVENT_ALL, NULL);
    lv_obj_add_event_cb(btnFond, btn_event_Font, LV_EVENT_ALL, NULL);
    lv_obj_add_event_cb(btnLigne, btn_event_Dessin, LV_EVENT_ALL, NULL);


    lv_obj_t *rect = lv_obj_create(lv_scr_act());
    lv_obj_set_size(rect, 390, 192);
    lv_obj_align(rect, LV_ALIGN_TOP_LEFT, 0, 0);

    threadLvgl.unlock();

    matrix.begin((uint8_t *)displaybuf, WIDTH, HEIGHT); // Sauv en mémoire la taille de la matrice LED

    scanner.attach(scan, 0.5ms); // Gère l'affiche sur la matrice LED
                                  // Un scan trop lent (>0.0009) provoque un effet de scintillement
                                  // Un scan trop rapide (<0.0001) provoque un effet d'allumage des LEDs qui sont sur la même ligne

    matrix.clear(); // Fonction de nettoyage de la matrice, toutes les LEDs s'éteignent

    touchpadInit();

    while (1)
    {
        // put your main code here, to run repeatedly:
        ThisThread::sleep_for(10ms);
    }
    ///////////// BOUCLE INFINI //////////////
    // while (1);
} // FIN MAIN
///////////////////////// FIN DU MAIN ////////////////////////////////////////
