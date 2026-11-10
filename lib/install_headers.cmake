#
# Copyright (c) 2026, Realtek Semiconductor Corporation
#
# SPDX-License-Identifier: MIT
#

# Common header files installation for HoneyGUI library
# Usage: include(${CMAKE_CURRENT_SOURCE_DIR}/../install_headers.cmake)

set(REALGUI_DIR "${CMAKE_CURRENT_SOURCE_DIR}/../../realgui")

file(GLOB_RECURSE HEADER_FILES "${REALGUI_DIR}/gui_version.h")

file(GLOB_RECURSE TMP_HEADERS "${REALGUI_DIR}/widget/*.h")
list(APPEND HEADER_FILES ${TMP_HEADERS})

file(GLOB_RECURSE TMP_HEADERS "${REALGUI_DIR}/server/*.h")
list(APPEND HEADER_FILES ${TMP_HEADERS})

file(GLOB_RECURSE TMP_HEADERS "${REALGUI_DIR}/core/*.h")
list(APPEND HEADER_FILES ${TMP_HEADERS})

file(GLOB_RECURSE TMP_HEADERS "${REALGUI_DIR}/app/*.h")
list(APPEND HEADER_FILES ${TMP_HEADERS})

file(GLOB_RECURSE TMP_HEADERS "${REALGUI_DIR}/dc/*.h")
list(APPEND HEADER_FILES ${TMP_HEADERS})

file(GLOB_RECURSE TMP_HEADERS "${REALGUI_DIR}/input/*.h")
list(APPEND HEADER_FILES ${TMP_HEADERS})

file(GLOB_RECURSE TMP_HEADERS "${REALGUI_DIR}/engine/*.h")
list(APPEND HEADER_FILES ${TMP_HEADERS})

file(GLOB_RECURSE TMP_HEADERS "${REALGUI_DIR}/misc/*.h")
list(APPEND HEADER_FILES ${TMP_HEADERS})

file(GLOB_RECURSE TMP_HEADERS "${REALGUI_DIR}/sysval/*.h")
list(APPEND HEADER_FILES ${TMP_HEADERS})

# Letter shell headers - conditionally included based on config
if(DEFINED CONFIG_REALTEK_BUILD_LETTER_SHELL AND CONFIG_REALTEK_BUILD_LETTER_SHELL)
    file(GLOB_RECURSE TMP_HEADERS "${REALGUI_DIR}/3rd/letter_shell/*.h")
    list(APPEND HEADER_FILES ${TMP_HEADERS})
endif()

install(FILES ${HEADER_FILES} DESTINATION include)
