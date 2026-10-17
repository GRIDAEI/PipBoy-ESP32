#include "status.h"
#include "../screen.h"
#include "../topbar.h"
#include "../../imgs/GRiD_Logo_White.h"
#include <time.h>
#include <Preferences.h> // 
Status::Status() {
    cursor = 0;
    ust_cursor = 0;
    ust_edit = false;
    current_selected = SCR_STN;
    action_pool = SCR_NUFN;
    stn_edit = false;
    temp_hp = player.hp;
    gmf_last_time = 0;
    gmf_show_grid = false;
    strcpy(playerName, "VAULTBOY"); // Domyślne imię (max 8 znaków)
    name_char_idx = 0;
    
    // Inicjalizacja tablicy ustawień (zmieniamy rozmiar na 6)
    // Inicjalizacja tablicy ustawień
// Inicjalizacja tablicy ustawień
    settings[0] = { "Wroc",       1, 0,   1, "BACK" };
    settings[1] = { "MOTYW",      0, 0,   2, "THEM" };
    settings[2] = { "IMIE",       0, 0,   0, ""     };
    settings[3] = { "LEVEL",      1, 1, 100, ""     };
    settings[4] = { "ZDROWIE",    player.hp, 1, 500, ""     };
    settings[5] = { "AP",         1, 1,  99, ""     };
    settings[6] = { "JASNOSC",   80, 0, 100, "%"    };
    settings[7] = { "GLOSNOSC",  50, 0, 100, "%"    };
    settings[8] = { "KONTRAST",  70, 0, 100, "%"    };
    settings[9] = { "CZAS SYNC",  1, 0,   1, "BOOL" };
    settings[10]= { "GODZINA",   12, 0,  23, " H"   };
    settings[11]= { "MINUTA",     0, 0,  59, " M"   };
    

}

void Status::loadFromPreferences() {
    Preferences prefs;
    prefs.begin("pipboy", false); // Tryb odczytu i zapisu
    
    // Wczytaj HP
    if (prefs.isKey("player_hp")) {
        player.hp = prefs.getInt("player_hp");
    } else {
        prefs.putInt("player_hp", player.hp); // Zapisz domyślne, jeśli brak
    }
    temp_hp = player.hp;
    settings[4].value = player.hp;
    
    // Wczytaj Imię
    if (prefs.isKey("playerName")) {
        String loadedName = prefs.getString("playerName");
        strncpy(playerName, loadedName.c_str(), 8);
        playerName[8] = '\0';
    } else {
        prefs.putString("playerName", playerName);
    }
    
    // Wczytaj ustawienia liczbowe (pomijamy indeks 0, to "Wroc", indeks 2 to imię string)
    for (int i = 1; i < SETTINGS_COUNT; i++) {
        if (i == 2) continue; // Zabezpieczenie (imię to string)
        
        char klucz[15];
        snprintf(klucz, sizeof(klucz), "UST_%d", i);
        if (prefs.isKey(klucz)) {
            settings[i].value = prefs.getInt(klucz);
        } else {
            prefs.putInt(klucz, settings[i].value);
        }
    }
    
    prefs.end();
    
    // Ustawienie ważnych wartości po wczytaniu
    player.level = settings[3].value;
    
    setGlobalThemeColors(settings[1].value);
}
#include <time.h>
#include <sys/time.h>
void restoreTimeFromMemory() {
    Preferences prefs;
    prefs.begin("zegar", true); // true = tryb tylko do odczytu (szybszy)
    unsigned long saved_time = prefs.getULong("zapisany_czas", 0);
    prefs.end();

    if (saved_time > 0) {
        struct timeval tv;
        tv.tv_sec = saved_time;
        tv.tv_usec = 0;
        settimeofday(&tv, NULL);
        Serial.println("Czas wczytany pomyslnie z pamieci flash!");
    } else {
        Serial.println("Brak zapisanego czasu, ustawiam domyslny.");
    }
}

void Status::loadSprites() {
    uint32_t currentThemeColor = COLOR_GREEN; // Tu wpadnie kolor z ustawień

    sprite[0].deleteSprite(); 
    
    sprite[0].setColorDepth(16);
    sprite[0].setPsram(true); 
    loadTintedSprite(sprite[0], statusBody, statusBody_width, statusBody_height, currentThemeColor, COLOR_BG);
    sprite[0].setPivot(statusBody_width / 2, statusBody_height / 2);

    int width = 60;
    int height = 60;
    const uint16_t* b[] = { minka0, minka0, minka1, minka2, minka3, minka4 };
    
    for(int i = 1; i < 6; i++) {
        sprite[i].deleteSprite(); // Zwalnia pamięć dla każdej minki
        
        sprite[i].setColorDepth(16);
        sprite[i].setPsram(true); 
        loadTintedSprite(sprite[i], b[i], width, height, currentThemeColor, COLOR_BG);
        sprite[i].setPivot(width / 2, height / 2);
    }
    
    restoreTimeFromMemory();
}
void drawBar(int x,int y, int weight){
    if (weight < 0) weight = 0;
    if (weight > 100) weight = 100;
    tft.fillRect(x,y,70,16, COLOR_BG);
    tft.fillRect(x,y,70,16, COLOR_GREEN);
    tft.fillRect(x+1,y+1,67,14, COLOR_BG);
    tft.fillRect(x+2,y+2,64*weight/100,12, COLOR_GREEN);
}
void Status::drawThemAll(){
    int hp = stn_edit ? temp_hp : player.hp;
    int weight = hp * 100 / player.maxHp;
    drawBar(205,50,weight);
    drawBar(100,100,weight);
    drawBar(310,100,weight);    
    drawBar(110,210,weight);    
    drawBar(300,210,weight);    
}
void Status::changeCursor(int d) {
    Serial.println(current_selected);
    if (action_pool == SCR_NUFN) {
        cursor += d;
        
        if (cursor < 0) cursor = 0;
        if (cursor > 3) cursor = 3;

        if (strcmp(label[cursor], "STN") == 0) {
            current_selected = SCR_STN;
        } else if (strcmp(label[cursor], "RAD") == 0) {
            current_selected = SCR_RAD;
        } else if (strcmp(label[cursor], "ZEG") == 0) {
            current_selected = SCR_ZEG;
        } else if (strcmp(label[cursor], "UST") == 0) {
            current_selected = SCR_UST;
        }else if (strcmp(label[cursor], "GMF") == 0) {
            current_selected = SCR_GMF;
        }
        Serial.print("Wybralem: ");
        Serial.println(label[cursor]);
        drawScreen();
    }else if (action_pool == SCR_STN) {
        if (stn_edit) {
            temp_hp += (d * 5); // Zmieniaj HP o 5 punktów za każdym skokiem
            
            if (temp_hp < 0) temp_hp = 0;
            if (temp_hp > player.maxHp) temp_hp = player.maxHp;
            
            int weight = temp_hp * 100 / player.maxHp;
            int picid = 0;
            switch (weight) {
                case 85 ... 100: picid = 1; break;
                case 65 ... 84:  picid = 2; break;
                case 35 ... 64:  picid = 3; break;
                case 15 ... 34:  picid = 4; break;
                case 0 ... 14:   picid = 5; break;
            }
            sprite[picid].pushRotateZoom(&tft, 240, 108, 0, 0.8, 0.8, COLOR_BG);
            drawThemAll(); // Rysuj ponownie, by zaktualizować paski
        }
    }
    
    else if (action_pool == SCR_UST) {
        if (!ust_edit) {
            ust_cursor += d;
            // Pamiętaj by zaktualizować limit na SETTINGS_COUNT (czyli 6)
            if (ust_cursor < 0) ust_cursor = 0;
            if (ust_cursor >= SETTINGS_COUNT) ust_cursor = SETTINGS_COUNT - 1;
        } else {
            if (ust_cursor == 2) { 
                char& c = playerName[name_char_idx];
                if (d > 0) {
                    if (c >= 'A' && c < 'Z') c++;
                    else if (c == 'Z') c = ' '; // Po Z jest spacja
                    else if (c == ' ') c = 'A'; // Po spacji znowu A
                    else c = 'A'; // Zabezpieczenie
                } else if (d < 0) {
                    if (c > 'A' && c <= 'Z') c--;
                    else if (c == 'A') c = ' '; // Przed A jest spacja
                    else if (c == ' ') c = 'Z'; // Przed spacją jest Z
                    else c = 'Z'; // Zabezpieczenie
                }
            } else {
                int step = 1; 
                if (strcmp(settings[ust_cursor].unit, "%") == 0) step = 5;
                settings[ust_cursor].value += (d * step);

                if (settings[ust_cursor].value < settings[ust_cursor].min_val) settings[ust_cursor].value = settings[ust_cursor].min_val;
                if (settings[ust_cursor].value > settings[ust_cursor].max_val) settings[ust_cursor].value = settings[ust_cursor].max_val;
                

            }
        }
        drawScreen();
    }
}

void Status::drawScreen() {
    tft.fillRect(0, 50, 480, 250, COLOR_BG);
    Serial.println(current_selected);
    if (current_selected == SCR_STN) {
        drawScreenSTN();
    } else if (current_selected == SCR_RAD) {
        drawScreenRAD();
    } else if (current_selected == SCR_ZEG) {
        drawScreenZEG();
    } else if (current_selected == SCR_UST) {
        drawScreenUST();
    }else if (current_selected == SCR_GMF) {
        drawScreenGMF();
    }

    tft.setFont(&monofonto_rg9pt7b);
    tft.setTextColor(COLOR_GREEN);
    tft.setTextDatum(ML_DATUM);
    int y = 90;
    int n = 0;
    
    for (const char* i : label) {
        if (n == cursor) {
            tft.fillRect(20, y - 17, 46, 30, COLOR_GREEN);
            tft.fillRect(23, y - 14, 40, 24, COLOR_BG);
        }
        tft.drawString(i, 30, y);
        y += 40;
        n++;
    }
    
    tft.unloadFont();
}

void Status::drawScreenSTN() {
    // 1. Najpierw czytamy ile ma zdrowia
    if (!stn_edit) {
        Preferences prefs;
        prefs.begin("pipboy", true);
        if (prefs.isKey("player_hp")) {
            player.hp = prefs.getInt("player_hp");
        }
        prefs.end();
        temp_hp = player.hp;
        settings[4].value = player.hp;
    }

    int hp_do_wyswietlenia = stn_edit ? temp_hp : player.hp;
    int weight = hp_do_wyswietlenia * 100 / player.maxHp;

    sprite[0].pushRotateZoom(&tft, 240, 170, 0, 0.8, 0.8); 
    
    int picid = 0;
    switch (weight) {
        case 85 ... 100: picid = 1; break;
        case 65 ... 84:  picid = 2; break;
        case 35 ... 64:  picid = 3; break;
        case 15 ... 34:  picid = 4; break;
        case 0 ... 14:   picid = 5; break;
    }

    sprite[picid].pushRotateZoom(&tft, 240, 108, 0, 0.8, 0.8, COLOR_BG);

    // 2. A potem tworzymy paski
    drawThemAll();
}

void Status::drawScreenRAD() {
    sprite[2].pushRotateZoom(&tft, 
    240, 
    200, 
    0, 
    1.0, 
    1.0); 
}

void Status::drawScreenZEG() {
    
    tft.setFont(&monofonto_rg70pt7b);
    tft.setTextColor(COLOR_GREEN, COLOR_BG);
    
    time_t now;
    struct tm timeinfo;
    time(&now);
    localtime_r(&now, &timeinfo);
    
    char timeStringBuff[10]; 

    if (timeinfo.tm_year < 100) {
        strcpy(timeStringBuff, "--:--");
    } else {
        // Mruganie dwukropka co sekundę (parzysta sekunda = dwukropek, nieparzysta = spacja)
        if (timeinfo.tm_sec % 2 == 0) {
            strftime(timeStringBuff, sizeof(timeStringBuff), "%H:%M", &timeinfo);
        } else {
            strftime(timeStringBuff, sizeof(timeStringBuff), "%H %M", &timeinfo);
        }
    }

    tft.drawString(timeStringBuff, 80, 160);
    tft.unloadFont();
}
void Status::drawScreenDIFF() {
    sprite[1].pushRotateZoom(&tft, 
    240, 
    160, 
    0, 
    1.0, 
    1.0); 
}
void Status::drawScreenUST() {
    const int TABLE_X    = 80;   
    const int TABLE_Y    = 60;   
    const int ROW_H      = 42;   
    const int COL_NAME_W = 180;  
    const int COL_VAL_W  = 140;  
    const int TABLE_W    = COL_NAME_W + COL_VAL_W;
    
    const int ITEMS_PER_PAGE = 4; 
    
    int current_page = ust_cursor / ITEMS_PER_PAGE;
    
    // Ustalamy indeks początkowy i końcowy dla pętli rysującej
    int start_idx = current_page * ITEMS_PER_PAGE;
    int end_idx = start_idx + ITEMS_PER_PAGE;
    
    if (end_idx > SETTINGS_COUNT) {
        end_idx = SETTINGS_COUNT;
    }

    tft.fillRect(TABLE_X, TABLE_Y, TABLE_W, ROW_H - 6, COLOR_GREEN);
    tft.setFont(&monofonto_rg13pt7b);
    tft.setTextColor(COLOR_BG);
    tft.setTextDatum(ML_DATUM);
    tft.drawString("USTAWIENIE", TABLE_X + 10, TABLE_Y + (ROW_H - 6) / 2);
    tft.setTextDatum(MR_DATUM);
    tft.drawString("WARTOSC",    TABLE_X + TABLE_W - 10, TABLE_Y + (ROW_H - 6) / 2);

    for (int i = start_idx; i < end_idx; i++) {
        
        int screen_row = i - start_idx; 
        int row_y = TABLE_Y + 5 + (ROW_H - 6) + screen_row * ROW_H;
        
        bool selected = (i == ust_cursor) && (action_pool == SCR_UST);

        if (selected && !ust_edit) {
            tft.fillRect(TABLE_X, row_y, TABLE_W, ROW_H - 2, COLOR_GREEN);
        } else if (selected && ust_edit) {
            tft.fillRect(TABLE_X, row_y, TABLE_W, ROW_H - 2, COLOR_BG);
            tft.drawRect(TABLE_X, row_y, TABLE_W, ROW_H - 2, COLOR_GREEN);
        } else {
            tft.fillRect(TABLE_X, row_y, TABLE_W, ROW_H - 2, COLOR_BG);
            tft.drawFastHLine(TABLE_X, row_y + ROW_H - 2, TABLE_W, COLOR_GREEN);
        }

        tft.drawFastVLine(TABLE_X + COL_NAME_W, row_y, ROW_H - 2, COLOR_GREEN);

        tft.setFont(&monofonto_rg13pt7b);
        tft.setTextDatum(ML_DATUM);
        tft.setTextColor(selected && !ust_edit ? COLOR_BG : COLOR_GREEN);
        tft.drawString(settings[i].name, TABLE_X + 10, row_y + ROW_H / 2);

        char val_buf[24];

        if (i == 2) {
            if (ust_edit && selected) {
                int pos = 0;
                for(int j = 0; j < 8; j++) {
                    if (j == name_char_idx) val_buf[pos++] = '>';
                    val_buf[pos++] = playerName[j];
                    if (j == name_char_idx) val_buf[pos++] = '<';
                }
                val_buf[pos] = '\0';
            } else {
                snprintf(val_buf, sizeof(val_buf), "%s", playerName);
            }
        } else {
            if (strcmp(settings[i].unit, "BOOL") == 0) {
                snprintf(val_buf, sizeof(val_buf), "%s", settings[i].value ? "ON" : "OFF");
            } else if (strcmp(settings[i].unit, "THEM") == 0) { 
                if (settings[i].value == 0) snprintf(val_buf, sizeof(val_buf), "F3");
                else if (settings[i].value == 1) snprintf(val_buf, sizeof(val_buf), "NV");
                else snprintf(val_buf, sizeof(val_buf), "QUANTUM");
            }
            else if (strcmp(settings[i].unit, "BACK") == 0){
                snprintf(val_buf, sizeof(val_buf), "%s", settings[i].value ? "" : "");
            
            } else {
                snprintf(val_buf, sizeof(val_buf), "%d%s", settings[i].value, settings[i].unit);
            }
        }

        tft.setTextDatum(MR_DATUM);
        
        if (selected && ust_edit) {
            tft.setTextColor(COLOR_GREEN);
            
            if (i == 0) {
                tft.drawString(val_buf, TABLE_X + TABLE_W - 10, row_y + ROW_H / 2);
            } else {
                char edit_buf[32];
                snprintf(edit_buf, sizeof(edit_buf), "< %s >", val_buf);
                tft.drawString(edit_buf, TABLE_X + TABLE_W - 10, row_y + ROW_H / 2);
            }
        } else {
            tft.drawString(val_buf, TABLE_X + TABLE_W - 10, row_y + ROW_H / 2);
        }
    }
    tft.unloadFont();

    int arrow_x = TABLE_X + TABLE_W + 20; 

    if (start_idx > 0) {
        int up_y = TABLE_Y + 30; // Pozycja nad pierwszym elementem
        // fillTriangle(x1, y1, x2, y2, x3, y3, color)
        tft.fillTriangle(arrow_x - 10, up_y + 10, arrow_x + 10, up_y + 10, arrow_x, up_y, COLOR_GREEN);
    }

    if (end_idx < SETTINGS_COUNT) {
        int down_y = TABLE_Y + 5 + (ROW_H - 6) + (ITEMS_PER_PAGE * ROW_H) - 10; 
        // Trójkąt skierowany w dół
        tft.fillTriangle(arrow_x - 10, down_y - 10, arrow_x + 10, down_y - 10, arrow_x, down_y, COLOR_GREEN);
    }
}

void Status::statusSelect() {
    if (action_pool == SCR_NUFN) {
        if (current_selected == SCR_UST) {
            action_pool = SCR_UST;
            ust_cursor = 0;
            ust_edit = false;
            
            // Pobieramy aktualny czas, by zaktualizować pozycje w menu
            time_t now;
            struct tm timeinfo;
            time(&now);
            localtime_r(&now, &timeinfo);
            settings[10].value = timeinfo.tm_hour;
            settings[11].value = timeinfo.tm_min;

            drawScreen();
        }else if (current_selected == SCR_STN) {
            // WEJŚCIE W EDYCJĘ HP
            action_pool = SCR_STN;
            stn_edit = true;
            temp_hp = player.hp; // Pobieramy aktualne HP na start edycji
            drawScreen();
        }
    } else if (action_pool == SCR_STN) {
        if (stn_edit) {
            stn_edit = false;            // Zakończ edycję
            action_pool = SCR_NUFN; // Wróć sterowaniem do zakładek (TopBar)
            player.hp = temp_hp;
            settings[4].value = player.hp;
            
            Preferences prefs;
            prefs.begin("pipboy", false);
            prefs.putInt("player_hp", player.hp);
            prefs.putInt("UST_4", settings[4].value);
            prefs.end();

            drawTopBar();                // Aktualizacja TopBaru (HP w tekście)
            drawScreen();         // Rysujemy ponownie STN, co teraz zaktualizuje minkę
        }
    }
    else if (action_pool == SCR_UST) {
        if (!ust_edit) {
            ust_edit = true; // Wchodzimy w tryb edycji
            if (ust_cursor == 2) { // 2 = IMIE
                name_char_idx = 0; // Zawsze zaczynamy edycję od 1. litery
            }
            else if(ust_cursor == 0){ // 0 = WROC
                action_pool = SCR_NUFN;
                ust_edit = false;
            }
        } else {
            if (ust_cursor == 2) { // 2 = Jesteśmy w edycji IMIENIA
                name_char_idx++;   // Przeskocz na następną literę
                
                if (name_char_idx >= 8) { // Jeśli zatwierdzono 8. literę
                    ust_edit = false;     // Zakończ edycję
                    name_char_idx = 0;
                }
            } else if (ust_cursor == 3) {
                ust_edit = false; 
                player.level = settings[3].value;
                drawTopBar();
            } else if (ust_cursor == 4) {
                ust_edit = false;
                player.hp = settings[4].value;
                temp_hp = player.hp;
                Preferences prefs;
                prefs.begin("pipboy", false);
                prefs.putInt("player_hp", player.hp);
                prefs.putInt("UST_4", settings[4].value);
                prefs.end();
                drawTopBar();
            } else if (ust_cursor == 1) { 
                ust_edit = false;
                
                setGlobalThemeColors(settings[1].value);
                
                load_all_sprites(); 
                topbot();
                drawScreen();
            } else if (ust_cursor == 10 || ust_cursor == 11) {
                ust_edit = false;
                
                time_t now;
                struct tm timeinfo;
                time(&now);
                localtime_r(&now, &timeinfo);
                
                timeinfo.tm_hour = settings[10].value;
                timeinfo.tm_min = settings[11].value;
                timeinfo.tm_sec = 0; 
                
                time_t timeSinceEpoch = mktime(&timeinfo);
                struct timeval tv;
                tv.tv_sec = timeSinceEpoch;
                tv.tv_usec = 0;
                settimeofday(&tv, NULL);
                
                Preferences prefs;
                prefs.begin("zegar", false);
                prefs.putULong("zapisany_czas", timeSinceEpoch);
                prefs.end();
            } else {
                ust_edit = false; 
            }
            
            if (!ust_edit) {
                Preferences prefs;
                prefs.begin("pipboy", false);
                
                if (ust_cursor == 2) {
                    prefs.putString("playerName", playerName);
                } else if (ust_cursor != 0) { // 0 = "Wroc"
                    char klucz[15];
                    snprintf(klucz, sizeof(klucz), "UST_%d", ust_cursor);
                    prefs.putInt(klucz, settings[ust_cursor].value);
                }
                
                prefs.end();
            }
        }
        drawScreen();
    }
}
void Status::drawScreenGMF() {
    tft.fillRect(70, 60, 350, 120, COLOR_BG); 
    
    // 2. Wymuszamy ustawienia czcionki
    tft.setFont(&monofonto_rg70pt7b);
    tft.setTextColor(COLOR_GREEN, COLOR_BG); // Dodajemy kolor tła, by nadpisywał piksele
    tft.setTextDatum(TL_DATUM); // Standardowe wyrównanie (Top-Left)
    
    
    tft.drawString("SKN", 80, 40);
    tft.drawString("GRiD", 80, 160);
    tft.unloadFont();
}

void Status::update() {
    if (current_selected == SCR_ZEG && action_pool == SCR_NUFN) {
        
        static int last_second = -1;
        time_t now;
        struct tm timeinfo;
        time(&now);
        localtime_r(&now, &timeinfo);

        if (timeinfo.tm_year > 100) { // Zegar pomyślnie ustawiony (rok > 2000)
            if (timeinfo.tm_sec != last_second) {
                last_second = timeinfo.tm_sec;
                drawScreenZEG();
            }
        }
    }
}