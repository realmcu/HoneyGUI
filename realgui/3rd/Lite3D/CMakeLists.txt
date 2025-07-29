# Copyright (c) 2024 Realtek Semiconductor Corp.
# SPDX-License-Identifier: Apache-2.0

cmake_minimum_required(VERSION 3.10)
if(POLICY CMP0079)
  cmake_policy(SET CMP0079 NEW)
endif()

# Define a variable for the library name
if(CONFIG_REALTEK_BUILD_LITE3D) #set to 1 to enable or variable to for depnedency

    file(GLOB SOURCES "*.c" "*.cpp")
    file(GLOB HEADERS "*.h" "*.hpp")

    target_include_directories(${GUI_TARGET_NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)

    if(CONFIG_REALTEK_BUILD_LITE3D_FOR_WIN32_GCC_LIB)
        target_link_libraries(${GUI_TARGET_NAME} PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/lib/libLite3D_GCC.a)
    endif()
    if(CONFIG_REALTEK_BUILD_LITE3D_8773E_ARMCC_LIB)
        target_link_libraries(${GUI_TARGET_NAME} PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/lib/Lite3D_RTL8773E_ARMCC.lib)
    endif()
    if(CONFIG_REALTEK_BUILD_LITE3D_8773E_ARMCL_LIB)
        target_link_libraries(${GUI_TARGET_NAME} PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/lib/Lite3D_RTL8773E_ARMCLANG.lib)
    endif()
    if(CONFIG_REALTEK_BUILD_LITE3D_8773G_ARMCL_LIB)
        target_link_libraries(${GUI_TARGET_NAME} PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/lib/Lite3D_RTL8773G_ARMCLANG.lib)
    endif()
    if(CONFIG_REALTEK_BUILD_LITE3D_8773G_ZEPHYR_LIB)
        target_link_libraries(${GUI_TARGET_NAME} PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/lib/Lite3D_RTL8773G_ZEPHYR.a)
    endif()

endif()
