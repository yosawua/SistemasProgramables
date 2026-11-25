@echo off
chcp 65001 > nul
echo ========================================================
echo   Instalador de Entorno - Proyecto Banda Transportadora
echo   Equipo: Los Pythones
echo ========================================================

REM 1. Crear entorno virtual si no existe
if not exist ".venv" (
    echo [1/4] Creando entorno virtual Python (.venv)...
    python -m venv .venv
    if %errorlevel% neq 0 (
        echo [ERROR] No se pudo crear el entorno virtual. Verifica que Python este instalado y en tu PATH.
        pause
        exit /b %errorlevel%
    )
) else (
    echo [1/4] Entorno virtual .venv ya existente.
)

REM 2. Instalar dependencias en el entorno virtual
echo [2/4] Instalando dependencias desde requirements.txt...
call .venv\Scripts\activate.bat
python -m pip install --upgrade pip
pip install -r requirements.txt
if %errorlevel% neq 0 (
    echo [ADVERTENCIA] Ocurrio un inconveniente al instalar requerimientos.
)

REM 3. Preparar archivo .env si no existe
if not exist ".env" (
    echo [3/4] Creando archivo .env a partir de .env.example...
    copy .env.example .env > nul
    echo       Recuerda editar .env con tus credenciales reales.
) else (
    echo [3/4] Archivo .env detectado.
)

REM 4. Generar cabecera secrets.h
echo [4/4] Generando firmware/include/secrets.h...
python tools\gen_secrets.py

echo.
echo ========================================================
echo   Entorno configurado exitosamente.
echo   Para activar el entorno: .venv\Scripts\activate
echo   Para validar credenciales: python tools\check_env.py
echo   Para compilar firmware: cd firmware ^&^& pio run
echo ========================================================
pause
