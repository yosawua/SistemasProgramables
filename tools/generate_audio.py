#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
Script de síntesis de voz (Text-to-Speech) con ElevenLabs para DFPlayer Mini.
Genera pistas de audio MP3 nombradas secuencialmente (0001.mp3, 0002.mp3, etc.)
en la carpeta /audio para ser copiadas en la tarjeta micro SD.

Regla: No regenera archivos que ya existan en disco.
"""

import os
import sys
from pathlib import Path

# Lista configurable de pistas de audio para la banda transportadora
FRASES_SISTEMA = {
    1: "Recipiente metálico detectado",
    2: "Llenando",
    3: "Llenado completo",
    4: "Recipiente no metálico",
    5: "Sistema detenido"
}

def load_env(env_path: Path) -> dict:
    config = {}
    if not env_path.exists():
        return config
    try:
        from dotenv import dotenv_values
        config = dotenv_values(env_path)
    except ImportError:
        with open(env_path, "r", encoding="utf-8") as f:
            for line in f:
                line = line.strip()
                if not line or line.startswith("#"):
                    continue
                if "=" in line:
                    k, v = line.split("=", 1)
                    config[k.strip()] = v.strip().strip("'\"")
    return {k: (v if v is not None else "") for k, v in config.items()}

def generar_audio(track_id: int, texto: str, output_dir: Path, api_key: str, voice_id: str) -> bool:
    import requests

    filename = f"{track_id:04d}.mp3"
    target_path = output_dir / filename

    if target_path.exists():
        print(f" ⏩ [OMITIDO] {filename} ya existe ({target_path.stat().st_size} bytes). Saltando...")
        return True

    print(f" 🎙️  [GENERANDO] Pista {filename}: \"{texto}\"...")

    url = f"https://api.elevenlabs.io/v1/text-to-speech/{voice_id}"
    headers = {
        "xi-api-key": api_key,
        "Content-Type": "application/json",
        "Accept": "audio/mpeg"
    }
    payload = {
        "text": texto,
        "model_id": "eleven_multilingual_v2",
        "voice_settings": {
            "stability": 0.5,
            "similarity_boost": 0.75,
            "style": 0.0,
            "use_speaker_boost": True
        }
    }

    try:
        response = requests.post(url, json=payload, headers=headers, timeout=30)
        if response.status_code == 200:
            with open(target_path, "wb") as f:
                f.write(response.content)
            print(f" ✅ [GUARDADO] {filename} creado exitosamente ({len(response.content)} bytes)")
            return True
        else:
            print(f" ❌ [ERROR] Falló la API para {filename} (HTTP {response.status_code}): {response.text[:120]}")
            return False
    except requests.exceptions.RequestException as e:
        print(f" ❌ [ERROR DE RED] {e}")
        return False

def main():
    script_dir = Path(__file__).resolve().parent
    repo_root = script_dir.parent
    env_file = repo_root / ".env"
    audio_dir = repo_root / "audio"
    audio_dir.mkdir(parents=True, exist_ok=True)

    print("==================================================")
    print("   Generador de Pistas de Audio MP3 (ElevenLabs)")
    print("==================================================")

    config = load_env(env_file)
    api_key = config.get("ELEVENLABS_API_KEY", "").strip()
    voice_id = config.get("ELEVENLABS_VOICE_ID", "").strip()

    if not api_key:
        print("❌ Error: ELEVENLABS_API_KEY no está configurada en .env")
        print("   Por favor agrega tu API Key antes de generar audios.")
        return 1

    if not voice_id:
        print("❌ Error: ELEVENLABS_VOICE_ID no está configurada en .env")
        print("   Por favor especifica un ID de voz válido.")
        return 1

    try:
        import requests
    except ImportError:
        print("❌ Error: La librería 'requests' no está instalada.")
        print("   Ejecuta: pip install -r requirements.txt")
        return 1

    exitos = 0
    total = len(FRASES_SISTEMA)

    for track_num, frase in FRASES_SISTEMA.items():
        if generar_audio(track_num, frase, audio_dir, api_key, voice_id):
            exitos += 1

    print("==================================================")
    print(f" Proceso finalizado: {exitos}/{total} pistas disponibles en {audio_dir.name}/")
    print(" 💡 Recuerda copiar estos archivos en la raíz de una tarjeta microSD")
    print("    formateada en FAT32 para su uso en el DFPlayer Mini.")
    print("==================================================")
    return 0 if exitos == total else 1

if __name__ == "__main__":
    sys.exit(main())
