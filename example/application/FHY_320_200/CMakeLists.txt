# CMakeLists.txt for FHY_320_200 demo
cmake_minimum_required(VERSION 3.10)

if(CONFIG_REALTEK_BUILD_GUI_FHY_320_200_DEMO)
    # Collect all source files
    file(GLOB SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/*.c")
    file(GLOB SOURCES_CPP "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp")
    
    # Add font file
    if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/font_smarttx/font_file_smarttx.c")
        list(APPEND SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/font_smarttx/font_file_smarttx.c")
    endif()
    
    # Convert binary resource file to object file using objcopy
    set(BIN_FILE "${CMAKE_CURRENT_SOURCE_DIR}/root_image/root_0x00950000.bin")
    set(OBJ_FILE "${CMAKE_CURRENT_SOURCE_DIR}/root_image/root_0x00950000_bin.o")
    
    if(EXISTS ${BIN_FILE})
        add_custom_command(
            OUTPUT ${OBJ_FILE}
            COMMAND objcopy -I binary -O elf64-x86-64 --binary-architecture i386:x86-64
                    "root_0x00950000.bin" "${OBJ_FILE}"
            DEPENDS ${BIN_FILE}
            WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/root_image
            COMMENT "Converting root_0x00950000.bin to object file"
            VERBATIM
        )
        list(APPEND SOURCES ${OBJ_FILE})
        message(STATUS "Added binary resource: ${BIN_FILE}")
    endif()
    
    # Add sources to target
    if(SOURCES OR SOURCES_CPP)
        target_sources(${GUI_TARGET_NAME} PRIVATE ${SOURCES} ${SOURCES_CPP})
    endif()
    
    # Add include directories
    target_include_directories(${GUI_TARGET_NAME} PUBLIC 
        ${CMAKE_CURRENT_SOURCE_DIR}
        ${CMAKE_CURRENT_SOURCE_DIR}/root_image
        ${CMAKE_CURRENT_SOURCE_DIR}/font_smarttx
    )
    
    # Add compile definitions
    target_compile_definitions(${GUI_TARGET_NAME} PRIVATE
        DRV_LCD_WIDTH=320
        DRV_LCD_HEIGHT=200
        DRV_PIXEL_BITS=16
    )
    
    message(STATUS "Building FHY_320_200 demo (320x200)")
    
    message(STATUS "Added FHY_320_200 demo sources")
endif()
