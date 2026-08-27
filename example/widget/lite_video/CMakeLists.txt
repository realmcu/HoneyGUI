cmake_minimum_required(VERSION 3.10)

if(CONFIG_REALTEK_BUILD_EXAMPLE_LITE_VIDEO_WIDGET)
    target_sources(${GUI_TARGET_NAME} PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/example_lite_video.c
    )
    target_include_directories(${GUI_TARGET_NAME} PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}
        ${CMAKE_CURRENT_SOURCE_DIR}/root_image
    )
    target_compile_definitions(${GUI_TARGET_NAME} PRIVATE
        DRV_LCD_WIDTH=360
        DRV_LCD_HEIGHT=360
        DRV_PIXEL_BITS=16
        DRV_LCD_CORNER_RADIUS=0
    )
endif()
