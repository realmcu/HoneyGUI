file(GLOB gui_input_sources *.c *.h)
set(gui_input_sources ${gui_input_sources} CACHE INTERNAL  "gui input sources ")

if(gui_debug_enable)  
   message(--------------gui input sources-------------)
foreach(ITEM ${gui_input_sources})
   message(${ITEM})
endforeach()
endif()

