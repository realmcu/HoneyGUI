cmake_minimum_required(VERSION 3.10)

if(1)

    file(GLOB SOURCES "*.c")
    if(SOURCES)
        target_sources(${GUI_TARGET_NAME} PRIVATE ${SOURCES})
        target_include_directories(${GUI_TARGET_NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
    endif()

endif()
