file(GLOB gui_app_sources *.c *.h)
set(gui_app_sources ${gui_app_sources} CACHE INTERNAL  "gui app sources ")

if(gui_debug_enable)  
   message(--------------gui app sources-------------)
foreach(ITEM ${gui_app_sources})
   message(${ITEM})
endforeach()
endif()

