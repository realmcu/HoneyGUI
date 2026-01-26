#
# Copyright (c) 2026, Realtek Semiconductor Corporation
#
# SPDX-License-Identifier: Apache-2.0
#

# Apply the flags to C and C++ compiler settings armcc

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} \
    --c99 \
    -c	\
    --cpu=Cortex-M4.fp.sp \
    --li \
    -g \
    -O2 \
    --apcs=interwork \
    --bss_threshold=0 \
    --diag_suppress=177 ")
	
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} \
    -c	\
    --cpu=Cortex-M4.fp.sp \
    --li \
    -g \
    -O2 \
    --apcs=interwork \
    --bss_threshold=0 \
    --diag_suppress=177")
