#
# Copyright (c) 2026, Realtek Semiconductor Corporation
#
# SPDX-License-Identifier: MIT
#

set(CONFIG_SOC_SERIES_RTL8763D y)
set(mcpu_arch "-mcpu=cortex-m4")
	set(mfpu_arch "-mfpu=fpv4-sp-d16")
	set(mfloat_arch "-mfloat-abi=hard")
	set(march_arch "")
	add_compile_options(
						-w
						-g1
						-Os
						-mcpu=cortex-m4
						-mfpu=fpv4-sp-d16
						-mfloat-abi=hard
						-mthumb -Wall
						-Wno-parentheses
						-Wno-unused-variable
						-Wno-return-type
						-Wno-address-of-packed-member
						-fdata-sections
						-ffunction-sections
						-std=gnu99
						-fstack-usage
						-fno-delete-null-pointer-checks
						-fshort-wchar
                        -Werror=undef 
                        -Werror=sign-compare 
                        -Werror=unused-parameter 
                        -Werror=missing-braces 
                        -Werror=unused-variable 
                        -Werror=format 
                        -Werror=char-subscripts 
                        -Werror=parentheses 
                        -Werror=implicit-fallthrough 
                        -Werror=type-limits 
                        -Wno-packed 
                        -Wno-missing-prototypes 
                        -Wno-missing-noreturn 
                        -Wno-sign-conversion 
                        -Wno-unused-macros 
                        -Wno-implicit-function-declaration
                        -Werror=deprecated-copy
                        -Wmissing-field-initializers)
