file(GLOB gui_server_sources *.c *.h)
set(gui_server_sources ${gui_server_sources} CACHE INTERNAL  "gui server sources ")

if(gui_debug_enable)  
   message(--------------gui server sources-------------)
foreach(ITEM ${gui_server_sources})
   message(${ITEM})
endforeach()
endif()

