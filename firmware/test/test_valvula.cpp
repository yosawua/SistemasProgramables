/**
 * @file test_valvula.cpp
 * @brief Prueba de accionamiento de la Electroválvula 12V con TB6612FNG (Canal B)
 * @author Los Pythones
 *
 * Secuencia de prueba:
 * - Abre la electroválvula durante 2 segundos (activación de solenoide 12V).
 * - Cierra la electroválvula durante 2 segundos.
 */

#include <Arduino.h>
#include "pins.h"
#include "config.h"

void abrirValvula() {
    digitalWrite(PIN_VALVULA_BIN1, HIGH);
    digitalWrite(PIN_VALVULA_BIN2, LOW);
    ledcWrite(PWM_VALVULA_CANAL, POTENCIA_VALVULA_ON);
}

void cerrarValvula() {
    digitalWrite(PIN_VALVULA_BIN1, LOW);
    digitalWrite(PIN_VALVULA_BIN2, LOW);
    ledcWrite(PWM_VALVULA_CANAL, POTENCIA_VALVULA_OFF);
}

void setup() {
    Serial.begin(115200);
    delay(1000);

    Serial.println("==================================================");
    Serial.println("   PRUEBA DE HARDWARE: ELECTROVÁLVULA 12V");
    Serial.println("==================================================");

    pinMode(PIN_TB6612_STBY, OUTPUT);
    pinMode(PIN_VALVULA_BIN1, OUTPUT);
    pinMode(PIN_VALVULA_BIN2, OUTPUT);

    // Habilitar driver
    digitalWrite(PIN_TB6612_STBY, HIGH);

    // Configuración PWM Canal B
    ledcSetup(PWM_VALVULA_CANAL, PWM_VALVULA_FREQ, PWM_VALVULA_RES);
    ledcAttachPin(PIN_VALVULA_PWMB, PWM_VALVULA_CANAL);

    cerrarValvula();

    Serial.println("Canal B configurado. Iniciando ciclos de apertura/cierre...");
    Serial.println("--------------------------------------------------");
}

void loop() {
    Serial.println("💧 [VÁLVULA ABIERTA]  -> Dosificando líquido (2 segundos)...");
    abrirValvula();
    delay(2000);

    Serial.println("🛑 [VÁLVULA CERRADA]  -> Solenoide desenergizado (2 segundos)...");
    cerrarValvula();
    delay(2000);
}
