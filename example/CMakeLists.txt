#
# Copyright (c) 2026, Realtek Semiconductor Corporation
#
# SPDX-License-Identifier: MIT
#

cmake_minimum_required(VERSION 3.10)

# gui_add_binary_resource(), for demos that embed a file instead of reading it
# from a filesystem. Defined in lib/ because it is not example-specific; made
# available here so the demo CMakeLists below can keep calling it.
include(${CMAKE_CURRENT_LIST_DIR}/../lib/embed_binary_resource.cmake)

# The demos. A simulator adds this directory only when its .config selected one
# (see lib/add_example_demos.cmake), and each demo gates its own sources on its
# own option, so building with no demo selected contributes nothing here.

# Sources directly in this directory are shared by every demo, so there should
# be none. Globbing them unconditionally would drop a stray file into builds
# that never asked for it (including demo-free engines), so fail loudly instead.
file(GLOB STRAY_SOURCES "*.c" "*.cpp")
if(STRAY_SOURCES)
    message(FATAL_ERROR
        "Unexpected top-level source files in example/: ${STRAY_SOURCES}. "
        "Put them in the directory of the demo that should build them.")
endif()

file(GLOB SUBDIRS RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} ${CMAKE_CURRENT_SOURCE_DIR}/*)

foreach(subdir ${SUBDIRS})
    if(IS_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/${subdir})
        if(EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/${subdir}/CMakeLists.txt)
            add_subdirectory(${subdir})
        endif()
    endif()
endforeach()
