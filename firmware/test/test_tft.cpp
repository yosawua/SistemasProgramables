/**
 * @file test_tft.cpp
 * @brief Prueba de gráficos y texto en pantalla TFT 2.8" ILI9341 SPI (240x320)
 * @author Los Pythones
 *
 * Utiliza Adafruit_GFX y Adafruit_ILI9341 con bus hardware VSPI.
 */

#include <Arduino.h>
#include <SPI.h>
#include <Adafruit_GFX.h>
#include <Adafruit_ILI9341.h>
#include "pins.h"
#include "config.h"

Adafruit_ILI9341 tft = Adafruit_ILI9341(PIN_TFT_CS, PIN_TFT_DC, PIN_TFT_RST);

void setup() {
    Serial.begin(115200);
    delay(1000);

    Serial.println("==================================================");
    Serial.println("   PRUEBA DE HARDWARE: PANTALLA TFT ILI9341");
    Serial.println("==================================================");

    // Encender retroiluminación si está conectada
    if (PIN_TFT_LED >= 0) {
        pinMode(PIN_TFT_LED, OUTPUT);
        digitalWrite(PIN_TFT_LED, HIGH);
    }

    tft.begin();
    tft.setRotation(TFT_ROTATION); // Horizontal (320x240)

    Serial.println("Pantalla inicializada correctamente.");
}

void loop() {
    // 1. Prueba de Colores Base
    Serial.println(">>> Dibujando colores primarios...");
    tft.fillScreen(ILI9341_RED);
    delay(500);
    tft.fillScreen(ILI9341_GREEN);
    delay(500);
    tft.fillScreen(ILI9341_BLUE);
    delay(500);
    tft.fillScreen(ILI9341_BLACK);
    delay(300);

    // 2. Interfaz Gráfica Demo
    Serial.println(">>> Dibujando interfaz de demostración...");

    // Encabezado
    tft.fillRect(0, 0, 320, 38, ILI9341_NAVY);
    tft.drawFastHLine(0, 38, 320, ILI9341_CYAN);

    tft.setTextColor(ILI9341_WHITE);
    tft.setTextSize(2);
    tft.setCursor(18, 11);
    tft.print("SISTEMA BANDA 12V");

    // Tarjeta de Estado
    tft.fillRoundRect(15, 50, 290, 80, 8, ILI9341_DARKGREY);
    tft.drawRoundRect(15, 50, 290, 80, 8, ILI9341_WHITE);

    tft.setTextColor(ILI9341_YELLOW);
    tft.setTextSize(2);
    tft.setCursor(30, 65);
    tft.print("ESTADO:");

    tft.setTextColor(ILI9341_GREEN);
    tft.setTextSize(3);
    tft.setCursor(30, 92);
    tft.print("OPERATIVO");

    // Indicadores de Sensores
    tft.fillRoundRect(15, 142, 140, 75, 8, 0x18E3); // Gris azulado
    tft.setTextColor(ILI9341_WHITE);
    tft.setTextSize(1);
    tft.setCursor(25, 155);
    tft.print("SENSOR INDUCTIVO");
    tft.setTextSize(2);
    tft.setTextColor(ILI9341_GREEN);
    tft.setCursor(25, 180);
    tft.print("ACTIVO");

    tft.fillRoundRect(165, 142, 140, 75, 8, 0x18E3);
    tft.setTextColor(ILI9341_WHITE);
    tft.setTextSize(1);
    tft.setCursor(175, 155);
    tft.print("SENSOR IR");
    tft.setTextSize(2);
    tft.setTextColor(ILI9341_CYAN);
    tft.setCursor(175, 180);
    tft.print("LISTO");

    // Pie de pantalla
    tft.setTextSize(1);
    tft.setTextColor(ILI9341_LIGHTGREY);
    tft.setCursor(20, 225);
    tft.print("ESP32 DevKit V1 | Los Pythones 2026");

    Serial.println("Gráficos dibujados con éxito. Esperando 4 segundos...");
    delay(4000);
}
