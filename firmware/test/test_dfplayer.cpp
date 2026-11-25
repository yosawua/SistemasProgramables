/**
 * @file test_dfplayer.cpp
 * @brief Prueba de reproducción de audio con DFPlayer Mini (clon MP3-TF-16P)
 * @author Los Pythones
 *
 * Conexiones de Hardware:
 * - VCC: 5V (desde reductor buck o pin VIN)
 * - GND: GND común del circuito
 * - TX (DFPlayer): GPIO 16 (RX2 del ESP32)
 * - RX (DFPlayer): GPIO 17 (TX2 del ESP32) a través de una resistencia de 1k
 * - SPK_1 / SPK_2: A bocina de 8 ohms / 2W-3W
 *
 * NOTA DE HARDWARE:
 * Se debe colocar una resistencia de 1k en serie entre el pin TX del ESP32 (GPIO 17)
 * y el pin RX del DFPlayer Mini. Esto adapta la señal de 3.3V a 5V y elimina el ruido
 * estático y zumbidos causados por la diferencia de potenciales.
 */

#include <Arduino.h>
#include <DFRobotDFPlayerMini.h>
#include "pins.h"
#include "config.h"

// Instancia de comunicación Serial por hardware (UART2)
HardwareSerial dfSerial(2);
DFRobotDFPlayerMini myDFPlayer;

void setup() {
    Serial.begin(115200);
    delay(1000);

    Serial.println("==================================================");
    Serial.println("   PRUEBA DE HARDWARE: REPRODUCTOR DFPLAYER MINI");
    Serial.println("==================================================");

    // NOTA: Resistencia de 1k entre TX del ESP32 (GPIO 17) y RX del DFPlayer
    dfSerial.begin(9600, SERIAL_8N1, PIN_DFPLAYER_RX, PIN_DFPLAYER_TX);

    Serial.println("Inicializando DFPlayer Mini (UART2: RX=16, TX=17)...");

    if (!myDFPlayer.begin(dfSerial, true, true)) {
        Serial.println("❌ ERROR: No se pudo comunicar con el DFPlayer Mini.");
        Serial.println("   Revisa:");
        Serial.println("   1. Alimentación de 5V al módulo.");
        Serial.println("   2. Cruce de líneas (TX ESP32 -> 1k -> RX DFPlayer; RX ESP32 <- TX DFPlayer).");
        Serial.println("   3. Tarjeta MicroSD insertada, formateada en FAT32 con archivos 0001.mp3, etc.");
        while (true) {
            delay(1000);
        }
    }

    Serial.println("✅ DFPlayer Mini detectado y listo.");
    myDFPlayer.volume(DFPLAYER_VOLUMEN); // Configurar volumen (0 a 30)

    Serial.println("▶️  Reproduciendo pista 0001.mp3 (\"Recipiente metálico detectado\")...");
    myDFPlayer.play(1);
}

void loop() {
    static unsigned long ultimoChequeo = 0;

    // Reporte de eventos del reproductor
    if (myDFPlayer.available()) {
        uint8_t tipo = myDFPlayer.readType();
        int valor = myDFPlayer.read();

        switch (tipo) {
            case DFPlayerPlayFinished:
                Serial.printf("⏹️  Pista %d ha terminado de reproducirse.\n", valor);
                break;
            case DFPlayerError:
                Serial.printf("❌ Error DFPlayer Código: %d\n", valor);
                break;
            default:
                break;
        }
    }

    // Repetir reproducción cada 8 segundos para prueba continua
    if (millis() - ultimoChequeo > 8000) {
        ultimoChequeo = millis();
        Serial.println("▶️  Re-ejecutando pista 0001.mp3...");
        myDFPlayer.play(1);
    }

    delay(50);
}
