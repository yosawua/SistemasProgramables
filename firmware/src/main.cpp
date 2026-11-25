/**
 * @file main.cpp
 * @brief Firmware principal para banda transportadora con clasificación y llenado
 * @author Los Pythones - Sistemas Programables
 *
 * Máquina de estados no bloqueante con millis():
 * - ESTADO_ESPERANDO
 * - ESTADO_METAL_DETECTADO
 * - ESTADO_POSICIONANDO
 * - ESTADO_LLENANDO
 * - ESTADO_REANUDANDO
 * - ESTADO_STOP
 */

#include <Arduino.h>
#include <SPI.h>
#include <WiFi.h>
#include <ESPAsyncWebServer.h>
#include <Adafruit_GFX.h>
#include <Adafruit_ILI9341.h>
#include <DFRobotDFPlayerMini.h>

#if __has_include("secrets.h")
    #include "secrets.h"
#else
    #define WIFI_AP_SSID      "BandaTransportadora_AP"
    #define WIFI_AP_PASSWORD  "12345678"
#endif

#include "pins.h"
#include "config.h"

// =============================================================================
// OBJETOS Y PERIFÉRICOS
// =============================================================================
Adafruit_ILI9341 tft = Adafruit_ILI9341(PIN_TFT_CS, PIN_TFT_DC, PIN_TFT_RST);
HardwareSerial dfSerial(2);
DFRobotDFPlayerMini dfPlayer;
AsyncWebServer server(WEB_SERVER_PORT);

// =============================================================================
// VARIABLES GLOBALES DE ESTADO Y TEMPORIZACIÓN (NON-BLOCKING)
// =============================================================================
EstadoSistema estadoActual = ESTADO_ESPERANDO;
EstadoSistema estadoAnterior = ESTADO_STOP; // Forzar redibujado inicial

unsigned long tiempoInicioLlenado = 0;
unsigned long tiempoInicioReanudacion = 0;
unsigned long ultimoRefrescoTFT = 0;
unsigned long contadorRecipientes = 0;

// Variables de antirrebote y lectura de botón
int ultimoBotonLectura = HIGH;
int estadoBotonEstable = HIGH;
unsigned long ultimoReboteBoton = 0;

bool dfPlayerListo = false;

// =============================================================================
// FUNCIONES DE CONTROL DE ACTUADORES
// =============================================================================
void fijarMotorBanda(int velocidad, bool avanzar = true) {
    if (velocidad <= 0) {
        digitalWrite(PIN_MOTOR_AIN1, LOW);
        digitalWrite(PIN_MOTOR_AIN2, LOW);
        ledcWrite(PWM_MOTOR_CANAL, 0);
    } else {
        if (avanzar) {
            digitalWrite(PIN_MOTOR_AIN1, HIGH);
            digitalWrite(PIN_MOTOR_AIN2, LOW);
        } else {
            digitalWrite(PIN_MOTOR_AIN1, LOW);
            digitalWrite(PIN_MOTOR_AIN2, HIGH);
        }
        ledcWrite(PWM_MOTOR_CANAL, constrain(velocidad, 0, 255));
    }
}

void controlarValvula(bool abrir) {
    if (abrir) {
        digitalWrite(PIN_VALVULA_BIN1, HIGH);
        digitalWrite(PIN_VALVULA_BIN2, LOW);
        ledcWrite(PWM_VALVULA_CANAL, POTENCIA_VALVULA_ON);
    } else {
        digitalWrite(PIN_VALVULA_BIN1, LOW);
        digitalWrite(PIN_VALVULA_BIN2, LOW);
        ledcWrite(PWM_VALVULA_CANAL, POTENCIA_VALVULA_OFF);
    }
}

void reproducirPista(int pista) {
    if (dfPlayerListo) {
        dfPlayer.play(pista);
    }
}

// =============================================================================
// INTERFAZ GRÁFICA TFT ILI9341
// =============================================================================
void dibujarEstructuraBaseTFT() {
    tft.fillScreen(ILI9341_BLACK);

    // Cabecera
    tft.fillRect(0, 0, 320, 36, ILI9341_NAVY);
    tft.drawFastHLine(0, 36, 320, ILI9341_CYAN);
    tft.setTextColor(ILI9341_WHITE);
    tft.setTextSize(2);
    tft.setCursor(15, 10);
    tft.print("BANDA CLASIFICADORA");

    // Tarjeta de Estado
    tft.drawRoundRect(10, 44, 300, 75, 8, ILI9341_DARKGREY);

    // Tarjetas de Sensores
    tft.drawRoundRect(10, 126, 145, 60, 6, ILI9341_DARKGREY);
    tft.setTextColor(ILI9341_LIGHTGREY);
    tft.setTextSize(1);
    tft.setCursor(20, 134);
    tft.print("INDUCTIVO (LJ12A3)");

    tft.drawRoundRect(165, 126, 145, 60, 6, ILI9341_DARKGREY);
    tft.setTextColor(ILI9341_LIGHTGREY);
    tft.setTextSize(1);
    tft.setCursor(180, 134);
    tft.print("OPTICO IR (FC-51)");

    // Contador y Pie
    tft.drawFastHLine(0, 195, 320, ILI9341_DARKGREY);
    tft.setTextColor(ILI9341_YELLOW);
    tft.setTextSize(2);
    tft.setCursor(15, 206);
    tft.print("PROCESADOS: ");
}

void actualizarPantallaTFT(bool cambioEstado) {
    if (cambioEstado) {
        // Limpiar área de estado
        tft.fillRect(15, 50, 290, 62, ILI9341_BLACK);
        tft.setCursor(20, 56);
        tft.setTextSize(1);
        tft.setTextColor(ILI9341_YELLOW);
        tft.print("ESTADO DEL SISTEMA:");

        tft.setCursor(20, 74);
        tft.setTextSize(2);

        switch (estadoActual) {
            case ESTADO_ESPERANDO:
                tft.setTextColor(ILI9341_GREEN);
                tft.print("ESPERANDO RECIPIENTE");
                break;
            case ESTADO_METAL_DETECTADO:
                tft.setTextColor(ILI9341_CYAN);
                tft.print("METAL DETECTADO!");
                break;
            case ESTADO_POSICIONANDO:
                tft.setTextColor(ILI9341_ORANGE);
                tft.print("POSICIONANDO...");
                break;
            case ESTADO_LLENANDO:
                tft.setTextColor(ILI9341_MAGENTA);
                tft.print("LLENANDO RECIPIENTE");
                break;
            case ESTADO_REANUDANDO:
                tft.setTextColor(ILI9341_BLUE);
                tft.print("REANUDANDO BANDA");
                break;
            case ESTADO_STOP:
                tft.setTextColor(ILI9341_RED);
                tft.print("SISTEMA DETENIDO");
                break;
        }

        // Actualizar número de contador
        tft.fillRect(170, 204, 140, 26, ILI9341_BLACK);
        tft.setTextColor(ILI9341_WHITE);
        tft.setTextSize(2);
        tft.setCursor(170, 206);
        tft.print(contadorRecipientes);
    }

    // Refrescar lectura de sensores
    bool metal = (digitalRead(PIN_SENSOR_INDUCTIVO) == LOW);
    bool ir = (digitalRead(PIN_SENSOR_IR) == LOW);

    // Sensor Inductivo
    tft.fillRect(20, 150, 125, 26, ILI9341_BLACK);
    tft.setCursor(25, 154);
    tft.setTextSize(2);
    if (metal) {
        tft.setTextColor(ILI9341_GREEN);
        tft.print("METAL");
    } else {
        tft.setTextColor(ILI9341_LIGHTGREY);
        tft.print("LIBRE");
    }

    // Sensor IR
    tft.fillRect(175, 150, 125, 26, ILI9341_BLACK);
    tft.setCursor(180, 154);
    tft.setTextSize(2);
    if (ir) {
        tft.setTextColor(ILI9341_RED);
        tft.print("DETECTADO");
    } else {
        tft.setTextColor(ILI9341_LIGHTGREY);
        tft.print("LIBRE");
    }

    // Si está llenando, dibujar barra de progreso de tiempo
    if (estadoActual == ESTADO_LLENANDO) {
        unsigned long transcurrido = millis() - tiempoInicioLlenado;
        int anchoProgreso = map(constrain(transcurrido, 0, TIEMPO_LLENADO_MS), 0, TIEMPO_LLENADO_MS, 0, 280);
        tft.drawRect(20, 100, 280, 8, ILI9341_WHITE);
        tft.fillRect(20, 100, anchoProgreso, 8, ILI9341_CYAN);
    }
}

// =============================================================================
// LECTURA DE BOTÓN STOP CON ANTIRREBOTE NO BLOQUEANTE
// =============================================================================
void verificarBotonStop() {
    int lectura = digitalRead(PIN_BOTON_STOP);

    if (lectura != ultimoBotonLectura) {
        ultimoReboteBoton = millis();
    }

    if ((millis() - ultimoReboteBoton) > DEBOUNCE_BOTON_MS) {
        if (lectura != estadoBotonEstable) {
            estadoBotonEstable = lectura;
            // Presionado (LOW debido a INPUT_PULLUP)
            if (estadoBotonEstable == LOW) {
                if (estadoActual != ESTADO_STOP) {
                    Serial.println("🛑 [BOTON STOP] Parada de emergencia solicitada.");
                    estadoActual = ESTADO_STOP;
                    fijarMotorBanda(VELOCIDAD_BANDA_PARADA);
                    controlarValvula(false);
                    reproducirPista(AUDIO_SISTEMA_DETENIDO);
                } else {
                    Serial.println("▶️  [BOTON STOP] Reanudando operación normal.");
                    estadoActual = ESTADO_ESPERANDO;
                    fijarMotorBanda(VELOCIDAD_BANDA_NOMINAL);
                }
            }
        }
    }
    ultimoBotonLectura = lectura;
}

// =============================================================================
// SERVIDOR WEB ASÍNCRONO
// =============================================================================
const char HTML_INDEX[] PROGMEM = R"rawliteral(
<!DOCTYPE html>
<html lang="es">
<head>
  <meta charset="UTF-8">
  <meta name="viewport" content="width=device-width, initial-scale=1.0">
  <title>Control Banda Embebida</title>
  <style>
    body { font-family: sans-serif; background: #0f172a; color: white; text-align: center; padding: 20px; }
    .box { background: #1e293b; max-width: 420px; margin: 0 auto; padding: 24px; border-radius: 12px; }
    .val { font-size: 38px; color: #38bdf8; font-weight: bold; }
    .btn { padding: 12px 24px; margin: 8px; border: none; border-radius: 8px; font-weight: bold; cursor: pointer; }
    .btn-green { background: #22c55e; color: white; }
    .btn-red { background: #ef4444; color: white; }
  </style>
</head>
<body>
  <div class="box">
    <h2>Banda Transportadora ESP32</h2>
    <p>Estado actual: <span id="st" style="font-weight:bold;">--</span></p>
    <div class="val" id="cnt">0</div>
    <p>Recipientes Procesados</p>
    <button class="btn btn-green" onclick="fetch('/iniciar',{method:'POST'})">▶️ Iniciar</button>
    <button class="btn btn-red" onclick="fetch('/stop',{method:'POST'})">🛑 Detener (Stop)</button>
  </div>
  <script>
    setInterval(() => {
      fetch('/api/estado').then(r => r.json()).then(d => {
        document.getElementById('cnt').innerText = d.contador;
        document.getElementById('st').innerText = d.estado;
      });
    }, 1000);
  </script>
</body>
</html>
)rawliteral";

void iniciarServidorWeb() {
    WiFi.softAP(WIFI_AP_SSID, WIFI_AP_PASSWORD);
    Serial.printf("🌐 SoftAP SSID: %s | IP: http://%s\n", WIFI_AP_SSID, WiFi.softAPIP().toString().c_str());

    server.on("/", HTTP_GET, [](AsyncWebServerRequest *request) {
        request->send_P(200, "text/html", HTML_INDEX);
    });

    server.on("/api/estado", HTTP_GET, [](AsyncWebServerRequest *request) {
        const char* nombresEstados[] = {"ESPERANDO", "METAL_DETECTADO", "POSICIONANDO", "LLENANDO", "REANUDANDO", "STOP"};
        String json = "{\"estado\":\"" + String(nombresEstados[estadoActual]) + "\",\"contador\":" + String(contadorRecipientes) + "}";
        request->send(200, "application/json", json);
    });

    server.on("/iniciar", HTTP_POST, [](AsyncWebServerRequest *request) {
        if (estadoActual == ESTADO_STOP) {
            estadoActual = ESTADO_ESPERANDO;
            fijarMotorBanda(VELOCIDAD_BANDA_NOMINAL);
        }
        request->send(200, "text/plain", "OK");
    });

    server.on("/stop", HTTP_POST, [](AsyncWebServerRequest *request) {
        estadoActual = ESTADO_STOP;
        fijarMotorBanda(VELOCIDAD_BANDA_PARADA);
        controlarValvula(false);
        reproducirPista(AUDIO_SISTEMA_DETENIDO);
        request->send(200, "text/plain", "STOPPED");
    });

    server.begin();
}

// =============================================================================
// SETUP
// =============================================================================
void setup() {
    Serial.begin(115200);
    delay(500);

    Serial.println("\n==================================================");
    Serial.println("  SISTEMA EMBEBIDO BANDA TRANSPORTADORA 12V");
    Serial.println("  Equipo: Los Pythones | Framework: Arduino (ESP32)");
    Serial.println("==================================================");

    // 1. Configuración de Entradas Digitales
    pinMode(PIN_SENSOR_INDUCTIVO, INPUT); // Requiere pull-up externo a 3.3V
    pinMode(PIN_SENSOR_IR, INPUT);
    pinMode(PIN_BOTON_STOP, INPUT_PULLUP);

    // 2. Configuración de Driver TB6612FNG
    pinMode(PIN_TB6612_STBY, OUTPUT);
    pinMode(PIN_MOTOR_AIN1, OUTPUT);
    pinMode(PIN_MOTOR_AIN2, OUTPUT);
    pinMode(PIN_VALVULA_BIN1, OUTPUT);
    pinMode(PIN_VALVULA_BIN2, OUTPUT);

    // PWM para motor y válvula
    ledcSetup(PWM_MOTOR_CANAL, PWM_MOTOR_FREQ, PWM_MOTOR_RES);
    ledcAttachPin(PIN_MOTOR_PWMA, PWM_MOTOR_CANAL);

    ledcSetup(PWM_VALVULA_CANAL, PWM_VALVULA_FREQ, PWM_VALVULA_RES);
    ledcAttachPin(PIN_VALVULA_PWMB, PWM_VALVULA_CANAL);

    digitalWrite(PIN_TB6612_STBY, HIGH);
    fijarMotorBanda(VELOCIDAD_BANDA_PARADA);
    controlarValvula(false);

    // 3. Inicialización de Pantalla TFT
    if (PIN_TFT_LED >= 0) {
        pinMode(PIN_TFT_LED, OUTPUT);
        digitalWrite(PIN_TFT_LED, HIGH);
    }
    tft.begin();
    tft.setRotation(TFT_ROTATION);
    dibujarEstructuraBaseTFT();

    // 4. Inicialización de DFPlayer Mini (Serial2: RX=16, TX=17 con R=1k)
    dfSerial.begin(9600, SERIAL_8N1, PIN_DFPLAYER_RX, PIN_DFPLAYER_TX);
    if (dfPlayer.begin(dfSerial, true, true)) {
        dfPlayerListo = true;
        dfPlayer.volume(DFPLAYER_VOLUMEN);
        Serial.println("✅ DFPlayer Mini inicializado correctamente.");
    } else {
        Serial.println("⚠️  DFPlayer Mini no respondió (continuando en modo silencioso).");
    }

    // 5. Iniciar Servidor Web Asíncrono
    iniciarServidorWeb();

    // Iniciar banda transportadora en estado de espera
    estadoActual = ESTADO_ESPERANDO;
    fijarMotorBanda(VELOCIDAD_BANDA_NOMINAL);
    actualizarPantallaTFT(true);
    Serial.println("Sistema iniciado en ESTADO_ESPERANDO. Banda en marcha.");
}

// =============================================================================
// BUCLE PRINCIPAL (MÁQUINA DE ESTADOS NO BLOQUEANTE CON MILLIS)
// =============================================================================
void loop() {
    unsigned long ahora = millis();

    // Monitoreo permanente de parada de emergencia
    verificarBotonStop();

    bool cambioDeEstado = (estadoActual != estadoAnterior);
    if (cambioDeEstado) {
        estadoAnterior = estadoActual;
        actualizarPantallaTFT(true);
    } else if (ahora - ultimoRefrescoTFT > 250) {
        ultimoRefrescoTFT = ahora;
        actualizarPantallaTFT(false);
    }

    // Transiciones y acciones de la máquina de estados
    switch (estadoActual) {
        case ESTADO_ESPERANDO: {
            fijarMotorBanda(VELOCIDAD_BANDA_NOMINAL);
            controlarValvula(false);

            // Detección de metal por sensor inductivo (activo en LOW)
            if (digitalRead(PIN_SENSOR_INDUCTIVO) == LOW) {
                Serial.println("🔍 [INDUCTIVO] Metal detectado -> Transición a ESTADO_METAL_DETECTADO");
                reproducirPista(AUDIO_METAL_DETECTADO);
                estadoActual = ESTADO_METAL_DETECTADO;
            }
            break;
        }

        case ESTADO_METAL_DETECTADO: {
            // Breve transición al estado de posicionamiento
            estadoActual = ESTADO_POSICIONANDO;
            fijarMotorBanda(VELOCIDAD_BANDA_LENTA);
            break;
        }

        case ESTADO_POSICIONANDO: {
            fijarMotorBanda(VELOCIDAD_BANDA_LENTA);

            // Sensor IR detecta llegada a la estación de llenado (activo en LOW)
            if (digitalRead(PIN_SENSOR_IR) == LOW) {
                Serial.println("🎯 [IR] Recipiente posicionado -> Transición a ESTADO_LLENANDO");
                fijarMotorBanda(VELOCIDAD_BANDA_PARADA);
                controlarValvula(true);
                tiempoInicioLlenado = millis();
                reproducirPista(AUDIO_LLENANDO);
                estadoActual = ESTADO_LLENANDO;
            }
            break;
        }

        case ESTADO_LLENANDO: {
            fijarMotorBanda(VELOCIDAD_BANDA_PARADA);
            controlarValvula(true);

            // Temporizador no bloqueante de llenado
            if (ahora - tiempoInicioLlenado >= TIEMPO_LLENADO_MS) {
                Serial.println("💧 [VÁLVULA] Llenado completado -> Transición a ESTADO_REANUDANDO");
                controlarValvula(false);
                contadorRecipientes++;
                reproducirPista(AUDIO_LLENADO_COMPLETO);
                tiempoInicioReanudacion = millis();
                fijarMotorBanda(VELOCIDAD_BANDA_NOMINAL);
                estadoActual = ESTADO_REANUDANDO;
            }
            break;
        }

        case ESTADO_REANUDANDO: {
            controlarValvula(false);
            fijarMotorBanda(VELOCIDAD_BANDA_NOMINAL);

            // Permitir que el recipiente abandone el campo del sensor IR antes de volver a esperar
            if ((ahora - tiempoInicioReanudacion >= TIEMPO_DESPEJE_MS) && (digitalRead(PIN_SENSOR_IR) == HIGH)) {
                Serial.println("🔄 [BANDA] Zona despejada -> Transición a ESTADO_ESPERANDO");
                estadoActual = ESTADO_ESPERANDO;
            }
            break;
        }

        case ESTADO_STOP: {
            fijarMotorBanda(VELOCIDAD_BANDA_PARADA);
            controlarValvula(false);
            // Permanece en STOP hasta intervención por botón o web
            break;
        }
    }
}
