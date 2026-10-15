#
# Copyright (c) 2026, Realtek Semiconductor Corporation
#
# SPDX-License-Identifier: MIT
#

# Browser (WebAssembly) simulator. Build with the Emscripten toolchain:
#
#   emcmake cmake -S web_sim -B web_sim/build -G Ninja
#   cmake --build web_sim/build
#
# Produces gui.js (with the wasm embedded). Run "python web_sim/pack_files.py"
# once, then open web_sim/index.html by double-clicking it.
#
# The demo is selected the same way as win32_sim, through web_sim/.config.
# web_sim/files.json lists the runtime files the demo reads (its romfs image);
# update it and re-run pack_files.py after selecting a different demo.
#
# Options:
#   WEB_SIM_CONFIG       .config to use instead of web_sim/.config, for example
#                        web_sim/.config_engine (all widgets, no demo app, no
#                        letter_shell which needs a GNU linker script)
#   WEB_SIM_MAIN_MODULE  build the engine as a main module that can load a
#                        project compiled separately as a side module at run
#                        time (-sSIDE_MODULE). Implies position independent
#                        code and a separate gui.wasm.

cmake_minimum_required(VERSION 3.13)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_EXTENSIONS ON)
set(CMAKE_C_STANDARD 99)
set(CMAKE_C_EXTENSIONS ON)

project(HoneyGUI_web_sim)

if(NOT EMSCRIPTEN)
    message(FATAL_ERROR "web_sim must be configured with the Emscripten toolchain (emcmake cmake ...)")
endif()

# An unoptimized Asyncify build is much larger and slower, so optimize when no
# build type is given. Not the Release type: its NDEBUG would turn off assert(),
# unlike win32_sim. Pass -DCMAKE_BUILD_TYPE=Debug for debugging.
if(NOT CMAKE_BUILD_TYPE)
    add_compile_options(-O2)
    add_link_options(-O2)
endif()

set(WEB_SIM_CONFIG "${CMAKE_SOURCE_DIR}/.config" CACHE FILEPATH
    ".config that selects the widgets and the demo app")
option(WEB_SIM_MAIN_MODULE "Build the engine as a main module for run-time loaded projects" OFF)

find_package(Python3 COMPONENTS Interpreter REQUIRED)

set(WEB_SIM_CONFIG_CMAKE ${CMAKE_BINARY_DIR}/config.cmake)

execute_process(
    COMMAND ${Python3_EXECUTABLE}
            ${CMAKE_SOURCE_DIR}/../tool/convert_dot_config_to.py
            ${WEB_SIM_CONFIG}
            ${WEB_SIM_CONFIG_CMAKE}
    RESULT_VARIABLE result
    ERROR_VARIABLE error_output
)
if(result)
    message(FATAL_ERROR "Python script failed with error: ${error_output}")
endif()

include(${WEB_SIM_CONFIG_CMAKE})

set(GUI_PLATFORM_SIMULATOR ON)
set(GUI_BASE_PATH "${CMAKE_CURRENT_SOURCE_DIR}")

set(GUI_TARGET_NAME gui)

if(WEB_SIM_MAIN_MODULE)
    # Side modules are position independent, so the engine they link against
    # at run time must be too.
    add_compile_options(-fPIC)
endif()

add_executable(${GUI_TARGET_NAME} main.c)

target_compile_definitions(${GUI_TARGET_NAME} PRIVATE _HONEYGUI_SIMULATOR_)

add_subdirectory(../realgui realgui)
add_subdirectory(../example example)
add_subdirectory(port port)

get_property(GUI_BINARY_RESOURCES GLOBAL PROPERTY GUI_BINARY_RESOURCES)
if(GUI_BINARY_RESOURCES)
    target_link_libraries(${GUI_TARGET_NAME} PRIVATE ${GUI_BINARY_RESOURCES})
endif()

# Asyncify lets gui_thread_mdelay() yield to the browser from inside the
# GUI server loop (see port/gui_port_os.c).
target_link_options(${GUI_TARGET_NAME} PRIVATE
    -sASYNCIFY=1
    -sASYNCIFY_STACK_SIZE=65536
    -sALLOW_MEMORY_GROWTH=1
    -sINITIAL_MEMORY=64MB
    -sSTACK_SIZE=1MB
    -sMODULARIZE=1
    -sEXPORT_NAME=createHoneyGUI
    -sEXPORTED_RUNTIME_METHODS=FS,HEAPU8,callMain
    -sINVOKE_RUN=0
    -sFORCE_FILESYSTEM=1
    -sENVIRONMENT=web,worker,node
)

if(WEB_SIM_MAIN_MODULE)
    # Keep every engine symbol exported: the project module is linked against
    # them at load time, and the linker cannot know which it will use.
    target_link_options(${GUI_TARGET_NAME} PRIVATE
        -sMAIN_MODULE=1
        -sEXPORTED_RUNTIME_METHODS=FS,HEAPU8,callMain,loadDynamicLibrary)
else()
    # SINGLE_FILE embeds the .wasm in gui.js. Browsers refuse to fetch a
    # separate .wasm file for pages opened from disk (file://), and embedding
    # it is what lets index.html work by double-clicking it. Emscripten 4.0.18
    # and later embed it as UTF-8 text rather than base64, so a page that
    # loads gui.js must be decoded as UTF-8 (index.html declares it); in any
    # other encoding the wasm fails to compile.
    target_link_options(${GUI_TARGET_NAME} PRIVATE -sSINGLE_FILE=1)
endif()
