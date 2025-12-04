@echo off
REM ========================================
REM Build script for C3 Customer
REM SOC: RTL8762G
REM ========================================



cmake -G "MinGW Makefiles" -DSOC=RTL8762G -B "./temp_C3"
cd temp_C3
cmake --build . -j 32
cmake --build . --target install -j 32
pause
