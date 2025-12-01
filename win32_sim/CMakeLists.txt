cmake_minimum_required (VERSION 3.13)

SET(CMAKE_C_COMPILER gcc)
SET(CMAKE_CXX_COMPILER g++)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_EXTENSIONS ON) 

set(CMAKE_C_STANDARD 99)
set(CMAKE_C_EXTENSIONS ON)

set(CMAKE_VERBOSE_MAKEFILE OFF)


project(HoneyGUI)

execute_process(
    COMMAND python ${CMAKE_SOURCE_DIR}/../tool/convert_dot_config_to.py ${CMAKE_SOURCE_DIR}/.config ${CMAKE_SOURCE_DIR}/config.cmake
    RESULT_VARIABLE result
    ERROR_VARIABLE error_output
    OUTPUT_VARIABLE output
)

# Check result of the process
if(result)
    message(FATAL_ERROR "Python script failed with error: ${error_output}")
else()
    message(STATUS "Python script output: ${output}")
endif()

# Include the generated config
include(${CMAKE_SOURCE_DIR}/config.cmake)

# Screen size and demo-specific definitions are set in each application's CMakeLists.txt



if(CMAKE_SYSTEM_PROCESSOR STREQUAL "arm")
    message(STATUS "Target processor is ARM.")
    set(GUI_PLATFORM_EMBEDDED ON)
else()
	message(STATUS "Target processor is X86.")
	set(GUI_PLATFORM_SIMULATOR ON)
endif()

set(GUI_BASE_PATH "${CMAKE_CURRENT_SOURCE_DIR}")
message("HoneyGUI base path ${GUI_BASE_PATH}")

message(STATUS "GUI_PLATFORM_SIMULATOR: ${GUI_PLATFORM_SIMULATOR}")
message(STATUS "GUI_PLATFORM_EMBEDDED: ${GUI_PLATFORM_EMBEDDED}")

set(GUI_TARGET_NAME gui)

add_executable(${GUI_TARGET_NAME} main.c)

# Add SDL2 include directories
target_include_directories(${GUI_TARGET_NAME} PRIVATE 
    ${CMAKE_CURRENT_SOURCE_DIR}/../lib/sim/SDL2-2.26.0-STATIC/include
    ${CMAKE_CURRENT_SOURCE_DIR}/../lib/sim/SDL2-2.26.0-STATIC/include/SDL2
)

add_subdirectory(../realgui realgui)
add_subdirectory(../example example)
add_subdirectory(../lib/sim sim)
add_subdirectory(port port)


set(SDL2_LIB_PATH "${CMAKE_CURRENT_SOURCE_DIR}/../lib/sim/SDL2-2.26.0-STATIC/lib")

find_library(SDL2_MAIN_LIB SDL2main PATHS ${SDL2_LIB_PATH} NO_DEFAULT_PATH)
find_library(SDL2_LIB SDL2 PATHS ${SDL2_LIB_PATH} NO_DEFAULT_PATH)

# Link libraries in correct order for static linking
target_link_libraries(${GUI_TARGET_NAME} PRIVATE 
    mingw32
    ${SDL2_MAIN_LIB}
    ${SDL2_LIB}
    setupapi
    imm32
    version
    winmm
    gdi32
    user32
    shell32
    opengl32
    -static-libgcc
    -static-libstdc++
)

set(CUSTOM_LINKER_SCRIPT "${CMAKE_CURRENT_SOURCE_DIR}/honeygui_mingw.ld")

target_link_options(${GUI_TARGET_NAME} PRIVATE "-Wl,-Map=${GUI_TARGET_NAME}.map")
target_link_options(${GUI_TARGET_NAME} PRIVATE "-T${CUSTOM_LINKER_SCRIPT}")




