#include <Arduino.h>
#include <SPI.h>
#include <SD.h>
#include "AudioGeneratorWAV.h"
#include "AudioGeneratorMP3.h"     // Dodane do obsługi plików MP3 z Radia
#include "AudioFileSourceLittleFS.h"
#include "AudioFileSourceSD.h"     // Dodane do obsługi muzyki z karty SD
#include "AudioOutputI2S.h"
#include "AudioOutputMixer.h"

#define MAX_VOICES 3 

AudioOutputI2S *outI2S;
AudioOutputMixer *mixer;

// Kanały dla dźwięków UI (WAV z LittleFS)
AudioGeneratorWAV *wav[MAX_VOICES];
AudioFileSourceLittleFS *file[MAX_VOICES];
AudioOutputMixerStub *stub[MAX_VOICES];

// Dedykowany kanał dla Radia (MP3 z karty SD)
AudioGeneratorMP3 *mp3;
AudioFileSourceSD *fileSD;
AudioOutputMixerStub *stubRadio;

void initAudio() {
    outI2S = new AudioOutputI2S();
    outI2S->SetPinout(6, 7, 8); // Twoje piny I2S
    outI2S->SetGain(0.6); 

    mixer = new AudioOutputMixer(128, outI2S);

    // Inicjalizacja 3 kanałów do dźwięków systemowych
    for (int i = 0; i < MAX_VOICES; i++) {
        wav[i] = nullptr;
        file[i] = nullptr;
        stub[i] = mixer->NewInput();
        stub[i]->SetGain(0.25); 
    }

    // Inicjalizacja kanału Radia
    mp3 = nullptr;
    fileSD = nullptr;
    stubRadio = mixer->NewInput();
    stubRadio->SetGain(0.4); // Głośność Radia
    pinMode(15, OUTPUT);
    digitalWrite(15, HIGH);

    // 2. Przygotowanie pinu CS dla karty SD
    pinMode(21, OUTPUT);
    digitalWrite(21, HIGH);

    // 3. Uruchomienie szyny SPI (SCK=18, MISO=14, MOSI=12, CS=21)
    SPI.begin(18, 14, 12, 21); 

    // 4. Inicjalizacja z ograniczoną prędkością zegara do 4 MHz (bardzo pomaga przy współdzielonym SPI)
    if (!SD.begin(21, SPI, 4000000)) {
        Serial.println("AUDIO: Brak karty SD lub blad odczytu!");
    } else {
        Serial.println("AUDIO: Karta SD wykryta poprawnie.");
    }
}

void playSound(const char* name) {
    int freeChannel = -1;
    for (int i = 0; i < MAX_VOICES; i++) {
        if (wav[i] == nullptr || !wav[i]->isRunning()) {
            freeChannel = i;
            break;
        }
    }
    if (freeChannel == -1) return; 

    if (wav[freeChannel]) { delete wav[freeChannel]; wav[freeChannel] = nullptr; }
    if (file[freeChannel]) { delete file[freeChannel]; file[freeChannel] = nullptr; }

    file[freeChannel] = new AudioFileSourceLittleFS(name);
    if (!file[freeChannel]->isOpen()) {
        delete file[freeChannel]; 
        file[freeChannel] = nullptr;
        return;
    }

    wav[freeChannel] = new AudioGeneratorWAV();
    wav[freeChannel]->begin(file[freeChannel], stub[freeChannel]);
}

// --- NOWE FUNKCJE OBSŁUGI RADIA ---
void playRadio(const char* path) {
    if (mp3 && mp3->isRunning()) {
        mp3->stop();
    }
    if (mp3) { delete mp3; mp3 = nullptr; }
    if (fileSD) { delete fileSD; fileSD = nullptr; }

    fileSD = new AudioFileSourceSD(path);
    mp3 = new AudioGeneratorMP3();
    mp3->begin(fileSD, stubRadio);
}

void stopRadio() {
    if (mp3 && mp3->isRunning()) {
        mp3->stop();
    }
}

bool isRadioPlaying() {
    return (mp3 && mp3->isRunning());
}

// Główna pętla obsługująca dekodowanie
void handleAudio() {
    // 1. Zrzut buforów dla UI
    for (int i = 0; i < MAX_VOICES; i++) {
        if (wav[i] && wav[i]->isRunning()) {
            if (!wav[i]->loop()) {
                wav[i]->stop();
                delete wav[i]; wav[i] = nullptr;
                delete file[i]; file[i] = nullptr;
            }
        }
    }

    // 2. Zrzut buforów dla Radia
    if (mp3 && mp3->isRunning()) {
        if (!mp3->loop()) {
            mp3->stop();
            delete mp3; mp3 = nullptr;
            delete fileSD; fileSD = nullptr;
        }
    }
}