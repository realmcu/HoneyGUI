


cmake -G "MinGW Makefiles" -B ttt

cmake -G "MinGW Makefiles" -B ttt -DSOC=RTL8762G


cmake -G "MinGW Makefiles" -B ttt && cmake --build ttt --target install
rmdir /S /Q ttt cmake -G "MinGW Makefiles" -B ttt && cmake --build ttt --target install


cmake -G "MinGW Makefiles" -B ttt  -DSOC=RTL8773E && cmake --build ttt  --target install
rmdir /S /Q ttt && cmake -G "MinGW Makefiles" -B ttt   -DSOC=RTL8773E && cmake --build ttt --target install


rmdir /S /Q ttt && cmake -DCMAKE_VERBOSE_MAKEFILE:BOOL=ON -G "MinGW Makefiles" -B ttt   -DSOC=RTL8773E && cmake --build ttt --target install