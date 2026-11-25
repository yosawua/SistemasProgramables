#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
Script de verificación y validación de variables de entorno (.env).
Comprueba la existencia, formato y autenticidad de credenciales sin revelar secretos.
"""

import os
import sys
from pathlib import Path

def print_status(emoji: str, field: str, message: str):
    print(f" {emoji} {field:<22} : {message}")

def main():
    script_dir = Path(__file__).resolve().parent
    repo_root = script_dir.parent
    env_file = repo_root / ".env"

    print("==================================================")
    print("   Verificación de Variables de Entorno (.env)")
    print("==================================================")

    errores = 0

    if not env_file.exists():
        print_status("❌", "Archivo .env", f"No se encontró el archivo en {env_file}")
        print("\n💡 Sugerencia: Copia '.env.example' a '.env' y completa tus valores.")
        return 1

    print_status("✅", "Archivo .env", "Presente en la raíz del proyecto")

    # Cargar variables con python-dotenv o fallback
    try:
        from dotenv import dotenv_values
        config = dotenv_values(env_file)
    except ImportError:
        # Fallback de lectura simple
        config = {}
        with open(env_file, "r", encoding="utf-8") as f:
            for line in f:
                line = line.strip()
                if not line or line.startswith("#"):
                    continue
                if "=" in line:
                    k, v = line.split("=", 1)
                    config[k.strip()] = v.strip().strip("'\"")

    # 1. Validación de WIFI_AP_SSID
    ssid = config.get("WIFI_AP_SSID", "").strip()
    if ssid:
        print_status("✅", "WIFI_AP_SSID", f"Definido correctamente ('{ssid}')")
    else:
        print_status("❌", "WIFI_AP_SSID", "Variable vacía o no definida")
        errores += 1

    # 2. Validación de WIFI_AP_PASSWORD
    password = config.get("WIFI_AP_PASSWORD", "").strip()
    if not password:
        print_status("❌", "WIFI_AP_PASSWORD", "Variable vacía o no definida")
        errores += 1
    elif len(password) < 8:
        print_status("❌", "WIFI_AP_PASSWORD", f"Longitud insuficiente ({len(password)} car.). Mínimo 8 requerido por WPA2")
        errores += 1
    else:
        print_status("✅", "WIFI_AP_PASSWORD", f"Válido ({len(password)} caracteres)")

    # 3. Validación de ELEVENLABS_API_KEY
    api_key = config.get("ELEVENLABS_API_KEY", "").strip()
    if not api_key:
        print_status("❌", "ELEVENLABS_API_KEY", "Variable vacía o no definida")
        errores += 1
    else:
        print_status("✅", "ELEVENLABS_API_KEY", f"Formato detectado ({len(api_key)} caracteres)")

    # 4. Validación de ELEVENLABS_VOICE_ID
    voice_id = config.get("ELEVENLABS_VOICE_ID", "").strip()
    if not voice_id:
        print_status("❌", "ELEVENLABS_VOICE_ID", "Variable vacía o no definida")
        errores += 1
    else:
        print_status("✅", "ELEVENLABS_VOICE_ID", f"Definido ({voice_id})")

    # 5. Validación remota en la API de ElevenLabs (si se cuenta con llave y módulo requests)
    if api_key:
        print("\n--- Comprobando conexión y permisos con ElevenLabs API ---")
        try:
            import requests
            headers = {"xi-api-key": api_key}
            user_url = "https://api.elevenlabs.io/v1/user"

            resp = requests.get(user_url, headers=headers, timeout=10)
            if resp.status_code == 200:
                print_status("✅", "API ElevenLabs", "Autenticación exitosa (llave válida)")

                # Validar existencia del VOICE_ID si fue configurado
                if voice_id:
                    voice_url = f"https://api.elevenlabs.io/v1/voices/{voice_id}"
                    v_resp = requests.get(voice_url, headers=headers, timeout=10)
                    if v_resp.status_code == 200:
                        v_data = v_resp.json()
                        v_name = v_data.get("name", "Voz")
                        print_status("✅", "Voz Seleccionada", f"Voz confirmada: '{v_name}'")
                    else:
                        print_status("❌", "Voz Seleccionada", f"El VOICE_ID no existe o no está disponible (HTTP {v_resp.status_code})")
                        errores += 1
            elif resp.status_code == 401:
                print_status("❌", "API ElevenLabs", "Llave rechazada: No autorizada (401 Unauthorized)")
                errores += 1
            else:
                print_status("❌", "API ElevenLabs", f"Respuesta inesperada del servidor (HTTP {resp.status_code})")
                errores += 1

        except ImportError:
            print_status("⚠️", "Módulo requests", "No instalado en el entorno. No se pudo verificar la API remota")
        except requests.exceptions.RequestException as e:
            print_status("❌", "Conexión ElevenLabs", f"Fallo de conexión o tiempo de espera agotado ({type(e).__name__})")
            errores += 1

    print("==================================================")
    if errores == 0:
        print("🎉 ¡Todas las comprobaciones fueron superadas con éxito!")
        return 0
    else:
        print(f"⚠️  Se encontraron {errores} problema(s). Revisa la configuración en tu archivo .env")
        return 1

if __name__ == "__main__":
    sys.exit(main())
