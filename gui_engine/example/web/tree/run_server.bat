@echo off
setlocal enabledelayedexpansion

REM Set port number
set PORT=8000

REM Use `where` command to find all Python interpreters in system PATH
for /f "tokens=*" %%P in ('where python') do (
    echo Checking %%P...
    REM Specify Python installation path

    REM Get the directory of the Python interpreter and ensure no trailing backslash
    for %%D in ("%%P") do (
        set "PYTHONHOME=%%~dpD"
    )
    if "!PYTHONHOME:~-1!"=="\" set "PYTHONHOME=!PYTHONHOME:~0,-1!"
    echo PYTHONHOME: %PYTHONHOME%
    
    REM Check if the current Python interpreter can import the http module
    %%P -c "import http" >nul 2>&1
    if !errorlevel! == 0 (
        set PYTHON_EXECUTABLE=%%P
        echo Using %%P to start the server.
        
        REM Start the Python server in the background without showing a window
        start /B !PYTHON_EXECUTABLE! -m http.server %PORT%
        
        REM Wait for the server to start
        timeout /t 3 /nobreak >nul
        
        REM Open default browser to access the page
        start http://localhost:%PORT%//gui_engine/example/web/tree/
        
        goto :end
    )
)

echo No valid Python interpreter with http module found.

:end
endlocal
