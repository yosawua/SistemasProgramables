# Lista de Materiales (Bill of Materials - BOM)

**Proyecto:** Banda Transportadora Clasificadora y Dosificadora  
**Materia:** Sistemas Programables | **Equipo:** Los Pythones

---

| Item | Componente | Modelo / Especificación | Voltaje Operativo | Función en el Sistema | Pines ESP32 / Conexión |
| :---: | :--- | :--- | :---: | :--- | :--- |
| **1** | Microcontrolador | ESP32 DevKit V1 (30 pines) | 3.3V (Lógica) / 5V (VIN) | Controlador principal del sistema embebido | Placa base |
| **2** | Driver Dual de Potencia | TB6612FNG (Módulo puente H) | VM: 12V / VCC: 3.3V-5V | Control de motor de banda y electroválvula | STBY: 26, PWMA: 25, AIN1: 14, AIN2: 13, PWMB: 2, BIN1: 27, BIN2: 15 |
| **3** | Motor DC Banda | Motorreductor DC 12V | 12V DC (Canal A) | Tracción y arrastre de la cinta transportadora | Salidas MOTOR A (TB6612) |
| **4** | Electroválvula | Solenoide 12V normalmente cerrada | 12V DC (Canal B) | Dosificación controlada de líquido | Salidas MOTOR B (TB6612) |
| **5** | Sensor Inductivo | LJ12A3-4-Z/BX (Salida NPN N.O.) | 6V a 36V DC (usar 12V) | Detección de presencia de metales | Café: +12V, Azul: GND, Negro: GPIO 34 (con pull-up 10k a 3.3V) |
| **6** | Sensor Óptico Infrarrojo | FC-51 (Sensor de Proximidad IR) | 3.3V - 5V DC | Detección de parada en estación de llenado | VCC: 3.3V, GND: GND, OUT: GPIO 35 |
| **7** | Pantalla Gráfica TFT | 2.8" SPI ILI9341 (240x320) | 3.3V (Lógica y VCC) | Monitoreo visual de estado y conteo | SCK: 18, MISO: 19, MOSI: 23, CS: 5, DC: 21, RST: 22, LED: 4 |
| **8** | Reproductor de Audio | DFPlayer Mini (clon MP3-TF-16P) | 5V DC | Locución por voz de estados y alertas | VCC: 5V, RX: GPIO 17 (vía R=1k), TX: GPIO 16 (RX2), SPK: Bocina |
| **9** | Bocina / Altavoz | 8 Ω / 2W a 3W | Señal analógica | Salida sonora de mensajes del DFPlayer | Conectada a SPK_1 y SPK_2 del DFPlayer |
| **10** | Tarjeta MicroSD | 8GB a 32GB (FAT32) | 3.3V | Almacenamiento de pistas 0001.mp3 a 0005.mp3 | Ranura DFPlayer Mini |
| **11** | Botón de Parada | Pulsador normalmente abierto (NA) | 3.3V (INPUT_PULLUP) | Botón físico de parada de emergencia | Entre GPIO 32 y GND |
| **12** | Batería Principal | LiPo 2S (7.4V nominal) | 7.4V - 8.4V | Fuente de energía portátil del sistema | Conectada a interruptor principal |
| **13** | Elevador de Voltaje | Módulo Boost Step-Up (ej. XL6009 / MT3608)| Vin: 7.4V -> Vout: 12V | Suministro para motor, electroválvula e inductivo | Entrada desde LiPo, Salida a bus 12V |
| **14** | Reductor de Voltaje | Módulo Buck Step-Down (ej. LM2596 / MP1584)| Vin: 12V o 7.4V -> Vout: 5V | Alimentación para ESP32 (VIN), DFPlayer y TFT | Salida a bus 5V y entrada de reguladores 3.3V |
| **15** | Resistencia de Protección | 1 kΩ (1/4W) | Señal lógica | Adaptación de niveles y desacoplo ESP32 TX -> DFPlayer RX | En serie en línea GPIO 17 -> RX DFPlayer |
| **16** | Resistencia de Pull-up | 10 kΩ (1/4W) | Señal lógica | Pull-up externo a 3.3V para sensor inductivo NPN | Entre GPIO 34 y riel de 3.3V |

---

### Notas de Alimentación y Masas Comunes:
- **GND Común:** Todos los puntos de referencia GND (batería, elevador 12V, reductor 5V, ESP32, TB6612, DFPlayer y sensores) DEBEN estar interconectados en un punto común para garantizar el retorno de señal correcto.
- **Riel de 12V:** Alimenta el motorreductor, la electroválvula y el sensor inductivo LJ12A3-4-Z/BX.
- **Riel de 5V:** Alimenta el pin VIN del ESP32 y el pin VCC del DFPlayer Mini.
- **Riel de 3.3V:** Provisto por el regulador integrado del ESP32 para alimentar la pantalla TFT ILI9341, el sensor FC-51 y los resistores pull-up.
