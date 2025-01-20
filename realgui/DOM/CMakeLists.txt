file(GLOB gui_dom_sources *.c )
set(gui_dom_sources ${gui_dom_sources} CACHE INTERNAL  "gui dom sources ")

HEADER_DIRECTORIES(gui_dom_headers ./)
set(gui_dom_headers ${gui_dom_headers} CACHE INTERNAL  "gui dom headers ")

if(gui_debug_enable)  
   message(--------------gui dom sources-------------)
foreach(ITEM ${gui_dom_sources})
   message(${ITEM})
endforeach()
endif()
