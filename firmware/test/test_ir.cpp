/**
 * @file test_ir.cpp
 * @brief Prueba de funcionamiento del Sensor Óptico Infrarrojo FC-51
 * @author Los Pythones
 *
 * Conexión:
 * - VCC: 3.3V
 * - GND: GND
 * - OUT: GPIO 35 del ESP32
 */

#include <Arduino.h>
#include "pins.h"

int ultimoEstado = -1;
unsigned long ultimoReporte = 0;

void setup() {
    Serial.begin(115200);
    delay(1000);

    Serial.println("==================================================");
    Serial.println("   PRUEBA DE HARDWARE: SENSOR IR FC-51");
    Serial.println("==================================================");
    Serial.printf("Configurando GPIO %d como entrada digital...\n", PIN_SENSOR_IR);

    pinMode(PIN_SENSOR_IR, INPUT);

    Serial.println("Coloca o retira un recipiente frente al sensor IR...");
    Serial.println("(Si no conmuta, ajusta el potenciómetro en la placa FC-51)");
    Serial.println("--------------------------------------------------");
}

void loop() {
    int estadoActual = digitalRead(PIN_SENSOR_IR);

    if (estadoActual != ultimoEstado || (millis() - ultimoReporte > 1000)) {
        ultimoEstado = estadoActual;
        ultimoReporte = millis();

        if (estadoActual == LOW) {
            Serial.println("🔴 [OBJETO DETECTADO] -> Recipiente en posición de llenado (LOW)");
        } else {
            Serial.println("⚪ [ZONA DESPEJADA]   -> No hay obstáculo presente (HIGH)");
        }
    }

    delay(20);
}
