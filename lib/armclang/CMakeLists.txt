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



set(CMAKE_TRY_COMPILE_TARGET_TYPE STATIC_LIBRARY)

set(CMAKE_INSTALL_PREFIX "${CMAKE_CURRENT_SOURCE_DIR}/install" CACHE PATH "Installation Directory" FORCE)

# Default SOC
set(DEFAULT_SOC "RTL8762G")

# Check if SOC is defined
if(NOT DEFINED SOC)
    message(WARNING "SOC variable not specified. Using default SOC: ${DEFAULT_SOC}.")
    set(SOC "${DEFAULT_SOC}")
endif()

# Set the SOC file path
set(SOC_FILE "${CMAKE_CURRENT_SOURCE_DIR}/${SOC}.cmake")

# Check if the SOC file exists and include it
if(EXISTS "${SOC_FILE}")
    include("${SOC_FILE}")
else()
    message(FATAL_ERROR "SOC file ${SOC_FILE} does not exist.")
endif()


find_program(menuconfig_EXECUTABLE menuconfig NO_CMAKE_FIND_ROOTO_PATH)
set(menuconfig_ARGS "${CMAKE_CURRENT_SOURCE_DIR}/../../Kconfig.gui")
execute_process( COMMAND 
				 ${menuconfig_EXECUTABLE} 
				 ${menuconfig_ARGS} 
				)

execute_process( 
				COMMAND 
				python 
				${CMAKE_CURRENT_SOURCE_DIR}/../../tool/convert_dot_config_to.py 
				${CMAKE_CURRENT_SOURCE_DIR}/.config 
				${CMAKE_CURRENT_SOURCE_DIR}/config.cmake
				WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
				)


include(${CMAKE_CURRENT_SOURCE_DIR}/config.cmake)
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




