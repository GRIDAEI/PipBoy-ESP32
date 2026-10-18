#include "screen.h"

#include "display.h"
#include "topbar.h"
#include "botbar.h"
#include <functional>
#include <vector>
#include <ESP32Encoder.h>     // 1. Dodaj bibliotekę enkodera

extern ESP32Encoder encoder;
#include "./items/bron.h"
#include "./items/stroj.h"
#include "./items/pomoc.h"
#include "./items/rozne.h"
#include "./items/ammo.h"

#include "./data/localMap.h"
#include "./data/worldMap.h"
#include "./data/zadania.h"
#include "./data/dane.h"
#include "./data/radio.h"
AppScreen* current_screen = nullptr;
Special objSpecialScreen;
Status objStatusScreen;
Cechy objCechyScreen;
Relacje objRelacjeScreen;
Bron objBronScreen;
Stroj objStrojScreen;
Pomoc objPomocScreen;
Rozne objRozneScreen;
Ammo objAmmoScreen;
Zadania objZadaniaScreen;
Dane objDaneScreen;
Radio objRadioScreen;

Special& getSpecial() {
    static Special instSp;
    return instSp;
}

Status& getStatus() {
    return objStatusScreen;
}
Skille& getSkille() {
    static Skille instSk;
    return instSk;
}
Cechy& getCechy() {
    static Cechy instCh;
    return instCh;
}

Relacje& getRelacje() {
    static Relacje instRe;
    return instRe;
}

Bron& getBron() {
    static Bron instBr;
    return instBr;
}

Stroj& getStroj() {
    static Stroj instStr;
    return instStr;
}

Pomoc& getPomoc() {
    static Pomoc instPmc;
    return instPmc;
}

Rozne& getRozne() {
    static Rozne instRozn;
    return instRozn;
}

Ammo& getAmmo() {
    static Ammo instAmmo;
    return instAmmo;
}

LocalMap& getLocalMap() {
    static LocalMap instLocalMap;
    return instLocalMap;
}

WorldMap& getWorldMap() {
    static WorldMap instWorldMap;
    return instWorldMap;
}

Zadania& getZadania() {
    static Zadania instZadania;
    return instZadania;
}

Dane& getDane() {
    static Dane instDane;
    return instDane;
}
Radio& getRadio() {
    static Radio instRadio;
    return instRadio;
}
AppScreen Status_Screen{
    []() { getStatus().drawScreen(); },
    []() { getStatus().changeCursor(-1); }, // Zmiana na -1 (ruch w górę to zmniejszenie indeksu)
    []() { getStatus().changeCursor(1); },  // Zmiana na 1 (ruch w dół to zwiększenie indeksu)
    []() { getStatus().statusSelect(); }
};
AppScreen Special_Screen{
    []() { getSpecial().drawList(); },
    []() { getSpecial().up(); },
    []() { getSpecial().down(); },
    []() { getSpecial().select(); }
};
AppScreen Skille_Screen{
    []() { getSkille().drawList(); },
    []() { getSkille().up(); },
    []() { getSkille().down(); },
    []() { getSkille().select(); }
};
AppScreen Cechy_Screen{
    []() { getCechy().drawList(); },
    []() { getCechy().up(); },
    []() { getCechy().down(); },
    []() { getCechy().select(); }
};
AppScreen Relacje_Screen{
    []() { getRelacje().drawList(); },
    []() { getRelacje().up(); },
    []() { getRelacje().down(); },
    []() { getRelacje().select(); }
};
AppScreen Bron_Screen{
    []() { getBron().drawList(); },
    []() { getBron().up(); },
    []() { getBron().down(); },
    []() { getBron().select();}
};
AppScreen Stroj_Screen{
    []() { getStroj().drawList(); },
    []() { getStroj().up(); },
    []() { getStroj().down(); },
    []() { getStroj().select();}
};

AppScreen Pomoc_Screen{
    []() { getPomoc().drawList(); },
    []() { getPomoc().up(); },
    []() { getPomoc().down(); },
    []() { }
};

AppScreen Rozne_Screen{
    []() { getRozne().drawList(); },
    []() { getRozne().up(); },
    []() { getRozne().down(); },
    []() { }
};

AppScreen Ammo_Screen{
    []() { getAmmo().drawList(); },
    []() { getAmmo().up(); },
    []() { getAmmo().down(); },
    []() { }
};
AppScreen LocalMap_Screen{
    []() { getLocalMap().drawScreen(); },
    []() { getLocalMap().up(); },
    []() { getLocalMap().down(); },
    []() { getLocalMap().select(); }
};

AppScreen WorldMap_Screen{
    []() { getWorldMap().drawScreen(); },
    []() { getWorldMap().up(); },
    []() { getWorldMap().down(); },
    []() { getWorldMap().select(); }
};

AppScreen Zadania_Screen{
    []() { getZadania().drawList(); },
    []() { getZadania().up(); },
    []() { getZadania().down(); },
    []() { getZadania().select(); }
};

AppScreen Dane_Screen{
    []() { getDane().drawList(); },
    []() { getDane().up(); },
    []() { getDane().down(); },
    []() { getDane().select(); }
};
AppScreen Radio_Screen{
    []() { getRadio().drawScreen(); },
    []() { getRadio().up(); },
    []() { getRadio().down(); },
    []() { getRadio().select(); }
};
// --- Globalne zmienne nawigacyjne ---
int section_id = 0; // 0 = STATS, 1 = ITEMS, 2 = DATA
int screen_id = 0;  // Indeks aktualnej zakładki na dole

struct Section{
    std::vector<const char*> screen_names;
    std::vector<AppScreen> screens;
};

Section STATS{
     {"Status","S.P.E.C.J.A.L.", "Skille","Cechy","Relacje"}, {Status_Screen,Special_Screen,Skille_Screen, Cechy_Screen, Relacje_Screen}
};
Section ITEMS{
     {"Bron","Odziez", "Wsparcie","Rozne","Amunicja"}, {Bron_Screen,Stroj_Screen,Pomoc_Screen, Rozne_Screen, Ammo_Screen}
};
Section DATA{
     {"Local Map","World Map", "Zadania","Dane","Radio"}, {LocalMap_Screen,WorldMap_Screen,Zadania_Screen, Dane_Screen, Radio_Screen}
};

// --- Tablica przechowująca wskaźniki na wszystkie sekcje ---
Section* all_sections[] = { &STATS, &ITEMS, &DATA };
const int max_sections = 3;


int32_t currentRenderEncoderCount = 0;


bool isEncoderMoved() {
    return (int32_t)encoder.getCount() != currentRenderEncoderCount;
}

static int32_t lastEncoderCount = 0;


extern const int FNTBTN1 = 38;
extern const int FNTBTN2 = 47;
extern const int FNTBTN3 = 14;
bool isInputChanged() {
    // 1. Sprawdzenie enkodera (stara logika)
    if ((int32_t)encoder.getCount() != currentRenderEncoderCount) return true;

    // 2. Sprawdzenie przycisków (NOWE)
    // Jeśli rysujemy sekcję 1 lub 2, a ktoś wcisnął przycisk od sekcji 0 -> przerwij!
    // if (digitalRead(FNTBTN1) == LOW && section_id != 0) return true;
    // if (digitalRead(FNTBTN2) == LOW && section_id != 1) return true;
    // if (digitalRead(FNTBTN3) == LOW && section_id != 2) return true;

    return false;
}


void topbot(){
    drawTopBar(); 
    drawBottomNav(screen_id); // Używamy aktualnego ID zamiast sztywnej 1
}

void load_all_sprites(){
    getStatus().loadSprites();
    getSpecial().load_special_sprites();
    getCechy().load_cechy_sprites();
    getSkille().load_skille_sprites();
    getRelacje().load_relacje_sprites();
    getBron().load_bron_sprites();
    getStroj().load_stroj_sprites();
    getPomoc().load_pomoc_sprites();
    getRozne().load_rozne_sprites();
    getAmmo().load_ammo_sprites();
    getLocalMap().load_localMap_sprites();
    getWorldMap().load_worldMap_sprites();
}

void init_all_stats() {
    // 1. Konfiguracja maksymalnych wartości i unikalnych prefiksów
    getSpecial().setModifiable(true, 10, "SPC");
    getSkille().setModifiable(true, 100, "SKL");
    getCechy().setModifiable(true, 1, "CCH"); // Cechy to pewnie 0/1 (odblokowana)
    getRelacje().setModifiable(true, 100, "REL");

    // Jeśli chcesz by zakładki ekwipunku miały system zakładania:
    getBron().setEquippable(true);
    getStroj().setEquippable(true);

    // 2. Wczytanie z pamięci masowej
    getStatus().loadFromPreferences(); // <--- WCZYTANIE USTAWIEŃ STATUSU
    getSpecial().loadFromPreferences();
    getSkille().loadFromPreferences();
    getCechy().loadFromPreferences();
    getRelacje().loadFromPreferences();
}

const char* section_names[] = {
    "STATY.", 
    "PRZEDM.", 
    "DANE."
};
// --- NOWA FUNKCJA: Przełączanie głównych sekcji (górny pasek) ---
void change_section(int new_id) {
    
    if (isInputChanged()) return;
    if (new_id >= max_sections) new_id = max_sections - 1;
    if (new_id < 0) new_id = 0;
    
    // Zmieniamy zawartość tylko wtedy, gdy faktycznie przełączono na inną sekcję
    if (new_id != section_id) {
        section_id = new_id;
        screen_id = 0; // Po wejściu w nową sekcję zawsze resetujemy do 1. zakładki
        currentTopName = section_names[section_id];
        current_screen = &all_sections[section_id]->screens[screen_id];
        
        tft.fillRect(0, 50, 480, 250, COLOR_BG);
        drawTopBar(); // Podświetla odpowiednią kategorię na górnym pasku
        drawBottomNav(screen_id); // Odświeża dolne nazwy dla nowej sekcji
        current_screen->drawFunction();
    }
}

// --- ZMODYFIKOWANA FUNKCJA: Przełączanie zakładek (dolny pasek) ---
void change_screen(int i){
    int d = screen_id + i;
    // Sprawdzamy maksymalną ilość zakładek w BIEŻĄCEJ sekcji
    int max_d = (int)all_sections[section_id]->screens.size() - 1;
    
    if (d > max_d) d = max_d;
    else if (d < 0) d = 0;
    
    if (d != screen_id) {
        screen_id = d;
        current_screen = &all_sections[section_id]->screens[screen_id];
        
        tft.fillRect(0, 50, 480, 250, COLOR_BG);
        drawTopBar();
        drawBottomNav(screen_id);
        current_screen->drawFunction();
    }
}



void set_screen(int new_id) {
    // Sprawdzamy maksymalną ilość zakładek w BIEŻĄCEJ sekcji
    int max_id = (int)all_sections[section_id]->screens.size() - 1;
    
    // Zabezpieczenie na krawędziach przed błędami (np. ADC wrzuci 4, a ekranów jest 3)
    if (new_id > max_id) new_id = max_id;
    else if (new_id < 0) new_id = 0;
    
    // Zmieniamy zawartość tylko wtedy, gdy faktycznie przełączono na inną zakładkę
    if (new_id != screen_id) {
        screen_id = new_id;
        current_screen = &all_sections[section_id]->screens[screen_id];
        
        tft.fillRect(0, 50, 480, 250, COLOR_BG);
        drawTopBar();
        drawBottomNav(screen_id);
        current_screen->drawFunction();
    }
}
void current_up(){
    if (current_screen) current_screen->up();
}

void current_down(){
    if (current_screen) current_screen->down();
}

void press_current_select(){
    if (current_screen) current_screen->select();
}

