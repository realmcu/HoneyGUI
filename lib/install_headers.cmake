#
# Copyright (c) 2026, Realtek Semiconductor Corporation
#
# SPDX-License-Identifier: MIT
#

# Common header files installation for HoneyGUI library
# Usage: include(${CMAKE_CURRENT_SOURCE_DIR}/../install_headers.cmake)

set(REALGUI_DIR "${CMAKE_CURRENT_SOURCE_DIR}/../../realgui")

file(GLOB_RECURSE HEADER_FILES "${REALGUI_DIR}/gui_version.h")

# Widget headers - gated per widget, same switch as the widget sources
file(GLOB WIDGET_DIRS LIST_DIRECTORIES true RELATIVE "${REALGUI_DIR}/widget" "${REALGUI_DIR}/widget/*")
foreach(WIDGET_DIR ${WIDGET_DIRS})
    if(NOT IS_DIRECTORY "${REALGUI_DIR}/widget/${WIDGET_DIR}")
        continue()
    endif()
    string(REGEX REPLACE "^gui_" "" WIDGET_SWITCH "${WIDGET_DIR}")
    string(TOUPPER "${WIDGET_SWITCH}" WIDGET_SWITCH)
    set(WIDGET_SWITCH "CONFIG_REALTEK_GUI_WIDGET_${WIDGET_SWITCH}")
    if(DEFINED ${WIDGET_SWITCH} AND ${WIDGET_SWITCH})
        file(GLOB_RECURSE TMP_HEADERS "${REALGUI_DIR}/widget/${WIDGET_DIR}/*.h")
        list(APPEND HEADER_FILES ${TMP_HEADERS})
    endif()
endforeach()

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

file(GLOB_RECURSE TMP_HEADERS "${REALGUI_DIR}/3rd/nanovg/base/*.h")
list(APPEND HEADER_FILES ${TMP_HEADERS})

file(GLOB_RECURSE TMP_HEADERS "${REALGUI_DIR}/3rd/nanovg/agge/*.h")
list(APPEND HEADER_FILES ${TMP_HEADERS})

# ezXML headers - conditionally included based on config
if(DEFINED CONFIG_REALTEK_BUILD_XML_LOADER AND CONFIG_REALTEK_BUILD_XML_LOADER)
    file(GLOB_RECURSE TMP_HEADERS "${REALGUI_DIR}/3rd/ezXML/*.h")
    list(APPEND HEADER_FILES ${TMP_HEADERS})
endif()

file(GLOB_RECURSE TMP_HEADERS "${REALGUI_DIR}/3rd/gifdec/*.h")
list(APPEND HEADER_FILES ${TMP_HEADERS})

# Lite3D headers - conditionally included based on config
if(DEFINED CONFIG_REALTEK_BUILD_LITE3D AND CONFIG_REALTEK_BUILD_LITE3D)
    file(GLOB_RECURSE TMP_HEADERS "${REALGUI_DIR}/3rd/Lite3D/widget/*.h")
    list(APPEND HEADER_FILES ${TMP_HEADERS})

endif()

# h264bsd headers - follow CONFIG_REALTEK_H264BSD, the switch that builds them
if(DEFINED CONFIG_REALTEK_H264BSD AND CONFIG_REALTEK_H264BSD)
    file(GLOB_RECURSE TMP_HEADERS "${REALGUI_DIR}/3rd/h264bsd/inc/*.h")
    list(APPEND HEADER_FILES ${TMP_HEADERS})
endif()

# Particle system headers - conditionally included based on config
if(DEFINED CONFIG_REALTEK_BUILD_PARTICLE_SYSTEM AND CONFIG_REALTEK_BUILD_PARTICLE_SYSTEM)
    file(GLOB_RECURSE TMP_HEADERS "${REALGUI_DIR}/3rd/particle/*.h")
    list(APPEND HEADER_FILES ${TMP_HEADERS})
endif()

# Letter shell headers - conditionally included based on config
if(DEFINED CONFIG_REALTEK_BUILD_LETTER_SHELL AND CONFIG_REALTEK_BUILD_LETTER_SHELL)
    file(GLOB_RECURSE TMP_HEADERS "${REALGUI_DIR}/3rd/letter_shell/*.h")
    list(APPEND HEADER_FILES ${TMP_HEADERS})
endif()

# Lite3D vendors its own gui_version.h, which lands on the same
# include/gui_version.h as realgui/gui_version.h: the copy that won depended on
# the install order, so two builds of the same source shipped different version
# headers. The engine compiles with realgui/gui_version.h, so drop the other.
list(FILTER HEADER_FILES EXCLUDE REGEX "/3rd/Lite3D/widget/gui_version[.]h$")

# Clean include/ first - drop headers of removed or disabled code
install(CODE "file(REMOVE_RECURSE \"\${CMAKE_INSTALL_PREFIX}/include\")")

install(FILES ${HEADER_FILES} DESTINATION include)

# Box2D directory installation - conditionally included
if(DEFINED CONFIG_REALTEK_BUILD_GUI_BOX2D AND CONFIG_REALTEK_BUILD_GUI_BOX2D)
    install(DIRECTORY "${REALGUI_DIR}/3rd/box2d/include/box2d" DESTINATION include)
endif()
