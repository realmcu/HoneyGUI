@rem Build libgui.a for WebAssembly. Run from an emsdk-activated prompt
@rem (emsdk_env.bat), so that emcmake is in PATH.
call emcmake cmake -G "MinGW Makefiles" -S . -B build
cmake --build build -j 32
cmake --build build --target install
pause
