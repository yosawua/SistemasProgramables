# Guía de Conexiones Eléctricas y Cableado

Este documento detalla la interconexión de cada periférico con el ESP32 DevKit V1.

---

## 1. Mapa General de Pines del ESP32

```
                       +-------------------+
                       |    ESP32 DEVKIT   |
                       |       (30P)       |
           3V3 (Salida)| [ ]           [ ] | VIN (Entrada 5V Reductor)
                   GND | [ ]           [ ] | GND (Masa Común)
    (Botón Stop)  D32  | [ ]           [ ] | D13  (TB6612 AIN2)
   (Opcional TRIG) D33 | [ ]           [ ] | D12  (LIBRE - Strapping)
 (Sensor Inductivo) D34| [ ]           [ ] | D14  (TB6612 AIN1)
      (Sensor IR) D35  | [ ]           [ ] | D27  (TB6612 BIN1)
   (Opcional ECHO) VN  | [ ]           [ ] | D26  (TB6612 STBY)
                 VP    | [ ]           [ ] | D25  (TB6612 PWMA)
                   EN  | [ ]           [ ] | D33  (LIBRE)
                       | [ ]           [ ] | D21  (TFT DC)
      (DFPlayer RX) D16| [ ]           [ ] | TX0  (UART Programación)
      (DFPlayer TX) D17| [ ]           [ ] | RX0  (UART Programación)
            (TFT CS) D5| [ ]           [ ] | D22  (TFT RST)
           (TFT SCK)D18| [ ]           [ ] | D23  (TFT MOSI)
          (TFT MISO)D19| [ ]           [ ] | D19  (TFT MISO)
      (TB6612 PWMB) D2 | [ ]           [ ] | D15  (TB6612 BIN2)
           (TFT LED) D4| [ ]           [ ] | D2   (TB6612 PWMB)
                       +-------------------+
```

---

## 2. Sensor Inductivo NPN (LJ12A3-4-Z/BX)
- **Cable Café:** +12V (Salida elevador step-up)
- **Cable Azul:** GND
- **Cable Negro (Salida NPN Colector Abierto):** Conectar a **GPIO 34**.
- **Resistencia Pull-up:** Colocar una resistencia de 10k entre **GPIO 34** y **3.3V**.
  - *Principio:* Al no haber metal, el transistor interno está abierto y la resistencia fija la entrada en 3.3V (HIGH). Al detectar metal, conmuta a GND (LOW).

---

## 3. Sensor Óptico Infrarrojo (FC-51)
- **VCC:** 3.3V del ESP32
- **GND:** GND
- **OUT:** **GPIO 35** del ESP32

---

## 4. Driver de Motores TB6612FNG
- **VM:** +12V (Elevador)
- **VCC:** 3.3V o 5V (Lógica del driver)
- **GND (Ambos pines GND):** GND común
- **STBY (Standby):** Conectar a **GPIO 26** (HIGH para activar)

### Canal A (Motor DC Banda 12V):
- **PWMA:** Conectar a **GPIO 25** (PWM velocidad)
- **AIN1:** Conectar a **GPIO 14**
- **AIN2:** Conectar a **GPIO 13**
- **AO1 y AO2:** Conectados a las terminales del Motor DC de la banda.

### Canal B (Electroválvula 12V):
- **PWMB:** Conectar a **GPIO 2** (PWM / Habilitación de potencia)
- **BIN1:** Conectar a **GPIO 27**
- **BIN2:** Conectar a **GPIO 15**
- **BO1 y BO2:** Conectados a las dos terminales de la bobina de la electroválvula 12V.

---

## 5. Pantalla TFT 2.8" ILI9341 SPI (240x320)
- **VCC:** 3.3V (o 5V si la placa cuenta con regulador LDO interno integrado)
- **GND:** GND
- **CS:** **GPIO 5**
- **RESET:** **GPIO 22**
- **DC (Data/Command):** **GPIO 21**
- **SDI (MOSI):** **GPIO 23**
- **SCK (Clock):** **GPIO 18**
- **LED (Backlight):** **GPIO 4** (a través de resistor de 100Ω) o directo a 3.3V
- **SDO (MISO):** **GPIO 19**

---

## 6. Reproductor DFPlayer Mini (MP3-TF-16P)
- **VCC:** 5V (desde reductor DC-DC)
- **GND:** GND común
- **TX:** Conectar a **GPIO 16** (RX2 del ESP32)
- **RX:** Conectar a **GPIO 17** (TX2 del ESP32) **intercalando en serie una resistencia de 1 kΩ**.
  - *Propósito:* El DFPlayer opera su lógica a 5V o es sensible a sobrecorrientes en RX; la resistencia de 1k evita transitorios y reduce zumbidos en el audio.
- **SPK_1 y SPK_2:** Conectados al altavoz / bocina de 8Ω.

---

## 7. Botón de Parada de Emergencia (STOP)
- **Terminal 1:** Conectar a **GPIO 32**.
- **Terminal 2:** Conectar a **GND**.
- *Nota:* El microcontrolador activa internamente `pinMode(PIN_BOTON_STOP, INPUT_PULLUP)`. Al pulsarse, lleva el pin a LOW.
