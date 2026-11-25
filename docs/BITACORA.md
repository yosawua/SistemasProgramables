# Bitácora de Desarrollo del Proyecto

**Proyecto:** Banda Transportadora Clasificadora y Dosificadora con ESP32  
**Materia:** Sistemas Programables | **Equipo:** Los Pythones

---

## Registro de Actividades y Configuración

### 1. Definición de Arquitectura y Mapa de Pines
- Análisis de la distribución de GPIOs del ESP32 DevKit V1 para evitar conflictos con strapping pins (GPIO 0 y GPIO 12 dejados libres).
- Asignación de bus VSPI por hardware para la pantalla TFT 2.8" ILI9341 (SCK=18, MISO=19, MOSI=23, CS=5).
- Asignación de UART2 (Serial2) para el módulo DFPlayer Mini con resistencia de protección de 1kΩ.
- Separación de canales en el driver TB6612FNG: Canal A para tracción de banda y Canal B para control de electroválvula.
- Acondicionamiento del sensor inductivo NPN LJ12A3-4-Z/BX mediante resistencia externa de 10kΩ a 3.3V hacia GPIO 34.

### 2. Estructura de Seguridad y Gestión de Secretos
- Creación de `.env.example` con plantilla para API keys de ElevenLabs y credenciales de red WiFi SoftAP.
- Configuración de `.gitignore` para salvaguardar credenciales, archivos compilados, entornos virtuales y binarios de audio.
- Desarrollo del script `tools/gen_secrets.py` para generación automatizada de `firmware/include/secrets.h`.
- Desarrollo del validador `tools/check_env.py` con comprobación de longitud WPA2 y autenticación remota en ElevenLabs.

### 3. Síntesis de Locuciones de Voz
- Implementación de `tools/generate_audio.py` con el motor multilingüe de ElevenLabs.
- Asignación de 5 pistas de estado numeradas según el estándar de DFPlayer Mini (`0001.mp3` a `0005.mp3`).

### 4. Entornos de Diagnóstico y Pruebas Unitarias de Hardware
- Implementación de 7 bancos de prueba independientes en PlatformIO:
  1. `test_inductivo`: Lectura digital de presencia metálica.
  2. `test_ir`: Lectura de detección de posición en estación de llenado.
  3. `test_motor`: Comprobación de aceleración PWM, avance y reversa en Canal A de TB6612.
  4. `test_valvula`: Ciclos de apertura y cierre de solenoide 12V en Canal B de TB6612.
  5. `test_tft`: Despliegue gráfico de paletas de color, geometría y tipografía en ILI9341.
  6. `test_dfplayer`: Verificación de comandos UART y reproducción de pista de prueba.
  7. `test_web`: Servidor web asíncrono con panel de control y contador en tiempo real.

### 5. Firmware Principal del Sistema
- Construcción de máquina de estados finitos no bloqueante basada en `millis()` en `src/main.cpp`.
- Integración de interfaz gráfica local en TFT y control remoto concurrente por WiFi.
