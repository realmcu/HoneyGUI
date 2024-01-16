file(GLOB gui_engine_sources *.c *.h)
set(gui_engine_sources ${gui_engine_sources} CACHE INTERNAL  "gui engine sources ")

if(gui_debug_enable)  
   message(--------------gui engine sources-------------)
foreach(ITEM ${gui_engine_sources})
   message(${ITEM})
endforeach()
endif()

