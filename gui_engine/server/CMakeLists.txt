file(GLOB gui_server_sources *.c)
set(gui_server_sources ${gui_server_sources} CACHE INTERNAL  "gui server sources ")

HEADER_DIRECTORIES(gui_server_headers ./)
set(gui_server_headers ${gui_server_headers} CACHE INTERNAL  "gui server headers ")

if(gui_debug_enable)  
   message(--------------gui server sources-------------)
foreach(ITEM ${gui_server_sources})
   message(${ITEM})
endforeach()
endif()