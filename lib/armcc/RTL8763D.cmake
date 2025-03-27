set(CONFIG_SOC_SERIES_RTL8763D y)

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} \
    -c \
    --cpu=Cortex-M4 \
    --fpu=FPv4-SP \
    --apcs=interwork \
    -O0 \
    --c99 \
    --li \
    -g \
    --bss_threshold=0 \
    --diag_suppress=177 ")


set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} \
    -c	\
    --cpu=Cortex-M4 \
    -g \
    -O0 \
    --cpp \
    --li \
    --apcs=interwork \
    --fpu=FPv4-SP \
    --bss_threshold=0 \
    --diag_suppress=177")
