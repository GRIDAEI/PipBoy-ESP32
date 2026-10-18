#include "radio.h"
#include "../../modules/audio.h" // Załączamy główny moduł dźwięku

extern LGFX tft;

// Lista MP3 na karcie SD w folderze /mp3/
RadioTrack radio_tracks[] = {
    {"SrakaFM",   "/mp3/burdel.mp3",      "98.5 FM"}
};

const int radio_max_tracks = sizeof(radio_tracks) / sizeof(radio_tracks[0]);

Radio::Radio() {
    currentSelection = 0;
    playingTrack = -1;
}

void Radio::drawScreen() {
    tft.fillRect(0, 50, 480, 250, COLOR_BG);
    drawTrackList();
    drawVisualizer();
}

void Radio::drawTrackList() {
    tft.setFont(&monofonto_rg9pt7b);
    int startX = 15, startY = 65, rowHeight = 28, listWidth = 230;

    for (int i = 0; i < radio_max_tracks; i++) {
        int y = startY + (i * rowHeight);
        if (y + rowHeight > 295) break;

        bool isSelected = (i == currentSelection);
        bool currentIsPlaying = (isRadioPlaying() && i == playingTrack);

        if (isSelected) {
            tft.fillRect(startX - 5, y - 2, listWidth, rowHeight - 4, COLOR_GREEN);
            tft.setTextColor(COLOR_BG, COLOR_GREEN);
        } else {
            tft.setTextColor(COLOR_GREEN, COLOR_BG);
        }

        tft.setTextDatum(TL_DATUM);
        String displayTitle = (currentIsPlaying ? "* " : "  ") + String(radio_tracks[i].title);
        tft.drawString(displayTitle, startX, y);
    }
}

void Radio::drawVisualizer() {
    int boxX = 260, boxY = 65, boxW = 200, boxH = 140;

    tft.drawRect(boxX, boxY, boxW, boxH, COLOR_GREEN);
    int midY = boxY + (boxH / 2);
    
    // Siatka oscyloskopu
    for (int x = boxX + 5; x < boxX + boxW - 5; x += 8) tft.drawPixel(x, midY, COLOR_GREEN);
    for (int y = boxY + 5; y < boxY + boxH - 5; y += 8) tft.drawPixel(boxX + (boxW / 2), y, COLOR_GREEN);

    // Animacja fali
    if (isRadioPlaying()) {
        int prevX = boxX + 2;
        int prevY = midY;
        for (int x = 2; x < boxW - 2; x++) {
            float angle = x * 0.15f;
            float envelope = sin((float)x / boxW * 3.14159f); 
            int waveY = midY + (int)(sin(angle) * cos(angle * 0.4f) * 45.0f * envelope);
            tft.drawLine(prevX, prevY, boxX + x, waveY, COLOR_GREEN);
            prevX = boxX + x; prevY = waveY;
        }
    } else {
        tft.drawFastHLine(boxX + 2, midY, boxW - 4, COLOR_GREEN);
    }

    // Status i tekst
    tft.setFont(&monofonto_rg9pt7b);
    tft.setTextColor(COLOR_GREEN, COLOR_BG);
    tft.setTextDatum(TL_DATUM);

    int infoY = boxY + boxH + 12;
    tft.drawFastHLine(boxX, infoY - 5, boxW, COLOR_GREEN);

    if (isRadioPlaying() && playingTrack >= 0) {
        tft.drawString("STATUS: ODTWARZANIE", boxX, infoY);
        tft.drawString("CZEST:  " + String(radio_tracks[playingTrack].freq), boxX, infoY + 22);
    } else {
        tft.drawString("STATUS: ZATRZYMANO", boxX, infoY);
        tft.drawString("CZEST:  " + String(radio_tracks[currentSelection].freq), boxX, infoY + 22);
    }
}

void Radio::up() {
    currentSelection = (currentSelection - 1 < 0) ? radio_max_tracks - 1 : currentSelection - 1;
    drawScreen();
}

void Radio::down() {
    currentSelection = (currentSelection + 1 >= radio_max_tracks) ? 0 : currentSelection + 1;
    drawScreen();
}

void Radio::select() {
    if (isRadioPlaying() && playingTrack == currentSelection) {
        stopRadio();
    } else {
        playingTrack = currentSelection;
        playRadio(radio_tracks[currentSelection].filePath);
    }
    drawScreen();
}