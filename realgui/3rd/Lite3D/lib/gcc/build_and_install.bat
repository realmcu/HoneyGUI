@echo off
REM This script configures and builds a CMake project

REM Set paths and options
set GENERATOR="MinGW Makefiles"
set BUILD_DIR=tmp

REM Configure the project
cmake -G %GENERATOR% -DCONFIG_REALTEK_BUILD_LITE3D=y -B %BUILD_DIR%
if %errorlevel% neq 0 (
    echo "CMake configuration failed."
    exit /b %errorlevel%
)

REM Build and install the project
cmake --build %BUILD_DIR% --target install
if %errorlevel% neq 0 (
    echo "Build and installation failed."
    exit /b %errorlevel%
)

echo "Build and installation successful."


cmake -G "MinGW Makefiles" -DCONFIG_REALTEK_BUILD_LITE3D=y --build -B tmp --target install
