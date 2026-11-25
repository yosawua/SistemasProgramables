# Script de inicialización de entorno en PowerShell
# Proyecto: Banda Transportadora Clasificadora y Dosificadora

Write-Host "========================================================" -ForegroundColor Cyan
Write-Host "  Instalador de Entorno - Proyecto Banda Transportadora" -ForegroundColor Cyan
Write-Host "  Equipo: Los Pythones" -ForegroundColor Cyan
Write-Host "========================================================" -ForegroundColor Cyan

# 1. Crear entorno virtual
if (-not (Test-Path ".venv")) {
    Write-Host "[1/4] Creando entorno virtual Python (.venv)..." -ForegroundColor Yellow
    python -m venv .venv
} else {
    Write-Host "[1/4] Entorno virtual .venv ya existente." -ForegroundColor Green
}

# 2. Instalar requerimientos
Write-Host "[2/4] Instalando dependencias de Python..." -ForegroundColor Yellow
& ".\.venv\Scripts\python.exe" -m pip install --upgrade pip
& ".\.venv\Scripts\pip.exe" install -r requirements.txt

# 3. Preparar .env
if (-not (Test-Path ".env")) {
    Write-Host "[3/4] Creando archivo .env a partir de .env.example..." -ForegroundColor Yellow
    Copy-Item ".env.example" ".env"
    Write-Host "      Recuerda editar .env con tus credenciales reales." -ForegroundColor Gray
} else {
    Write-Host "[3/4] Archivo .env detectado." -ForegroundColor Green
}

# 4. Generar secrets.h
Write-Host "[4/4] Generando firmware/include/secrets.h..." -ForegroundColor Yellow
& ".\.venv\Scripts\python.exe" tools\gen_secrets.py

Write-Host "`n========================================================" -ForegroundColor Cyan
Write-Host "  Configuración completada con éxito." -ForegroundColor Green
Write-Host "  Para validar credenciales: python tools\check_env.py" -ForegroundColor White
Write-Host "  Para compilar firmware: cd firmware; pio run" -ForegroundColor White
Write-Host "========================================================" -ForegroundColor Cyan
