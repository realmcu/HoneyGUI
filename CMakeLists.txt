cmake_minimum_required(VERSION 3.28)

# Determine LVGL path based on configuration
# Check if LVGL_V9 is defined
if(CONFIG_REALTEK_APP_LVGL_DEMO_SUPPORT)
    # Set LVGL path and add the LVGL subdirectory
    set(LVGL_PATH "${CMAKE_CURRENT_SOURCE_DIR}/lvgl_v9/lvgl")
    message(STATUS "[CONFIG_REALTEK_APP_LVGL_DEMO_SUPPORT] target: ${LVGL_PATH}")
    add_subdirectory(${LVGL_PATH})
    
else()
    # Only create guiLib if CONFIG_REALTEK_BUILD_LVGL_V9 is NOT defined
    set(GUI_TARGET_NAME guiLib)

    if(ZEPHYR_BASE)
        message(STATUS "ZEPHYR_BASE is set to: ${ZEPHYR_BASE}")
        add_library(${GUI_TARGET_NAME} STATIC)
        target_link_libraries(${GUI_TARGET_NAME} PRIVATE zephyr_interface)
    else()
        add_library(${GUI_TARGET_NAME})
    endif()

    # Add the realgui subdirectory for additional setup
    add_subdirectory(realgui)

    # Output compile information for clarity
    message(STATUS "[Compile info] target: ${GUI_TARGET_NAME}")
endif()
