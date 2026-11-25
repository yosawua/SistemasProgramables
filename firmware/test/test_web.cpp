/**
 * @file test_web.cpp
 * @brief Servidor Web Asíncrono en ESP32 con Modo Access Point (WiFi SoftAP)
 * @author Los Pythones
 *
 * Levanta una red WiFi local ('BandaTransportadora_AP') y sirve una interfaz
 * web interactiva con botones para Iniciar, Detener y un contador de recipientes.
 */

#include <Arduino.h>
#include <WiFi.h>
#include <ESPAsyncWebServer.h>

#if __has_include("secrets.h")
    #include "secrets.h"
#else
    #define WIFI_AP_SSID      "BandaTransportadora_AP"
    #define WIFI_AP_PASSWORD  "12345678"
#endif

#include "pins.h"
#include "config.h"

AsyncWebServer server(WEB_SERVER_PORT);

// Variables de estado del sistema web
volatile bool sistemaActivo = false;
volatile unsigned long contadorRecipientes = 0;

// Código HTML/CSS/JS de la interfaz incrustada
const char INDEX_HTML[] PROGMEM = R"rawliteral(
<!DOCTYPE html>
<html lang="es">
<head>
  <meta charset="UTF-8">
  <meta name="viewport" content="width=device-width, initial-scale=1.0">
  <title>Control de Banda Transportadora</title>
  <style>
    * { box-sizing: border-box; font-family: 'Segoe UI', Tahoma, Geneva, Verdana, sans-serif; }
    body { background-color: #0f172a; color: #f8fafc; display: flex; justify-content: center; align-items: center; min-height: 100vh; margin: 0; padding: 20px; }
    .card { background: #1e293b; border-radius: 16px; padding: 28px; width: 100%; max-width: 440px; box-shadow: 0 10px 25px -5px rgba(0,0,0,0.5); text-align: center; border: 1px solid #334155; }
    h1 { font-size: 22px; margin-bottom: 6px; color: #38bdf8; }
    p.sub { font-size: 13px; color: #94a3b8; margin-top: 0; margin-bottom: 24px; }
    .status-badge { display: inline-block; padding: 8px 18px; border-radius: 9999px; font-weight: 600; font-size: 14px; margin-bottom: 24px; letter-spacing: 0.5px; }
    .status-active { background-color: rgba(34, 197, 94, 0.2); color: #4ade80; border: 1px solid #22c55e; }
    .status-stopped { background-color: rgba(239, 68, 68, 0.2); color: #f87171; border: 1px solid #ef4444; }
    .counter-box { background-color: #0f172a; border-radius: 12px; padding: 18px; margin-bottom: 24px; border: 1px solid #334155; }
    .counter-value { font-size: 42px; font-weight: 800; color: #facc15; }
    .counter-label { font-size: 13px; text-transform: uppercase; color: #94a3b8; letter-spacing: 1px; }
    .btn-group { display: flex; gap: 12px; margin-bottom: 12px; }
    button { flex: 1; padding: 14px; border: none; border-radius: 10px; font-size: 16px; font-weight: bold; cursor: pointer; transition: all 0.2s ease; }
    .btn-start { background-color: #22c55e; color: #ffffff; }
    .btn-start:hover { background-color: #16a34a; transform: translateY(-2px); }
    .btn-stop { background-color: #ef4444; color: #ffffff; }
    .btn-stop:hover { background-color: #dc2626; transform: translateY(-2px); }
    .btn-inc { background-color: #3b82f6; color: white; width: 100%; margin-top: 8px; }
    .btn-inc:hover { background-color: #2563eb; }
  </style>
</head>
<body>
  <div class="card">
    <h1>Banda Transportadora 12V</h1>
    <p class="sub">Panel de Monitoreo y Control Remoto</p>

    <div id="statusBadge" class="status-badge status-stopped">SISTEMA DETENIDO</div>

    <div class="counter-box">
      <div id="contador" class="counter-value">0</div>
      <div class="counter-label">Recipientes Procesados</div>
    </div>

    <div class="btn-group">
      <button class="btn-start" onclick="enviarComando('/iniciar')">▶️ Iniciar</button>
      <button class="btn-stop" onclick="enviarComando('/stop')">🛑 Detener</button>
    </div>
    <button class="btn-inc" onclick="enviarComando('/contar')">➕ Simular Conteo (+1)</button>
  </div>

  <script>
    function actualizarUI(data) {
      document.getElementById('contador').innerText = data.contador;
      const badge = document.getElementById('statusBadge');
      if (data.activo) {
        badge.className = 'status-badge status-active';
        badge.innerText = 'BANDA EN OPERACIÓN';
      } else {
        badge.className = 'status-badge status-stopped';
        badge.innerText = 'SISTEMA DETENIDO';
      }
    }

    function consultarEstado() {
      fetch('/api/estado')
        .then(res => res.json())
        .then(data => actualizarUI(data))
        .catch(err => console.log(err));
    }

    function enviarComando(endpoint) {
      fetch(endpoint, { method: 'POST' })
        .then(res => res.json())
        .then(data => actualizarUI(data))
        .catch(err => console.log(err));
    }

    setInterval(consultarEstado, 1000);
    consultarEstado();
  </script>
</body>
</html>
)rawliteral";

void setup() {
    Serial.begin(115200);
    delay(1000);

    Serial.println("==================================================");
    Serial.println("   PRUEBA DE SOFTWARE: SERVIDOR WEB ASÍNCRONO");
    Serial.println("==================================================");

    Serial.printf("Configurando SoftAP SSID: %s\n", WIFI_AP_SSID);
    WiFi.softAP(WIFI_AP_SSID, WIFI_AP_PASSWORD);

    IPAddress ip = WiFi.softAPIP();
    Serial.printf("✅ Red WiFi iniciada exitosamente.\n");
    Serial.printf("🌐 Dirección IP del Servidor: http://%s\n", ip.toString().c_str());
    Serial.println("Conéctate a la red WiFi desde tu teléfono o PC y abre la IP en tu navegador.");
    Serial.println("--------------------------------------------------");

    // Servir página principal
    server.on("/", HTTP_GET, [](AsyncWebServerRequest *request) {
        request->send_P(200, "text/html", INDEX_HTML);
    });

    // Endpoint API para consultar estado
    server.on("/api/estado", HTTP_GET, [](AsyncWebServerRequest *request) {
        String json = "{\"activo\":" + String(sistemaActivo ? "true" : "false") +
                      ",\"contador\":" + String(contadorRecipientes) + "}";
        request->send(200, "application/json", json);
    });

    // Endpoint para Iniciar
    server.on("/iniciar", HTTP_POST, [](AsyncWebServerRequest *request) {
        sistemaActivo = true;
        Serial.println("🌐 [WEB] Comando recibido: INICIAR SISTEMA");
        String json = "{\"activo\":true,\"contador\":" + String(contadorRecipientes) + "}";
        request->send(200, "application/json", json);
    });

    // Endpoint para Detener
    server.on("/stop", HTTP_POST, [](AsyncWebServerRequest *request) {
        sistemaActivo = false;
        Serial.println("🌐 [WEB] Comando recibido: DETENER SISTEMA");
        String json = "{\"activo\":false,\"contador\":" + String(contadorRecipientes) + "}";
        request->send(200, "application/json", json);
    });

    // Endpoint para incrementar contador (simulación)
    server.on("/contar", HTTP_POST, [](AsyncWebServerRequest *request) {
        contadorRecipientes++;
        Serial.printf("🌐 [WEB] Conteo incrementado a: %lu\n", contadorRecipientes);
        String json = "{\"activo\":" + String(sistemaActivo ? "true" : "false") +
                      ",\"contador\":" + String(contadorRecipientes) + "}";
        request->send(200, "application/json", json);
    });

    server.begin();
    Serial.println("Servidor web iniciado y escuchando peticiones en puerto 80.");
}

void loop() {
    // Al ser un servidor asíncrono (ESPAsyncWebServer), el loop permanece libre
    delay(100);
}
