#
# Copyright (c) 2026, Realtek Semiconductor Corporation
#
# SPDX-License-Identifier: MIT
#

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} \
    -xc \
    -march=armv8.1-m.main+dsp+mve+fp \
    -mthumb \
    -std=c11 \
    -mfloat-abi=hard \
    -O2 \
    -Wall -fdata-sections -ffunction-sections \
    -Wno-implicit-function-declaration")

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} \
    -std=c++11 \
    -march=armv8.1-m.main+dsp+mve+fp \
    -mthumb \
    -std=c11 \
    -mfloat-abi=hard \
    -O2 \
    -Wall -fdata-sections -ffunction-sections \
    -Wno-unused-macros")
