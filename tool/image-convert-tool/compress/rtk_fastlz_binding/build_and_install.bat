@echo off
REM Build and install RTK FastLz Python binding
REM 编译并安装 RTK FastLz Python 绑定

echo ========================================
echo Build RTK FastLz Python Binding
echo ========================================
echo.
echo This module uses Windows tool's FastLz code
echo MAX_DISTANCE=64 (compatible with C runtime)
echo.

REM Clean previous build
echo Step 1: Cleaning previous build...
if exist build rmdir /s /q build
if exist dist rmdir /s /q dist
if exist rtk_fastlz.egg-info rmdir /s /q rtk_fastlz.egg-info

REM Build
echo.
echo Step 2: Building module...
python setup.py build

if %ERRORLEVEL% NEQ 0 (
    echo.
    echo ERROR: Build failed!
    echo Make sure you have Visual Studio Build Tools installed.
    echo.
    pause
    exit /b 1
)

REM Install
echo.
echo Step 3: Installing module...
python setup.py install --force

if %ERRORLEVEL% NEQ 0 (
    echo.
    echo ERROR: Installation failed!
    echo Try running as Administrator.
    echo.
    pause
    exit /b 1
)

REM Test
echo.
echo Step 4: Testing module...
python -c "import rtk_fastlz; print('✓ RTK FastLz module loaded successfully!'); data = b'test' * 100; compressed = rtk_fastlz.compress(data); print(f'✓ Compression test: {len(data)} bytes -> {len(compressed)} bytes')"

if %ERRORLEVEL% NEQ 0 (
    echo.
    echo ERROR: Module test failed!
    echo.
    pause
    exit /b 1
)

echo.
echo ========================================
echo SUCCESS!
echo ========================================
echo RTK FastLz Python binding installed successfully.
echo.
echo You can now use:
echo   import rtk_fastlz
echo   compressed = rtk_fastlz.compress(data)
echo.
echo This will use Windows tool's FastLz code (100%% compatible)
echo.
pause
