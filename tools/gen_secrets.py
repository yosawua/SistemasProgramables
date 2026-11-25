#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
Script generador de cabecera de secretos C++ para firmware ESP32.
Lee las variables definidas en .env y produce firmware/include/secrets.h.

Regla de seguridad:
NUNCA imprime ni registra en logs los valores de las variables secretas.
Solo reporta el estado de presencia y validez.
"""

import os
import sys
from pathlib import Path

def parse_env_file(env_path: Path) -> dict:
    """Lee el archivo .env soportando python-dotenv o análisis nativo."""
    variables = {}
    if not env_path.exists():
        return variables

    try:
        from dotenv import dotenv_values
        variables = dotenv_values(env_path)
    except ImportError:
        # Fallback sin dependencias externas
        with open(env_path, "r", encoding="utf-8") as f:
            for line in f:
                line = line.strip()
                if not line or line.startswith("#"):
                    continue
                if "=" in line:
                    key, val = line.split("=", 1)
                    key = key.strip()
                    val = val.strip().strip("'\"")
                    variables[key] = val

    return {k: (v if v is not None else "") for k, v in variables.items()}

def main():
    script_dir = Path(__file__).resolve().parent
    repo_root = script_dir.parent
    env_file = repo_root / ".env"
    output_header = repo_root / "firmware" / "include" / "secrets.h"

    print("==================================================")
    print("  Generador de Secretos de Firmware (ESP32)")
    print("==================================================")

    if not env_file.exists():
        print(f"⚠️  No se encontró archivo .env en: {env_file}")
        print("   Creando secretos con valores por defecto seguros.")
        env_vars = {}
    else:
        print(f"📄 Archivo .env detectado: {env_file.name}")
        env_vars = parse_env_file(env_file)

    wifi_ssid = env_vars.get("WIFI_AP_SSID", "") or "BandaTransportadora_AP"
    wifi_pass = env_vars.get("WIFI_AP_PASSWORD", "") or "12345678"
    eleven_key = env_vars.get("ELEVENLABS_API_KEY", "")
    eleven_voice = env_vars.get("ELEVENLABS_VOICE_ID", "")

    # Auditoría de estado (sin imprimir valores)
    print(f" - WIFI_AP_SSID:         {'[CONFIGURADO]' if wifi_ssid else '[VACÍO]'}")
    print(f" - WIFI_AP_PASSWORD:     {'[LONGITUD VÁLIDA]' if len(wifi_pass) >= 8 else '[INVÁLIDO: < 8 CARACTERES]'}")
    print(f" - ELEVENLABS_API_KEY:   {'[PRESENTE]' if eleven_key else '[NO DEFINIDA]'}")
    print(f" - ELEVENLABS_VOICE_ID:  {'[PRESENTE]' if eleven_voice else '[NO DEFINIDA]'}")

    content = f"""#pragma once
/**
 * @file secrets.h
 * @brief Credenciales generadas automáticamente por tools/gen_secrets.py
 *
 * ¡ATENCIÓN! ESTE ARCHIVO CONTIENE DATOS SENSIBLES.
 * NO EDITAR MANUALMENTE Y NO SUBIR AL CONTROL DE VERSIONES (GIT).
 */

#ifndef SECRETS_H
#define SECRETS_H

#define WIFI_AP_SSID      "{wifi_ssid}"
#define WIFI_AP_PASSWORD  "{wifi_pass}"

#define ELEVENLABS_API_KEY "{eleven_key}"
#define ELEVENLABS_VOICE_ID "{eleven_voice}"

#endif // SECRETS_H
"""

    output_header.parent.mkdir(parents=True, exist_ok=True)
    with open(output_header, "w", encoding="utf-8") as f:
        f.write(content)

    print(f"\n✅ Archivo generado exitosamente en:")
    print(f"   {output_header.relative_to(repo_root)}")
    print("==================================================")
    return 0

if __name__ == "__main__":
    sys.exit(main())
