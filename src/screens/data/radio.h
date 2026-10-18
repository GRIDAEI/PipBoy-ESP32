#pragma once
#include <Arduino.h>
#include "../../display.h"

// Struktura pojedynczego utworu / stacji radiowej
struct RadioTrack {
    const char* title;     // Nazwa wyświetlana na liście w Pip-Boyu
    const char* filePath;  // Ścieżka do pliku na karcie SD (np. "/mp3/gnr_intro.mp3")
    const char* freq;      // Klimatyczny dodatek: częstotliwość lub czas trwania
};

class Radio {
private:
    int currentSelection; // Który element na liście najeżdżamy kursorem
    int playingTrack;     // Który utwór aktualnie gra (-1 = żaden / cisza)

    // Pomocnicze funkcje do rysowania części ekranu
    void drawTrackList();
    void drawVisualizer();

public:
    Radio();
    
    // Główne metody wywoływane przez system menu (identycznie jak w WorldMap)
    void drawScreen();   // Rysuje cały ekran radia
    void up();           // Nawigacja w górę
    void down();         // Nawigacja w dół
    void select();       // Kliknięcie enkodera (Play / Stop)
};