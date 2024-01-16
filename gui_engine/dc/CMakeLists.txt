file(GLOB gui_dc_sources *.c *.h)
set(gui_dc_sources ${gui_dc_sources} CACHE INTERNAL  "gui dc sources ")

if(gui_debug_enable)  
   message(--------------gui dc sources-------------)
foreach(ITEM ${gui_dc_sources})
   message(${ITEM})
endforeach()
endif()