cmake_minimum_required(VERSION 3.10)

file(GLOB MAP_TRVG_SOURCES "*.c" "*.cpp")
target_sources(${GUI_TARGET_NAME} PRIVATE ${MAP_TRVG_SOURCES})
target_include_directories(${GUI_TARGET_NAME} PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
)
