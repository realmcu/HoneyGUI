
cmake -G "MinGW Makefiles" -DSOC=RTL8773E -DCONFIG_REALTEK_BUILD_LITE3D=y -B "./temp"
cd temp
cmake --build . -j 32
cmake --build . --target install -j 32
pause