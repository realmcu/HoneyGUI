include(cmake/gui_build.cmake)
set(gui_debug_enable OFF)
if(CONFIG_LIB_MODE)
message(STATUS "LIB MODE.")
add_library(HoneyGUI STATIC
    ${gui_sources}
)

if(CMAKE_SYSTEM_PROCESSOR STREQUAL "arm")


# Check if a specific toolchain is specified
if(NOT CMAKE_TOOLCHAIN_FILE)
    message(FATAL_ERROR "Please specify a toolchain file with -DCMAKE_TOOLCHAIN_FILE=<path/to/toolchain>")
endif()

# Log the current compiler
if(CMAKE_C_COMPILER MATCHES ".*arm-none-eabi-gcc.*")
    set(COMPILER_NAME "arm-none-eabi-gcc")
elseif(CMAKE_C_COMPILER MATCHES ".*armcc.*")
    set(COMPILER_NAME "armcc")
elseif(CMAKE_C_COMPILER MATCHES ".*armclang.*")
    set(COMPILER_NAME "armclang")
else()
    set(COMPILER_NAME "Unknown Compiler")
endif()

message(STATUS "Using Compiler: ${COMPILER_NAME}")

set(LIB_DESTINATION ${CMAKE_CURRENT_LIST_DIR}/lib/${COMPILER_NAME}/)


get_target_property(LIB_OUTPUT_NAME HoneyGUI OUTPUT_NAME)
if(NOT LIB_OUTPUT_NAME)
    set(LIB_OUTPUT_NAME HoneyGUI)
endif()
set(LIB_FILE $<TARGET_FILE:HoneyGUI>)




# Source directory path
set(SOURCE_DIR  "${CMAKE_CURRENT_LIST_DIR}/realgui")

# Destination directory path
set(DEST_DIR  "${CMAKE_CURRENT_LIST_DIR}/lib/${COMPILER_NAME}/include")


add_custom_command(TARGET HoneyGUI POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy ${LIB_FILE} ${LIB_DESTINATION}/lib${LIB_OUTPUT_NAME}${CMAKE_STATIC_LIBRARY_SUFFIX}
    COMMENT "Copying ${LIB_OUTPUT_NAME} to ${LIB_DESTINATION}"
    COMMAND ${CMAKE_COMMAND} -D SOURCE_DIR=${SOURCE_DIR} -D DEST_DIR=${DEST_DIR} -P ${CMAKE_CURRENT_LIST_DIR}/lib/arm-none-eabi-gcc/lib.cmake
	COMMAND objdump -t ${LIB_FILE} > ${CMAKE_CURRENT_LIST_DIR}/lib/${COMPILER_NAME}/function_list.txt
)
 
endif()


endif()
if(CONFIG_LVGL)
set(LVGL_ROOT_DIR ${CMAKE_CURRENT_LIST_DIR}/lvgl_v8/lvgl)

file(GLOB_RECURSE SOURCES ${LVGL_ROOT_DIR}/src/*.c)
file(GLOB_RECURSE EXAMPLE_SOURCES ${LVGL_ROOT_DIR}/examples/*.c)
file(GLOB_RECURSE DEMO_SOURCES ${LVGL_ROOT_DIR}/demos/*.c)
set(lvgl_sources ${SOURCES} CACHE INTERNAL  "sources")
set(lvgl_example_sources ${EXAMPLE_SOURCES} CACHE INTERNAL  "sources")
set(lvgl_demo_sources ${DEMO_SOURCES} CACHE INTERNAL  "sources")
set(lvgl_include 
${LVGL_ROOT_DIR}/examples
${LVGL_ROOT_DIR}/demos
${LVGL_ROOT_DIR}
${LVGL_ROOT_DIR}/demos/widgets
 CACHE INTERNAL  "include")
endif()
    add_subdirectory(realgui)
	#gui_sources/gui_headers was set as cache variable
#if gui sources is desired, for example
#   target_source(example_target PRIVATE ${gui_sources})
list(APPEND gui_sources 
			${gui_app_sources} 
			${gui_dc_sources} 
			${gui_engine_sources}
			${gui_input_sources} 
			${gui_server_sources} 
			${gui_widget_sources}
			${gui_misc_tlsf_sources}
			${gui_engine_core_sources}
			${gui_3rd_sources}
			
			${gui_dom_sources}
			${gui_SaaA_sources}
			)
if(CONFIG_LVGL)
    list(APPEND gui_sources 
			${lvgl_sources} 
			${lvgl_example_sources} 
			${lvgl_demo_sources}
			
			)
			
endif()
set(gui_sources ${gui_sources} CACHE INTERNAL  "gui sources ")

list(APPEND gui_headers
			${gui_app_headers} 
			${gui_dc_headers} 
			${gui_engine_headers}
			${gui_input_headers} 
			${gui_server_headers} 
			${gui_widget_headers}
			${gui_3rd_nanovg_headers}
			${gui_3rd_nanosvg_headers}
			${gui_misc_headers}
            ${gui_3rd_common_headers}
            ${gui_3rd_headers}
			${gui_misc_tlsf_headers}
			${gui_engine_core_headers}
			
			${gui_dom_headers}
			${gui_SaaA_headers}
			)

if(CONFIG_LVGL)
    list(APPEND gui_headers 
			${lvgl_include} 

			
			)
endif()



set(gui_headers ${gui_headers} CACHE INTERNAL  "gui headers ")


if(CONFIG_LIB_MODE)


target_sources(HoneyGUI PRIVATE ${gui_sources})

# Include directories
target_include_directories(HoneyGUI PUBLIC
    ${gui_headers}
)


endif()


if(gui_debug_enable)
foreach(ITEM ${gui_sources})
   message(STATUS [GUI][srcs]${ITEM})
endforeach()

foreach(ITEM ${gui_headers})
   message(STATUS [GUI][headers]${ITEM})
endforeach()
endif()
