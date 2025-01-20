


file(GLOB gui_3rd_letter_shell_sources *.c)

set(gui_3rd_letter_shell_sources ${gui_3rd_letter_shell_sources} CACHE INTERNAL  "sources")

HEADER_DIRECTORIES(gui_3rd_letter_headers ./)

set(gui_3rd_letter_headers ${CMAKE_CURRENT_SOURCE_DIR} CACHE INTERNAL  "headers")



if(gui_debug_enable)  
   message(--------------gui 3rd letter_shell-------------)
foreach(ITEM ${gui_3rd_letter_shell_sources})
   message(${ITEM})
endforeach()
endif()

