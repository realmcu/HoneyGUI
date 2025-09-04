# Apply the flags to C and C++ compiler settings
set(CONFIG_SOC_SERIES_RTL8773G y)

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} \
    -xc \
    -std=gnu11 \
    -mcpu=cortex-m55 \
    -mfloat-abi=hard \
    -fno-exceptions \
    -funsigned-char \
    -fshort-enums \
    -fshort-wchar \
    -mlittle-endian \
    -gdwarf-4 \
    -O3 \
    -Werror=undef \
    -Werror=sign-compare \
    -Werror=unused-parameter \
    -Werror=missing-braces \
    -Werror=unused-variable \
    -Werror=format \
    -Werror=char-subscripts \
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
    -std=gnu++11 \
    -mcpu=cortex-m55 \
    -mfloat-abi=hard \
    -fno-exceptions \
    -funsigned-char \
    -fshort-enums \
    -fshort-wchar \
    -mlittle-endian \
    -gdwarf-4 \
    -O3 \
    -Wno-packed \
    -Wno-missing-noreturn \
    -Wno-sign-conversion \
    -Wno-unused-macros \
    -Wmissing-field-initializers")