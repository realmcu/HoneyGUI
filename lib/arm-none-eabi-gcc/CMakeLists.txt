cmake_minimum_required (VERSION 3.10)
cmake_policy(SET CMP0123 NEW)

set(CMAKE_SYSTEM_NAME Generic)

set(ARMCLANG_PATH "C:/Program Files (x86)/Arm GNU Toolchain arm-none-eabi/13.2 Rel1/bin")

set(CMAKE_C_COMPILER "${ARMCLANG_PATH}/arm-none-eabi-gcc.exe")
set(CMAKE_CXX_COMPILER "${ARMCLANG_PATH}/arm-none-eabi-g++.exe")


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
message("build SOC = ${SOC}")

# Set the SOC file path
set(SOC_FILE "${CMAKE_CURRENT_SOURCE_DIR}/${SOC}.cmake")

# Check if the SOC file exists and include it
if(EXISTS "${SOC_FILE}")
    include("${SOC_FILE}")
else()
    message(FATAL_ERROR "SOC file ${SOC_FILE} does not exist.")
endif()

execute_process( 
				COMMAND 
				python 
				${CMAKE_CURRENT_SOURCE_DIR}/../../tool/git_generate_version.py 
				${CMAKE_CURRENT_SOURCE_DIR}/../../realgui/
				WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
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
# set_target_properties(${GUI_TARGET_NAME} PROPERTIES PREFIX "" OUTPUT_NAME ${GUI_TARGET_NAME} SUFFIX ".lib")

add_subdirectory(../../realgui build/realgui)

install(TARGETS ${GUI_TARGET_NAME}
        ARCHIVE DESTINATION lib
        LIBRARY DESTINATION lib
        RUNTIME DESTINATION bin)

file(GLOB_RECURSE NEW_HEADER_FILES "../../realgui/gui_version.h")
list(APPEND HEADER_FILES ${NEW_HEADER_FILES})		
file(GLOB_RECURSE NEW_HEADER_FILES "../../realgui/widget/*.h")
list(FILTER NEW_HEADER_FILES EXCLUDE REGEX "/gui_todo_widget/")
list(APPEND HEADER_FILES ${NEW_HEADER_FILES})
file(GLOB_RECURSE NEW_HEADER_FILES "../../realgui/server/*.h")
list(APPEND HEADER_FILES ${NEW_HEADER_FILES})
file(GLOB_RECURSE NEW_HEADER_FILES "../../realgui/core/*.h")
list(APPEND HEADER_FILES ${NEW_HEADER_FILES})
file(GLOB_RECURSE NEW_HEADER_FILES "../../realgui/app/*.h")
list(APPEND HEADER_FILES ${NEW_HEADER_FILES})
file(GLOB_RECURSE NEW_HEADER_FILES "../../realgui/dc/*.h")
list(APPEND HEADER_FILES ${NEW_HEADER_FILES})
file(GLOB_RECURSE NEW_HEADER_FILES "../../realgui/input/*.h")
list(APPEND HEADER_FILES ${NEW_HEADER_FILES})
file(GLOB_RECURSE NEW_HEADER_FILES "../../realgui/engine/*.h")
list(APPEND HEADER_FILES ${NEW_HEADER_FILES})
file(GLOB_RECURSE NEW_HEADER_FILES "../../realgui/3rd/nanovg/base/*.h")
list(APPEND HEADER_FILES ${NEW_HEADER_FILES})

install(FILES ${HEADER_FILES} DESTINATION include)

install(DIRECTORY "../../realgui/3rd/box2d/include/box2d" DESTINATION "include")

install(FILES ${SRC_FILES} DESTINATION src)
