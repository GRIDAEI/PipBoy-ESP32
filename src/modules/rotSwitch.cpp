#include "rotSwitch.h"

RotSwitch::RotSwitch(int pin) {
    setPin(pin);
}

// NOWA FUNKCJA: Tłumaczy odczyt ADC na pozycję 0-4 (lub -1 w razie błędu)
int RotSwitch::getSwitchPosition(int adcValue) {
    // Pozycja 0: odczyty 4095
    if (adcValue >= 3800) {
        return 0;
    }
    // Pozycja 1: odczyty ok. 2870-3190
    else if (adcValue >= 2700 && adcValue <= 3400) {
        return 1;
    }
    // Pozycja 2: odczyty ok. 1910-2080
    else if (adcValue >= 1700 && adcValue <= 2300) {
        return 2;
    }
    // Pozycja 3: odczyty ok. 880-1100
    else if (adcValue >= 700 && adcValue <= 1300) {
        return 3;
    }
    // Pozycja 4: ~0V (odczyty 0-50)
    else if (adcValue < 400) {
        return 4;
    }

    // Stan nieustalony (pomiędzy "oknami" w trakcie kręcenia)
    return -1; 
}

int RotSwitch::update() {
    int adcValue = analogRead(analogPin);
    
    static unsigned long lastPrint = 0;
    if (millis() - lastPrint > 500) {
        Serial.print("rSwitch ADC: ");
        Serial.println(adcValue);
        lastPrint = millis();
    }

    int currentPosition = getSwitchPosition(adcValue);

    // 1. ODRZUCENIE BŁĘDÓW: Odczyt nie trafia w żadne "okno"
    if (currentPosition == -1) {
        are_you_sure = 0; 
        return -1; // Zwracamy -1 (brak potwierdzonej akcji)
    }

    // 2. BRAK ZMIANY: Pozycja stabilna
    if (currentPosition == lastPosition) {
        are_you_sure = 0; 
        return -1; 
    }

    // 3. WYKRYTO ZMIANĘ: Czekamy na upewnienie się (debouncing)
    if (!change_confidence()) {
        return -1; 
    }
  
    // 4. MAMY PEWNOŚĆ: Zmiana została potwierdzona
    lastPosition = currentPosition; // Zapisujemy nowy stan
    
    Serial.print("NOWA POZYCJA PRZEŁĄCZNIKA: ");
    Serial.println(currentPosition);
  
    // Zwracamy KONKRETNĄ pozycję (0, 1, 2, 3 lub 4)
    return currentPosition; 
}

void RotSwitch::update(ESP32Encoder &encoder, bool &up, bool &down) {
    long currentCount = (long)encoder.getCount();
    static long lastCount = 0; 
    
    // Domyślnie resetujemy stany na HIGH (zakładamy brak ruchu)
    up = HIGH;
    down = HIGH;
    
    // Jeśli pozycja się zmieniła
    if (currentCount != lastCount) {
        if (currentCount > lastCount) {
            up = LOW;     // Zmiana w górę
        } 
        else {
            down = LOW;   // Zmiana w dół
        }
        
        lastCount = currentCount; 
    }
}