# Banda Transportadora Clasificadora y Dosificadora con ESP32

Proyecto escolar de sistemas embebidos desarrollado para la materia de **Sistemas Programables** (Unidad Final).  
**Equipo:** Los Pythones

---

## 📋 Descripción del Proyecto

El sistema consiste en una banda transportadora industrial a escala automatizada con un microcontrolador **ESP32 (DevKit V1)**. El proceso operativo realiza las siguientes etapas:

1. **Detección de Material:** Un sensor de proximidad inductivo NPN (`LJ12A3-4-Z/BX`) detecta si el recipiente ingresado es metálico.
2. **Locución y Notificación:** Si se detecta metal, el reproductor de audio `DFPlayer Mini` emite un aviso por voz (*"Recipiente metálico detectado"*) y se actualiza el estado en la pantalla TFT 2.8".
3. **Posicionamiento:** La banda avanza a velocidad controlada hasta que un sensor infrarrojo óptico (`FC-51`) detecta el recipiente exactamente debajo de la estación de dosificación.
4. **Dosificación / Llenado:** La banda se detiene por completo, se abre una electroválvula de 12V mediante el canal B del driver `TB6612FNG` durante un intervalo programado (ej. 3 segundos), con mensaje sonoro (*"Llenando"*) y barra de progreso gráfica en pantalla.
5. **Reanudación y Conteo:** Se cierra la electroválvula, se reproduce el audio de confirmación (*"Llenado completo"*), se incrementa el contador de unidades procesadas y se reanuda la marcha de la banda para desalojar el recipiente.
6. **Seguridad y Control Remoto:** Botón físico de parada de emergencia (`STOP`) y servidor web asíncrono vía WiFi SoftAP para monitoreo y control en tiempo real desde smartphones o navegadores web.

---

## 🛠️ Lista de Componentes de Hardware

- **Microcontrolador:** ESP32 DevKit V1 (30 pines, micro-USB).
- **Driver de Potencia:** TB6612FNG (Doble puente H MOSFET).
  - *Canal A:* Motorreductor DC 12V para tracción de la banda transportadora.
  - *Canal B:* Electroválvula de 12V DC (solenoide normalmente cerrado).
- **Sensor Inductivo de Proximidad:** LJ12A3-4-Z/BX (Alimentación 6-36V, salida NPN normalmente abierta con pull-up externo a 3.3V).
- **Sensor de Presencia Óptico:** Módulo IR FC-51 (salida digital LM393).
- **Pantalla Gráfica:** TFT LCD 2.8" ILI9341 (Resolución 240x320 píxeles, comunicación SPI hardware VSPI).
- **Módulo de Audio:** DFPlayer Mini (clon MP3-TF-16P con ranura MicroSD).
- **Bocina / Altavoz:** 8 Ω / 2W - 3W.
- **Pulsador de Emergencia:** Botón normalmente abierto (STOP).
- **Alimentación Eléctrica Portátil:**
  - Batería LiPo 2S (7.4V nominal).
  - Convertidor elevador DC-DC Step-Up (salida ajustada a 12V para motor, válvula e inductivo).
  - Convertidor reductor DC-DC Step-Down (salida ajustada a 5V para ESP32 VIN y DFPlayer).
- **Componentes Pasivos:** Resistencia de 1 kΩ (protección en línea TX ESP32 -> RX DFPlayer) y resistencia de 10 kΩ (pull-up para sensor inductivo).

---

## 📌 Mapa Completo de Pines (ESP32 DevKit V1)

El mapa de pines fue cuidadosamente seleccionado respetando las restricciones del ESP32:
- **GPIO 34-39:** Entradas puras sin resistencias pull-up internas (usadas para sensores con pull-up externo o salidas lógicas).
- **GPIO 0, 12:** Libres para evitar problemas con modos de arranque (strapping pins) y voltajes de memoria flash.
- **VSPI (Pines por defecto):** Empleados para la comunicación SPI de alta velocidad con el display ILI9341.
- **Serial2 (UART2):** Empleado para la comunicación con el DFPlayer Mini.

| Módulo / Periférico | Pin del Módulo | GPIO ESP32 | Tipo | Notas Eléctricas / Función |
| :--- | :--- | :---: | :---: | :--- |
| **Sensor Inductivo** | Negro (NPN OUT) | **GPIO 34** | Entrada | Requiere resistencia PULL-UP de 10k a 3.3V |
| **Sensor IR FC-51** | OUT | **GPIO 35** | Entrada | Nivel LOW al detectar recipiente |
| **Botón STOP** | Terminal NA | **GPIO 32** | Entrada | Configurado con `INPUT_PULLUP` interno (LOW al pulsar) |
| **Ultrasónico (Opc.)** | TRIG | **GPIO 33** | Salida | Pulso de disparo de medición (opcional) |
| **Ultrasónico (Opc.)** | ECHO | **GPIO 36 (VP)**| Entrada | Entrada analógica/digital pura (usar divisor de voltaje) |
| **TB6612FNG** | STBY | **GPIO 26** | Salida | HIGH = Driver habilitado |
| **TB6612 (Motor Banda)**| PWMA | **GPIO 25** | Salida PWM | Canal LEDC 0 (frecuencia 20 kHz, 8 bits) |
| **TB6612 (Motor Banda)**| AIN1 | **GPIO 14** | Salida | Dirección de giro 1 |
| **TB6612 (Motor Banda)**| AIN2 | **GPIO 13** | Salida | Dirección de giro 2 |
| **TB6612 (Válvula)** | PWMB | **GPIO 2** | Salida PWM | Habilitación de potencia válvula (Canal LEDC 1) |
| **TB6612 (Válvula)** | BIN1 | **GPIO 27** | Salida | Activación solenoide (HIGH = abierta) |
| **TB6612 (Válvula)** | BIN2 | **GPIO 15** | Salida | Retorno solenoide (LOW) |
| **DFPlayer Mini** | TX | **GPIO 16** | Entrada UART | RX2 del ESP32 |
| **DFPlayer Mini** | RX | **GPIO 17** | Salida UART | **TX2 del ESP32 -> En serie con R = 1 kΩ** |
| **TFT ILI9341** | SCK / CLK | **GPIO 18** | Salida SPI | Reloj VSPI |
| **TFT ILI9341** | MISO / SDO | **GPIO 19** | Entrada SPI | Retorno de datos VSPI |
| **TFT ILI9341** | MOSI / SDI | **GPIO 23** | Salida SPI | Datos de salida VSPI |
| **TFT ILI9341** | CS | **GPIO 5** | Salida SPI | Chip Select de pantalla |
| **TFT ILI9341** | DC / RS | **GPIO 21** | Salida | Selección Dato / Comando |
| **TFT ILI9341** | RESET | **GPIO 22** | Salida | Reset por hardware de pantalla |
| **TFT ILI9341** | LED / BL | **GPIO 4** | Salida | Control de retroiluminación (Backlight) |

---

## ⚙️ Configuración del Entorno y Variables de Entorno (.env)

### 1. Clonar el repositorio y preparar el entorno virtual
En Windows, puedes ejecutar el script automatizado:
```cmd
setup.bat
```
O de forma manual en PowerShell / Bash:
```bash
# 1. Crear y activar el entorno virtual
python -m venv .venv
.\.venv\Scripts\activate   # En Linux/Mac: source .venv/bin/activate

# 2. Instalar dependencias
pip install -r requirements.txt
```

### 2. Configurar `.env`
Copia el archivo de plantilla `.env.example` a `.env`:
```bash
cp .env.example .env
```
Edita `.env` con un editor de texto y define tus credenciales:
```env
# Credenciales de ElevenLabs para síntesis de voz
ELEVENLABS_API_KEY=tu_api_key_de_elevenlabs
ELEVENLABS_VOICE_ID=tu_voice_id_preferido

# Red WiFi generada por el ESP32 (SoftAP)
WIFI_AP_SSID=BandaTransportadora_AP
WIFI_AP_PASSWORD=password123   # Mínimo 8 caracteres (seguridad WPA2)
```

### 3. Validar la configuración con `check_env.py`
Ejecuta el script de diagnóstico para verificar que las credenciales sean válidas:
```bash
python tools/check_env.py
```
El script comprobará la presencia de las variables, validará que la contraseña WiFi tenga al menos 8 caracteres y realizará una comprobación de autenticación contra la API de ElevenLabs (sin revelar jamás la llave en pantalla).

### 4. Generar la cabecera `secrets.h` para el firmware
```bash
python tools/gen_secrets.py
```
Este comando genera `firmware/include/secrets.h` (ignorado por Git) para que el firmware del ESP32 pueda compilar con tus credenciales.

### 5. Generar las pistas de audio para el DFPlayer Mini
```bash
python tools/generate_audio.py
```
Este script genera los archivos MP3 nombrados `0001.mp3`, `0002.mp3`, etc., en la carpeta `/audio`. Si un archivo ya existe, no se vuelve a generar para ahorrar cuota de la API. Copia estos archivos en la raíz de una memoria MicroSD formateada en FAT32 e insértala en el DFPlayer Mini.

---

## 🧪 Pruebas Unitarias de Hardware (PlatformIO)

Cada módulo de hardware dispone de un entorno de prueba independiente con salida clara por el monitor Serial a 115200 baudios.

Para compilar o subir cada prueba de forma independiente:

```bash
# Entrar a la carpeta del firmware
cd firmware

# 1. Prueba de Sensor Inductivo (detecta metal en GPIO 34)
pio run -e test_inductivo -t upload
pio device monitor

# 2. Prueba de Sensor Infrarrojo (detecta obstáculo en GPIO 35)
pio run -e test_ir -t upload
pio device monitor

# 3. Prueba de Motor DC (TB6612 Canal A: acelera, frena, reversa con PWM)
pio run -e test_motor -t upload
pio device monitor

# 4. Prueba de Electroválvula (TB6612 Canal B: abre y cierra 2s)
pio run -e test_valvula -t upload
pio device monitor

# 5. Prueba de Pantalla TFT ILI9341 (dibuja colores, textos y cuadros)
pio run -e test_tft -t upload
pio device monitor

# 6. Prueba de DFPlayer Mini (reproduce 0001.mp3 por Serial2)
pio run -e test_dfplayer -t upload
pio device monitor

# 7. Prueba de Servidor Web y SoftAP WiFi (levanta AP y página de control)
pio run -e test_web -t upload
pio device monitor
```

---

## 🚀 Compilación y Carga del Firmware Principal

Para compilar y cargar el firmware completo con la máquina de estados no bloqueante:

```bash
cd firmware
pio run -e main -t upload
pio device monitor
```

Una vez cargado el firmware:
1. Conéctate a la red WiFi generada: `BandaTransportadora_AP` con la contraseña configurada en tu `.env`.
2. Abre tu navegador en la dirección `http://192.168.4.1` para ver el panel de control y conteo en tiempo real.
3. Observa en la pantalla TFT 2.8" el estado dinámico del sistema y el número de recipientes procesados.

---

## 📂 Estructura del Repositorio

```
.
├── .env.example              # Plantilla de variables de entorno
├── .gitignore                # Reglas de exclusión de git (secretos, compilados, audio)
├── Licence                   # Licencia de uso educativo
├── README.md                 # Documentación técnica completa
├── requirements.txt          # Dependencias de Python (requests, python-dotenv, platformio)
├── setup.bat                 # Script de instalación automática en Windows
├── setup.ps1                 # Script de instalación en PowerShell
│
├── audio/                    # Carpeta de locuciones de voz (0001.mp3 a 0005.mp3)
│   └── README.md             # Guía de pistas y formato de la tarjeta MicroSD
│
├── docs/                     # Documentación de ingeniería y diseño
│   ├── BITACORA.md           # Bitácora de desarrollo y registro de pruebas
│   └── MAQUINA_ESTADOS.md    # Diagrama de estados Mermaid y tabla de eventos
│
├── firmware/                 # Proyecto PlatformIO (Framework Arduino, ESP32)
│   ├── platformio.ini        # Configuración de entornos de producción y pruebas
│   ├── include/
│   │   ├── config.h          # Constantes operativas, frecuencias PWM y timeouts
│   │   ├── pins.h            # Asignación y documentación de pines de hardware
│   │   └── secrets.example.h # Plantilla de cabecera de credenciales
│   ├── src/
│   │   └── main.cpp          # Máquina de estados no bloqueante con millis()
│   └── test/
│       ├── test_dfplayer.cpp # Test unitario DFPlayer Mini
│       ├── test_inductivo.cpp# Test unitario Sensor Inductivo NPN
│       ├── test_ir.cpp       # Test unitario Sensor Infrarrojo FC-51
│       ├── test_motor.cpp    # Test unitario Motor DC (TB6612 Canal A)
│       ├── test_tft.cpp      # Test unitario Pantalla TFT ILI9341
│       ├── test_valvula.cpp  # Test unitario Electroválvula (TB6612 Canal B)
│       └── test_web.cpp      # Test unitario Servidor Web Asíncrono
│
├── hardware/                 # Información de diseño físico y esquemático
│   ├── BOM.md                # Lista de materiales y características eléctricas
│   └── CONEXIONES.md         # Diagrama pin a pin y consideraciones de cableado
│
└── tools/                    # Herramientas de automatización en Python
    ├── check_env.py          # Verificador de credenciales y conexión ElevenLabs
    ├── gen_secrets.py        # Generador automático de include/secrets.h
    └── generate_audio.py     # Generador de locuciones MP3 con ElevenLabs TTS
```

---

## 👥 Equipo de Trabajo
- **Proyecto:** Sistemas Programables (Unidad Final)
- **Equipo:** Los Pythones
- **Año:** 2026
