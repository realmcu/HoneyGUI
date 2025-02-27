cmake_minimum_required (VERSION 3.10)
cmake_policy(SET CMP0123 NEW)

set(CMAKE_SYSTEM_NAME Generic)
set(CMAKE_C_COMPILER armclang)
set(CMAKE_CXX_COMPILER armclang)


set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_EXTENSIONS ON) 
set(CMAKE_C_STANDARD 99)
set(CMAKE_C_EXTENSIONS ON)

set(CMAKE_VERBOSE_MAKEFILE OFF)


# For ARM Cortex-M4
set(CPU_FLAGS "-mcpu=cortex-m4 -mthumb")

# Apply the flags to C and C++ compiler settings
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${CPU_FLAGS}")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${CPU_FLAGS}")



set(CMAKE_TRY_COMPILE_TARGET_TYPE STATIC_LIBRARY)

set(CMAKE_INSTALL_PREFIX "${CMAKE_CURRENT_SOURCE_DIR}/install" CACHE PATH "Installation Directory" FORCE)


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
