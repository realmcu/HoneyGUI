# Apply the flags to C and C++ compiler settings
set(CONFIG_SOC_SERIES_RTL8773E y)

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} \
    -xc \
    -std=gnu11 \
    -march=armv8-m.main -mfpu=fpv5-sp-d16 \
    -mfloat-abi=hard \
    -funsigned-char \
    -fshort-enums \
    -fshort-wchar \
    -mlittle-endian \
    -gdwarf-4 \
    -O2 \
    -Werror=undef \
    -Werror=sign-compare \
    -Wno-packed \
    -Wno-missing-prototypes \
    -Wno-missing-noreturn \
    -Wno-sign-conversion \
    -Wno-unused-macros \
    -Wno-implicit-function-declaration ")

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} \
    -std=gnu++11 \
    -march=armv8-m.main -mfpu=fpv5-sp-d16 \
    -mfloat-abi=hard \
    -funsigned-char \
    -fshort-enums \
    -fshort-wchar \
    -mlittle-endian \
    -gdwarf-4 \
    -O2 \
    -Wno-packed \
    -Wno-missing-noreturn \
    -Wno-sign-conversion \
    -Wno-unused-macros \
    -Wmissing-field-initializers")