#pragma once

// Usunąłem #include "status.h", ponieważ ten plik to właśnie status.h
#include "display.h"
#include <LittleFS.h>
#include "../data/monofonto_rg12pt7b.h"
#include "../data/monofonto_rg13pt7b.h"
#include "../data/monofonto_rg70pt7b.h"
#include "../../imgs/statusBody.h"
#include "../../imgs/minka0.h"
#include "../../imgs/minka1.h"
#include "../../imgs/minka2.h"
#include "../../imgs/minka3.h"
#include "../../imgs/minka4.h"
#include "special.h"

enum Screen { SCR_STN, SCR_RAD, SCR_DIFF, SCR_ZEG, SCR_UST, SCR_NUFN,SCR_GMF};

struct Setting {
    const char* name;
    int value;
    int min_val;
    int max_val;
    const char* unit;
};

class Status {
private:
    int cursor;
    LGFX_Sprite sprite[6];
    LGFX_Sprite gmfSprite[1];
    const char* label[5] = {"STN", "ZEG", "UST", "GMF"};
    bool stn_edit;
    int temp_hp;
    int ust_cursor;
    bool ust_edit;
    Screen current_selected;
    Screen action_pool;
    char playerName[9]; // 8 liter + znak końca stringa '\0'
    int name_char_idx;  // Indeks aktualnie edytowanej litery (0-7)
    static const int SETTINGS_COUNT = 12;
    Setting settings[SETTINGS_COUNT];

    // Prywatne metody rysujące
    void drawScreenSTN();
    void drawScreenRAD();
    void drawScreenZEG();
    void drawScreenDIFF();
    void drawScreenUST();
    void drawThemAll();
    void drawScreenGMF();
public:
    // Konstruktor
    Status();

    // Metody publiczne
    void loadSprites();
    void drawScreen();
    void changeCursor(int d);
    void statusSelect();
    int getCursor() const { return cursor; }
    const char* getPlayerName() const { return playerName; }
    void loadFromPreferences();
    unsigned long gmf_last_time;
    bool gmf_show_grid;
    void update();
};