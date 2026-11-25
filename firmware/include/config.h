#pragma once

/**
 * @file config.h
 * @brief Parámetros de configuración operativa del sistema embebido
 * @author Los Pythones - Proyecto Sistemas Programables
 */

#ifndef CONFIG_H
#define CONFIG_H

#include <Arduino.h>

// =============================================================================
// PARÁMETROS DEL MOTOR DE LA BANDA (CANAL A TB6612FNG)
// =============================================================================
#define PWM_MOTOR_FREQ          20000   // Frecuencia PWM en Hz (frecuencia inaudible)
#define PWM_MOTOR_RES           8       // Resolución en bits (0 - 255)
#define PWM_MOTOR_CANAL         0       // Canal LEDC del ESP32

#define VELOCIDAD_BANDA_NOMINAL 200     // Velocidad normal de transporte (0-255)
#define VELOCIDAD_BANDA_LENTA   140     // Velocidad de aproximación/posicionamiento
#define VELOCIDAD_BANDA_PARADA  0       // Motor apagado

// =============================================================================
// PARÁMETROS DE LA ELECTROVÁLVULA (CANAL B TB6612FNG)
// =============================================================================
#define PWM_VALVULA_FREQ        1000    // Frecuencia PWM en Hz
#define PWM_VALVULA_RES         8       // Resolución en bits (0 - 255)
#define PWM_VALVULA_CANAL       1       // Canal LEDC para válvula
#define POTENCIA_VALVULA_ON     255     // Activación al 100% (12V)
#define POTENCIA_VALVULA_OFF    0       // Válvula desenergizada

#define TIEMPO_LLENADO_MS       3000    // Duración de apertura de válvula (ms)
#define TIEMPO_DESPEJE_MS       1200    // Tiempo de marcha tras llenado para librar sensor IR (ms)

// =============================================================================
// PARÁMETROS DE FILTRADO Y ANTIRREBOTE
// =============================================================================
#define DEBOUNCE_BOTON_MS       50      // Antirrebote para botón de STOP
#define DEBOUNCE_SENSOR_MS      40      // Filtro para sensores inductivo e IR

// =============================================================================
// PARÁMETROS DE PANTALLA TFT ILI9341
// =============================================================================
#define TFT_ROTATION            3       // Orientación horizontal (320x240 landscape)
#define TFT_ANCHO               320
#define TFT_ALTO                240

// =============================================================================
// PARÁMETROS DEL REPRODUCTOR DFPLAYER MINI
// =============================================================================
#define DFPLAYER_VOLUMEN        25      // Volumen de reproducción (0 a 30)

// Índices de pistas de audio generadas (formato /000X.mp3 en tarjeta micro SD)
#define AUDIO_METAL_DETECTADO   1       // "Recipiente metálico detectado"
#define AUDIO_LLENANDO          2       // "Llenando"
#define AUDIO_LLENADO_COMPLETO  3       // "Llenado completo"
#define AUDIO_NO_METALICO       4       // "Recipiente no metálico"
#define AUDIO_SISTEMA_DETENIDO  5       // "Sistema detenido"

// =============================================================================
// DEFINICIÓN DE ESTADOS DE LA MÁQUINA DE CONTROL
// =============================================================================
enum EstadoSistema {
    ESTADO_ESPERANDO = 0,       // Esperando entrada de recipiente / banda en marcha
    ESTADO_METAL_DETECTADO,     // Sensor inductivo confirmó recipiente metálico
    ESTADO_POSICIONANDO,        // Desplazamiento hacia la estación de llenado
    ESTADO_LLENANDO,            // Banda detenida, válvula abierta dosificando
    ESTADO_REANUDANDO,          // Válvula cerrada, reanudando banda para evacuar
    ESTADO_STOP                 // Parada de emergencia activa (todo actuador apagado)
};

// =============================================================================
// CONFIGURACIÓN DE RED Y SERVIDOR WEB ASÍNCRONO
// =============================================================================
#define WEB_SERVER_PORT         80
#define DEFAULT_AP_SSID         "BandaTransportadora_AP"
#define DEFAULT_AP_PASSWORD     "12345678"

#endif // CONFIG_H
