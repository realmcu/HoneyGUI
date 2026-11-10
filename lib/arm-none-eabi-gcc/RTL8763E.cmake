#
# Copyright (c) 2026, Realtek Semiconductor Corporation
#
# SPDX-License-Identifier: MIT
#

# Apply the flags to C and C++ compiler settings
set(CONFIG_SOC_SERIES_RTL8763E y)
set(mcpu_arch "-mcpu=cortex-m3")
    set(mfpu_arch "-mfloat-abi=soft")
    set(mfloat_arch "")
    set(march_arch "-march=armv7-m")
    add_compile_options(
                        -w
                        -g1
                        -Os
                        -march=armv7-m
                        -mcpu=cortex-m3
                        -mfloat-abi=soft
                        -mthumb -Wall
                        -Wno-parentheses
                        -Wno-unused-variable
                        -Wno-return-type
                        -Wno-address-of-packed-member
                        -Wno-unknown-pragmas
                        -Wno-format-truncation
                        -Wno-format
                        -Wno-missing-braces
                        -fdata-sections
                        -ffunction-sections
                        -std=gnu99
                        -fstack-usage
                        -fno-delete-null-pointer-checks
                        -fshort-wchar)