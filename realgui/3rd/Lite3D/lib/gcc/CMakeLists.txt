#
# Copyright (c) 2026, Realtek Semiconductor Corporation
#
# SPDX-License-Identifier: Apache-2.0
#

cmake_minimum_required (VERSION 3.10)


set(CMAKE_SYSTEM_NAME Generic)
SET(CMAKE_C_COMPILER gcc)
SET(CMAKE_CXX_COMPILER g++)

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -m64")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -m64")

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_EXTENSIONS ON) 
set(CMAKE_C_STANDARD 99)
set(CMAKE_C_EXTENSIONS ON)

set(CMAKE_VERBOSE_MAKEFILE ON)


set(CMAKE_TRY_COMPILE_TARGET_TYPE STATIC_LIBRARY)

set(CMAKE_INSTALL_PREFIX "${CMAKE_CURRENT_SOURCE_DIR}/install" CACHE PATH "Installation Directory" FORCE)


execute_process( 
        COMMAND 
        python 
        ${CMAKE_CURRENT_SOURCE_DIR}/../../../../../tool/git_generate_version.py 
        ${CMAKE_CURRENT_SOURCE_DIR}/../../widget
        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
        )

project(Lite3D)



set(GUI_TARGET_NAME Lite3D)

add_library(${GUI_TARGET_NAME} STATIC)

set(OUTPUT_LIB_NAME "${GUI_TARGET_NAME}_GCC")
set_target_properties(
    ${GUI_TARGET_NAME}
    PROPERTIES
        OUTPUT_NAME "${OUTPUT_LIB_NAME}"                     
        SUFFIX ".a"                     
)


add_subdirectory(../../widget build/widget)

install(TARGETS ${GUI_TARGET_NAME}
        ARCHIVE DESTINATION lib
        LIBRARY DESTINATION lib
        RUNTIME DESTINATION bin)

file(GLOB_RECURSE NEW_HEADER_FILES "../../widget/l3.h")
list(APPEND HEADER_FILES ${NEW_HEADER_FILES})

file(GLOB_RECURSE NEW_HEADER_FILES "../../widget/l3_common.h")
list(APPEND HEADER_FILES ${NEW_HEADER_FILES})

file(GLOB_RECURSE NEW_HEADER_FILES "../../widget/l3_rect.h")
list(APPEND HEADER_FILES ${NEW_HEADER_FILES})

file(GLOB_RECURSE NEW_HEADER_FILES "../../widget/l3_tria.h")
list(APPEND HEADER_FILES ${NEW_HEADER_FILES})

file(GLOB_RECURSE NEW_HEADER_FILES "../../widget/gui_version.h")
list(APPEND HEADER_FILES ${NEW_HEADER_FILES})


install(FILES ${HEADER_FILES} DESTINATION include)
