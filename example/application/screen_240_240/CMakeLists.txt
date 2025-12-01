# CMakeLists.txt for screen_240_240 demo
cmake_minimum_required(VERSION 3.10)

if(CONFIG_REALTEK_BUILD_GUI_240_240_DEMO)
    # Collect all source files
    file(GLOB SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/*.c")
    
    # Convert binary resource file to object file using objcopy (Simulator only)
    if(GUI_PLATFORM_SIMULATOR)
        set(BIN_FILE "${CMAKE_CURRENT_SOURCE_DIR}/root_image/root_0x859000.bin")
        set(OBJ_FILE "${CMAKE_CURRENT_SOURCE_DIR}/root_image/root_0x859000.o")
        
        if(EXISTS ${BIN_FILE})
            add_custom_command(
                OUTPUT ${OBJ_FILE}
                COMMAND objcopy -I binary -O elf64-x86-64 --binary-architecture i386:x86-64
                        "root_0x859000.bin" "${OBJ_FILE}"
                DEPENDS ${BIN_FILE}
                WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/root_image
                COMMENT "Converting root_0x859000.bin to object file"
                VERBATIM
            )
            list(APPEND SOURCES ${OBJ_FILE})
            message(STATUS "Added binary resource: ${BIN_FILE}")
        endif()
    endif()
    
    # Add sources to target
    if(SOURCES)
        target_sources(${GUI_TARGET_NAME} PRIVATE ${SOURCES})
    endif()
    
    # Add include directories
    target_include_directories(${GUI_TARGET_NAME} PUBLIC 
        ${CMAKE_CURRENT_SOURCE_DIR}
    )
    
    # Add compile definitions
    target_compile_definitions(${GUI_TARGET_NAME} PRIVATE
        ENABLE_RTK_GUI_240_240_DEMO
        DRV_LCD_WIDTH=240
        DRV_LCD_HEIGHT=240
        DRV_PIXEL_BITS=16
    )
    
    message(STATUS "Building screen_240_240 demo (240x240)")
    
    message(STATUS "Added screen_240_240 demo sources")
endif()
