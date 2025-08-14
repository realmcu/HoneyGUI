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

# Default SOC
set(DEFAULT_SOC "RTL8762G")

# Check if SOC is defined
if(NOT DEFINED SOC)
    message(WARNING "SOC variable not specified. Using default SOC: ${DEFAULT_SOC}.")
    set(SOC "${DEFAULT_SOC}")
endif()
message("build soc = ${SOC}")

# Set the SOC file path
set(SOC_FILE "${CMAKE_CURRENT_SOURCE_DIR}/${SOC}.cmake")

# Check if the SOC file exists and include it
if(EXISTS "${SOC_FILE}")
    include("${SOC_FILE}")
else()
    message(FATAL_ERROR "SOC file ${SOC_FILE} does not exist.")
endif()

set(CMAKE_INSTALL_PREFIX "${CMAKE_CURRENT_SOURCE_DIR}/${SOC}/install" CACHE PATH "Installation Directory" FORCE)

execute_process( 
				COMMAND 
				python 
				${CMAKE_CURRENT_SOURCE_DIR}/../../../../../tool/git_generate_version.py 
				${CMAKE_CURRENT_SOURCE_DIR}/../../widget
				WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
				)

execute_process( 
				COMMAND 
				python 
				${CMAKE_CURRENT_SOURCE_DIR}/../../../../../tool/convert_dot_config_to.py 
				${CMAKE_CURRENT_SOURCE_DIR}/.config 
				${CMAKE_CURRENT_SOURCE_DIR}/config.cmake
				WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
				)


include(${CMAKE_CURRENT_SOURCE_DIR}/config.cmake)
project(Lite3D)

set(GUI_TARGET_NAME Lite3D)
set(LIBRARY_OUTPUT_NAME "${GUI_TARGET_NAME}_${SOC}_ARMCLANG")

add_library(${GUI_TARGET_NAME} STATIC)
set_target_properties(${GUI_TARGET_NAME} PROPERTIES PREFIX "" OUTPUT_NAME ${LIBRARY_OUTPUT_NAME} SUFFIX ".lib")

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

install(FILES ${SRC_FILES} DESTINATION src)



