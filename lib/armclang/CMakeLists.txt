cmake_minimum_required (VERSION 3.10)
cmake_policy(SET CMP0123 NEW)

set(CMAKE_SYSTEM_NAME Generic)

set(ARMCLANG_PATH "C:/Keil_v5/ARM/ArmCompilerforEmbedded6.22/bin")

set(CMAKE_C_COMPILER "${ARMCLANG_PATH}/armclang.exe")
set(CMAKE_CXX_COMPILER "${ARMCLANG_PATH}/armclang.exe")


set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_EXTENSIONS ON) 
set(CMAKE_C_STANDARD 99)
set(CMAKE_C_EXTENSIONS ON)

set(CMAKE_VERBOSE_MAKEFILE OFF)


# Apply the flags to C and C++ compiler settings
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} \
    -xc \
	-std=c11 \
    --target=arm-arm-none-eabi \
    -march=armv8.1-m.main+dsp+mve.fp+fp \
    -mfloat-abi=hard \
    -fno-rtti \
    -flto \
    -funsigned-char \
    -fshort-enums \
    -fshort-wchar \
    -mlittle-endian \
    -gdwarf-4 \
    -O2 \
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
	-std=c++98 \
    --target=arm-arm-none-eabi \
    -march=armv8.1-m.main+dsp+mve.fp+fp \
    -mfloat-abi=hard \
    -flto \
    -funsigned-char \
    -fshort-enums \
    -fshort-wchar \
    -mlittle-endian \
    -gdwarf-4 \
    -O2 \
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



set(CMAKE_TRY_COMPILE_TARGET_TYPE STATIC_LIBRARY)

set(CMAKE_INSTALL_PREFIX "${CMAKE_CURRENT_SOURCE_DIR}/install" CACHE PATH "Installation Directory" FORCE)


project(HoneyGUI)


set(GUI_TARGET_NAME gui)

add_library(${GUI_TARGET_NAME} STATIC)
add_subdirectory(../../realgui build/realgui)

install(TARGETS ${GUI_TARGET_NAME}
        ARCHIVE DESTINATION lib
        LIBRARY DESTINATION lib
        RUNTIME DESTINATION bin)

file(GLOB_RECURSE NEW_HEADER_FILES "../../realgui/widget/*.h")
list(APPEND HEADER_FILES ${NEW_HEADER_FILES})
file(GLOB_RECURSE NEW_HEADER_FILES "../../realgui/server/*.h")
list(APPEND HEADER_FILES ${NEW_HEADER_FILES})

install(FILES ${HEADER_FILES} DESTINATION include)
