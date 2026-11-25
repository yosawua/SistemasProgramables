#pragma once

/**
 * @file secrets.example.h
 * @brief Plantilla de credenciales y secretos para compilación de firmware
 * @author Los Pythones
 *
 * Para utilizar tus valores reales:
 * 1. Define tus credenciales en el archivo .env en la raíz del repositorio.
 * 2. Ejecuta 'python tools/gen_secrets.py' para generar 'secrets.h' automáticamente.
 * El archivo 'secrets.h' resultante se encuentra ignorado por git por seguridad.
 */

#ifndef SECRETS_H
#define SECRETS_H

// Identificador del Punto de Acceso WiFi creado por el ESP32
#define WIFI_AP_SSID      "BandaTransportadora_AP"

// Contraseña de seguridad WPA2 (debe contener al menos 8 caracteres)
#define WIFI_AP_PASSWORD  "12345678"

// Credenciales para la API de ElevenLabs (utilizadas únicamente en scripts de Python)
#define ELEVENLABS_API_KEY ""
#define ELEVENLABS_VOICE_ID ""

#endif // SECRETS_H
