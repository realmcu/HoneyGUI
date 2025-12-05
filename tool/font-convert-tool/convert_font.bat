@echo off
REM Font Converter - Windows Batch Launcher
REM Usage: convert_font.bat <config.json>

if "%~1"=="" (
    echo Usage: convert_font.bat ^<config.json^>
    echo Example: convert_font.bat examples\bitmap_basic.json
    exit /b 1
)

python convert_font.py %*
