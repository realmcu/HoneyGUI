
cmake -G "MinGW Makefiles" -DSOC=RTL8773E_C2 -B "./temp"
cd temp
cmake --build . -j 32
cmake --build . --target install -j 32
pause