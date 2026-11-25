# Pistas de Audio para DFPlayer Mini

Este directorio almacena los archivos de audio MP3 generados mediante el servicio de síntesis de voz (Text-to-Speech) de ElevenLabs a través del script `tools/generate_audio.py`.

---

## Tabla de Pistas Asignadas

| Archivo | Pista DFPlayer | Texto Síntesis de Voz | Evento Disparador |
| :---: | :---: | :--- | :--- |
| `0001.mp3` | Pista 1 | *"Recipiente metálico detectado"* | Activación de Sensor Inductivo LJ12A3 |
| `0002.mp3` | Pista 2 | *"Llenando"* | Activación de Sensor IR y apertura de Válvula |
| `0003.mp3` | Pista 3 | *"Llenado completo"* | Finalización del temporizador de dosificación |
| `0004.mp3` | Pista 4 | *"Recipiente no metálico"* | Descarte o paso de recipiente plástico |
| `0005.mp3` | Pista 5 | *"Sistema detenido"* | Parada de emergencia física o por comando Web |

---

## Instrucciones para Carga en Tarjeta MicroSD
1. Formatea una tarjeta MicroSD (capacidad máxima recomendada 32GB) con sistema de archivos **FAT32** con tamaño de sector predeterminado.
2. Copia los archivos `0001.mp3`, `0002.mp3`, `0003.mp3`, `0004.mp3` y `0005.mp3` directamente en la raíz de la tarjeta (o en una carpeta nombrada `mp3` si tu versión del módulo lo requiere).
3. Inserta la tarjeta en la ranura del módulo DFPlayer Mini con el circuito apagado.
