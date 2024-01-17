file(GLOB gui_app_sources *.c )
set(gui_app_sources ${gui_app_sources} CACHE INTERNAL  "gui app sources ")

HEADER_DIRECTORIES(gui_app_headers ./)
set(gui_app_headers ${gui_app_headers} CACHE INTERNAL  "gui app headers ")

if(gui_debug_enable)  
   message(--------------gui app sources-------------)
foreach(ITEM ${gui_app_sources})
   message(${ITEM})
endforeach()
endif()
