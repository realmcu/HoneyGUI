
cmake -G "MinGW Makefiles" -DSOC=RTL8773G -DCONFIG_REALTEK_BUILD_LITE3D=y -DCONFIG_REALTEK_BUILD_LITE3D_8773G_ARMCL_LIB=y -B "./temp"
cd temp
cmake --build . -j 32
cmake --build . --target install -j 32
pause