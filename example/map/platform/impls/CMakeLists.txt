cmake_minimum_required(VERSION 3.10)

file(GLOB MAP_PLATFORM_IMPL_SOURCES "*.c" "*.cpp")
target_sources(${GUI_TARGET_NAME} PRIVATE ${MAP_PLATFORM_IMPL_SOURCES})
target_include_directories(${GUI_TARGET_NAME} PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
)

add_subdirectory(honeygui)
add_subdirectory(posix)
