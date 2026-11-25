/**
 * @file test_motor.cpp
 * @brief Prueba de control del Motor DC 12V con Driver TB6612FNG (Canal A)
 * @author Los Pythones
 *
 * Secuencia de prueba:
 * 1. Giro en sentido horario (CW - avance) con rampa de velocidad PWM (0 a 255).
 * 2. Parada por 1 segundo.
 * 3. Giro en sentido antihorario (CCW - retroceso) con rampa de velocidad PWM.
 * 4. Parada por 1 segundo.
 */

#include <Arduino.h>
#include "pins.h"
#include "config.h"

void fijarDireccion(bool sentidoHorario) {
    if (sentidoHorario) {
        digitalWrite(PIN_MOTOR_AIN1, HIGH);
        digitalWrite(PIN_MOTOR_AIN2, LOW);
    } else {
        digitalWrite(PIN_MOTOR_AIN1, LOW);
        digitalWrite(PIN_MOTOR_AIN2, HIGH);
    }
}

void frenarMotor() {
    digitalWrite(PIN_MOTOR_AIN1, LOW);
    digitalWrite(PIN_MOTOR_AIN2, LOW);
    ledcWrite(PWM_MOTOR_CANAL, 0);
}

void setup() {
    Serial.begin(115200);
    delay(1000);

    Serial.println("==================================================");
    Serial.println("   PRUEBA DE HARDWARE: MOTOR DC CON TB6612FNG");
    Serial.println("==================================================");

    // Configuración de pines de dirección y Standby
    pinMode(PIN_TB6612_STBY, OUTPUT);
    pinMode(PIN_MOTOR_AIN1, OUTPUT);
    pinMode(PIN_MOTOR_AIN2, OUTPUT);

    // Habilitar driver
    digitalWrite(PIN_TB6612_STBY, HIGH);
    frenarMotor();

    // Configuración de canal PWM con la API LEDC de ESP32
    ledcSetup(PWM_MOTOR_CANAL, PWM_MOTOR_FREQ, PWM_MOTOR_RES);
    ledcAttachPin(PIN_MOTOR_PWMA, PWM_MOTOR_CANAL);

    Serial.println("Driver habilitado y canal PWM configurado.");
    Serial.println("Iniciando ciclo de prueba...");
    Serial.println("--------------------------------------------------");
}

void loop() {
    // 1. AVANCE CON RAMPA DE VELOCIDAD
    Serial.println("\n>>> [1/4] AVANCE (Giro Horario) - Acelerando...");
    fijarDireccion(true);
    for (int pwm = 50; pwm <= 255; pwm += 25) {
        ledcWrite(PWM_MOTOR_CANAL, pwm);
        Serial.printf("    Velocidad PWM: %d / 255 (%.1f%%)\n", pwm, (pwm / 255.0) * 100.0);
        delay(250);
    }
    delay(1000);

    // 2. PARADA
    Serial.println(">>> [2/4] PARANDO MOTOR (1 segundo)...");
    frenarMotor();
    delay(1000);

    // 3. RETROCESO CON RAMPA DE VELOCIDAD
    Serial.println(">>> [3/4] RETROCESO (Giro Antihorario) - Acelerando...");
    fijarDireccion(false);
    for (int pwm = 50; pwm <= 255; pwm += 25) {
        ledcWrite(PWM_MOTOR_CANAL, pwm);
        Serial.printf("    Velocidad PWM: %d / 255 (%.1f%%)\n", pwm, (pwm / 255.0) * 100.0);
        delay(250);
    }
    delay(1000);

    // 4. PARADA
    Serial.println(">>> [4/4] PARANDO MOTOR (1 segundo)...");
    frenarMotor();
    delay(1500);
}
