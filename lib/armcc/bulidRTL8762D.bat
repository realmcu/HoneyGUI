
cmake -G "MinGW Makefiles" -DSOC=RTL8762D -B "./temp"
cd temp
cmake --build . -j 32
cmake --build . --target install -j 32
pause