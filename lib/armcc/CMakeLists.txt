cmake_minimum_required (VERSION 3.10)

set(CMAKE_SYSTEM_NAME Generic)

set(ARMCLANG_PATH "C:/Keil_v5/ARM/ARMCC/bin")

set(CMAKE_C_COMPILER "${ARMCLANG_PATH}/armcc.exe")
set(CMAKE_CXX_COMPILER "${ARMCLANG_PATH}/armcc.exe")


set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_EXTENSIONS ON) 
set(CMAKE_C_STANDARD 99)
set(CMAKE_C_EXTENSIONS ON)

set(CMAKE_VERBOSE_MAKEFILE OFF)



set(CMAKE_TRY_COMPILE_TARGET_TYPE STATIC_LIBRARY)

set(CMAKE_INSTALL_PREFIX "${CMAKE_CURRENT_SOURCE_DIR}/install" CACHE PATH "Installation Directory" FORCE)

# Default SOC
set(DEFAULT_SOC "RTL8773E")

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

execute_process( 
				COMMAND 
				python 
				${CMAKE_CURRENT_SOURCE_DIR}/../../tool/git_generate_version.py 
				${CMAKE_CURRENT_SOURCE_DIR}/../../realgui/
				WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
				)

#find_program(menuconfig_EXECUTABLE menuconfig NO_CMAKE_FIND_ROOTO_PATH)
#set(menuconfig_ARGS "${CMAKE_CURRENT_SOURCE_DIR}/../../Kconfig.gui")
#execute_process( COMMAND 
#				 ${menuconfig_EXECUTABLE} 
#				 ${menuconfig_ARGS} 
#				)

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
set_target_properties(${GUI_TARGET_NAME} PROPERTIES PREFIX "" OUTPUT_NAME ${GUI_TARGET_NAME} SUFFIX ".lib")

add_subdirectory(../../realgui build/realgui)

install(TARGETS ${GUI_TARGET_NAME}
        ARCHIVE DESTINATION lib
        LIBRARY DESTINATION lib
        RUNTIME DESTINATION bin)

function(install_headers_flat source_dir)
    file(GLOB_RECURSE HEADERS 
        "${source_dir}/*.h"
        "${source_dir}/*.hpp"
    )
    
    set(FILTERED_HEADERS)
    foreach(header ${HEADERS})
        if(NOT header MATCHES "SConscript$" AND
           NOT header MATCHES "\\.(c|cpp|txt|o|a|lib)$")
            list(APPEND FILTERED_HEADERS ${header})
        endif()
    endforeach()
    
    if(FILTERED_HEADERS)
        install(FILES ${FILTERED_HEADERS} DESTINATION "include")
    endif()
endfunction()

file(GLOB_RECURSE REALGUI_HEADERS 
    "../../realgui/*.h"
    "../../realgui/*.hpp"
)
set(FILTERED_REALGUI_HEADERS)
foreach(header ${REALGUI_HEADERS})
    if(NOT header MATCHES "SConscript$" AND
       NOT header MATCHES "LICENSE$" AND
       NOT header MATCHES "\\.(c|cpp|txt|o|a|lib|gitignore)$" AND
       NOT header MATCHES ".*/(3rd|SaaA|test)/.*" AND
       NOT header MATCHES ".*widget/gui_todo_widget")
        list(APPEND FILTERED_REALGUI_HEADERS ${header})
    endif()
endforeach()
install(FILES ${FILTERED_REALGUI_HEADERS} DESTINATION "include")

if(CONFIG_REALTEK_BUILD_GUI_BOX2D)
	install_headers_flat("../../realgui/3rd/box2d/include/box2d")
endif()

if(CONFIG_REALTEK_BUILD_LITE3D)
    install_headers_flat("../../realgui/3rd/Lite3D/widget")
endif()

install_headers_flat("../../realgui/3rd/gifdec")

file(GLOB_RECURSE NANOVG_HEADERS 
    "../../realgui/3rd/nanovg/base/*.h"
    "../../realgui/3rd/nanovg/base/*.hpp"
)
set(FILTERED_NANOVG_HEADERS)
foreach(header ${NANOVG_HEADERS})
    if(NOT header MATCHES "SConscript$" AND
       NOT header MATCHES "fontstash.h$" AND
       NOT header MATCHES "\\.(c|cpp|txt|o|a|lib)$")
        list(APPEND FILTERED_NANOVG_HEADERS ${header})
    endif()
endforeach()
install(FILES ${FILTERED_NANOVG_HEADERS} DESTINATION "include")

install(FILES ${SRC_FILES} DESTINATION src)