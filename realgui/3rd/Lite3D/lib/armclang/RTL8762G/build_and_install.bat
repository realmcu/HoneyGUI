@echo off
REM This script configures and builds a CMake project

REM Set paths and options
set SOURCE_DIR=..
set GENERATOR="MinGW Makefiles"
set BUILD_DIR=tmp
set SOC_OPT=-DSOC=RTL8762G

REM Configure the project
cmake %SOURCE_DIR% -G %GENERATOR% -DCONFIG_REALTEK_BUILD_LITE3D=y -B %BUILD_DIR% %SOC_OPT%
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
