cmake_minimum_required (VERSION 3.13)

SET(CMAKE_C_COMPILER gcc)
SET(CMAKE_CXX_COMPILER g++)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_EXTENSIONS ON) 

set(CMAKE_C_STANDARD 99)
set(CMAKE_C_EXTENSIONS ON)

set(CMAKE_VERBOSE_MAKEFILE OFF)


project(HoneyGUI)

# execute_process(
#     COMMAND python ${CMAKE_SOURCE_DIR}/../../tool/parse_kconfig.py ${CMAKE_SOURCE_DIR}/.config ${CMAKE_SOURCE_DIR}/config.cmake
#     RESULT_VARIABLE result
#     ERROR_VARIABLE error_output
#     OUTPUT_VARIABLE output
# )

# # Check result of the process
# if(result)
#     message(FATAL_ERROR "Python script failed with error: ${error_output}")
# else()
#     message(STATUS "Python script output: ${output}")
# endif()



if(CMAKE_SYSTEM_PROCESSOR STREQUAL "arm")
    message(STATUS "Target processor is ARM.")
else()
	message(STATUS "Target processor is X86.")  
endif()

set(GUI_BASE_PATH "${CMAKE_CURRENT_SOURCE_DIR}")
message("HoneyGUI base path ${GUI_BASE_PATH}")


set(GUI_TARGET_NAME gui)

add_executable(${GUI_TARGET_NAME} main.c)
add_subdirectory(../realgui build/realgui)
add_subdirectory(port build/port)
add_subdirectory(RTE build/RTE)


set(SDL2_LIB_PATH "./RTE/SDL2-2.26.0/x86_64-w64-mingw32/lib")

find_library(SDL2_MAIN_LIB SDL2main PATHS ${SDL2_LIB_PATH})
find_library(SDL2_LIB SDL2 PATHS ${SDL2_LIB_PATH})

target_link_libraries(${GUI_TARGET_NAME} PRIVATE ${SDL2_MAIN_LIB} ${SDL2_LIB})
target_link_libraries(${GUI_TARGET_NAME} PRIVATE opengl32)
target_link_libraries(${GUI_TARGET_NAME} PRIVATE mingw32)

set(CUSTOM_LINKER_SCRIPT "${CMAKE_CURRENT_SOURCE_DIR}/default.ld")


target_link_options(${GUI_TARGET_NAME} PRIVATE "-Wl,-Map=${GUI_TARGET_NAME}.map")
target_link_options(${GUI_TARGET_NAME} PRIVATE "-T${CUSTOM_LINKER_SCRIPT}")




