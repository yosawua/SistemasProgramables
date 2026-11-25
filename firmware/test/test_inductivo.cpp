/**
 * @file test_inductivo.cpp
 * @brief Prueba de funcionamiento del Sensor Inductivo LJ12A3-4-Z/BX (NPN)
 * @author Los Pythones
 *
 * Conexión:
 * - Café: +12V (Alimentación)
 * - Azul: GND
 * - Negro (Salida NPN): GPIO 34 del ESP32 con resistencia PULL-UP externa de 10k a 3.3V.
 */

#include <Arduino.h>
#include "pins.h"

int ultimoEstado = -1;
unsigned long ultimoReporte = 0;

void setup() {
    Serial.begin(115200);
    delay(1000);

    Serial.println("==================================================");
    Serial.println("   PRUEBA DE HARDWARE: SENSOR INDUCTIVO NPN");
    Serial.println("==================================================");
    Serial.printf("Configurando GPIO %d como entrada (requiere pull-up externo a 3.3V)...\n", PIN_SENSOR_INDUCTIVO);

    pinMode(PIN_SENSOR_INDUCTIVO, INPUT);

    Serial.println("Aproxima un objeto metálico a la punta del sensor...");
    Serial.println("--------------------------------------------------");
}

void loop() {
    int estadoActual = digitalRead(PIN_SENSOR_INDUCTIVO);

    // Detección por cambio de estado o reporte periódico cada 1 segundo
    if (estadoActual != ultimoEstado || (millis() - ultimoReporte > 1000)) {
        ultimoEstado = estadoActual;
        ultimoReporte = millis();

        if (estadoActual == LOW) {
            Serial.println("🟢 [ESTADO: METAL DETECTADO] -> Entrada en nivel LOW (0V)");
        } else {
            Serial.println("⚪ [ESTADO: SIN METAL]       -> Entrada en nivel HIGH (3.3V pull-up)");
        }
    }

    delay(20);
}
