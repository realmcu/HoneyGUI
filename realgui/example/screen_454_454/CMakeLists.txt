
# Define the function
function(target_exists TARGET_NAME RESULT_VAR)
    if (TARGET ${TARGET_NAME})
        set(${RESULT_VAR} TRUE PARENT_SCOPE)
    else()
        set(${RESULT_VAR} FALSE PARENT_SCOPE)
    endif()
endfunction()

# Check if MyExecutable exists
target_exists(gui TARGET_FOUND)

if (TARGET_FOUND)
    target_compile_definitions(gui PRIVATE ENABLE_RTK_GUI_454_454_DEMO DRV_LCD_WIDTH=454 DRV_LCD_HIGHT=454 DRV_PIXEL_BITS=16)

else()
    
endif()
# Define macros for the specific target


file(GLOB gui_example_sources 
gui_benchmark/*.c 
gui_call/*.c 
gui_common/*.c 
gui_contacts/*.c 
gui_menu/*.c 
gui_player/*.c 
gui_record/*.c 
gui_watchface/*.c 
gui_menu/*.cpp
*.c )


set(gui_example_sources ${gui_example_sources} CACHE INTERNAL  "sources")

HEADER_DIRECTORIES(gui_example_headers ./)

set(gui_example_headers ${gui_example_headers} 
${CMAKE_CURRENT_SOURCE_DIR}/gui_benchmark
${CMAKE_CURRENT_SOURCE_DIR}/gui_call
${CMAKE_CURRENT_SOURCE_DIR}/gui_common
${CMAKE_CURRENT_SOURCE_DIR}/gui_contacts
${CMAKE_CURRENT_SOURCE_DIR}/gui_menu
${CMAKE_CURRENT_SOURCE_DIR}/gui_player
${CMAKE_CURRENT_SOURCE_DIR}/gui_record
${CMAKE_CURRENT_SOURCE_DIR}/gui_watchface
${CMAKE_CURRENT_SOURCE_DIR}/root_image
${CMAKE_CURRENT_SOURCE_DIR}
CACHE INTERNAL  "headers")


if(gui_debug_enable)  
   message(--------------example-------------)
foreach(ITEM ${gui_example_sources})
   message(${ITEM})
endforeach()
endif()

