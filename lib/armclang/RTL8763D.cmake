#
# Copyright (c) 2026, Realtek Semiconductor Corporation
#
# SPDX-License-Identifier: MIT
#

# Apply the flags to C and C++ compiler settings
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} \
    -xc \
	-std=gnu99 \
    --target=arm-arm-none-eabi \
	-mcpu=cortex-m4 -mfpu=fpv4-sp-d16 \
    -mfloat-abi=hard \
    -fno-rtti \
    -flto \
    -funsigned-char \
    -fshort-enums \
    -fshort-wchar \
    -mlittle-endian \
    -gdwarf-4 \
    -Ofast \
    -fno-function-sections \
    -Wno-packed \
    -Wno-missing-variable-declarations \
    -Wno-missing-prototypes \
    -Wno-missing-noreturn \
    -Wno-sign-conversion \
    -Wno-nonportable-include-path \
    -Wno-reserved-id-macro \
    -Wno-unused-macros \
    -Wno-documentation-unknown-command \
    -Wno-documentation \
    -Wno-license-management \
    -Wno-parentheses-equality \
    -Wno-implicit-function-declaration ")
	
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} \
	-std=c++11 \
    --target=arm-arm-none-eabi \
    -mcpu=cortex-m4 -mfpu=fpv4-sp-d16 \
    -mfloat-abi=hard \
    -flto \
    -funsigned-char \
    -fshort-enums \
    -fshort-wchar \
    -mlittle-endian \
    -gdwarf-4 \
    -Ofast \
    -fno-function-sections \
    -Wno-packed \
    -Wno-missing-variable-declarations \
    -Wno-missing-prototypes \
    -Wno-missing-noreturn \
    -Wno-sign-conversion \
    -Wno-nonportable-include-path \
    -Wno-reserved-id-macro \
    -Wno-unused-macros \
    -Wno-documentation-unknown-command \
    -Wno-documentation \
    -Wno-license-management \
    -Wno-parentheses-equality")