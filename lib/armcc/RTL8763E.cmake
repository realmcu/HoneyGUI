# Apply the flags to C and C++ compiler settings armcc
set(CONFIG_SOC_SERIES_RTL8763E y)
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} \
    --c99 \
    -c	\
    --cpu=Cortex-M3 \
    --li \
    -g \
    -O2 \
    --apcs=interwork \
    --bss_threshold=0 \
    --diag_suppress=177 ")
	
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} \
    -c	\
    --cpu=Cortex-M3 \
    --li \
    -g \
    -O2 \
    --apcs=interwork \
    --bss_threshold=0 \
    --diag_suppress=177")
