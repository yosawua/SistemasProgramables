#pragma once

/**
 * @file pins.h
 * @brief Asignación de pines de hardware para ESP32 DevKit V1
 * @author Los Pythones - Proyecto Sistemas Programables
 *
 * Criterios de diseño del mapa de pines:
 * - GPIO 34-39: Entradas puras sin resistencias pull-up internas (usados para sensores con acondicionamiento externo).
 * - Evitar pines de bootloader/strapping para señales críticas (GPIO 0 y 12 quedan completamente libres).
 * - TFT ILI9341 utiliza el bus de hardware VSPI (GPIO 18, 19, 23, 5).
 * - DFPlayer Mini utiliza la interfaz por hardware Serial2 (UART2: GPIO 16 y 17).
 * - TB6612FNG controla motor DC (Canal A) y electroválvula (Canal B).
 * - Se reservan pines para sensor ultrasónico opcional (TRIG en GPIO 33, ECHO en GPIO 36).
 */

#ifndef PINS_H
#define PINS_H

// =============================================================================
// SENSORES DIGITALES DE ENTRADA
// =============================================================================

// Sensor Inductivo LJ12A3-4-Z/BX (Salida NPN Colector Abierto, alimentación 6-36V)
// NOTA: Requiere resistencia pull-up externa de 4.7k a 10k conectada a 3.3V del ESP32.
// GPIO 34 es entrada analógica/digital pura (sin pull-up interno).
// Lógica: HIGH = Sin metal, LOW = Objeto metálico detectado.
#define PIN_SENSOR_INDUCTIVO    34

// Sensor Infrarrojo de Proximidad FC-51 (Estación de Llenado)
// Módulo comparador con salida digital activa en nivel bajo.
// GPIO 35 es entrada analógica/digital pura.
// Lógica: HIGH = Sin obstáculo, LOW = Recipiente en posición de llenado.
#define PIN_SENSOR_IR           35

// Botón de Parada de Emergencia (STOP)
// Conectado entre este pin y GND. Utiliza resistencia INPUT_PULLUP interna del ESP32.
// Lógica: HIGH = Reposo (no presionado), LOW = Presionado.
#define PIN_BOTON_STOP          32

// Sensor Ultrasónico Opcional (HC-SR04 / RCWL-9610)
// TRIG: Salida digital de disparo (pulso de 10us).
// ECHO: Entrada digital (si se usa sensor de 5V, requiere divisor de voltaje para 3.3V).
#define PIN_US_TRIG             33
#define PIN_US_ECHO             36  // GPIO 36 (SENSOR_VP, entrada pura)

// =============================================================================
// DRIVER DE POTENCIA TB6612FNG (CONTROL DE ACTUADORES 12V)
// =============================================================================

// Pin Standby del driver (HIGH = Driver habilitado, LOW = Salidas en alta impedancia)
#define PIN_TB6612_STBY         26

// CANAL A: Motor DC 12V (Banda Transportadora)
#define PIN_MOTOR_PWMA          25  // Control de velocidad mediante modulación PWM
#define PIN_MOTOR_AIN1          14  // Control de sentido de giro A1
#define PIN_MOTOR_AIN2          13  // Control de sentido de giro A2

// CANAL B: Electroválvula 12V (Dosificación / Llenado)
#define PIN_VALVULA_PWMB        2   // Habilitación de potencia / PWM Canal B
#define PIN_VALVULA_BIN1        27  // Control de activación B1 (HIGH = energizada)
#define PIN_VALVULA_BIN2        15  // Control de activación B2 (LOW = GND en válvula)

// =============================================================================
// REPRODUCTOR DE AUDIO DFPLAYER MINI (UART2 / SERIAL2)
// =============================================================================

// NOTA ELÉCTRICA: Conectar una resistencia de 1k entre PIN_DFPLAYER_TX (GPIO 17)
// y el pin RX del módulo DFPlayer Mini para acoplamiento de 3.3V a 5V y supresión de ruido.
#define PIN_DFPLAYER_RX         16  // ESP32 RX2 <- Conectar a TX de DFPlayer
#define PIN_DFPLAYER_TX         17  // ESP32 TX2 -> Resistencia 1k -> RX de DFPlayer
#define PIN_DFPLAYER_BUSY       -1  // Opcional (-1 si no se conecta)

// =============================================================================
// PANTALLA TFT 2.8" ILI9341 240x320 (INTERFAZ SPI HARDWARE - VSPI)
// =============================================================================

#define PIN_TFT_SCK             18  // VSPI Clock (SCK)
#define PIN_TFT_MISO            19  // VSPI MISO
#define PIN_TFT_MOSI            23  // VSPI Data Out (MOSI)
#define PIN_TFT_CS              5   // Chip Select (CS)
#define PIN_TFT_DC              21  // Data / Command (DC/RS)
#define PIN_TFT_RST             22  // Hardware Reset (RST)
#define PIN_TFT_LED             4   // Control de Retroiluminación / Backlight (PWM o ON/OFF)

#endif // PINS_H
