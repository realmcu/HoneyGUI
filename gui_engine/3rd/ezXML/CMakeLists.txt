


file(GLOB gui_3rd_ezxml_sources *.c)

set(gui_3rd_ezxml_sources ${gui_3rd_ezxml_sources} CACHE INTERNAL  "sources")

HEADER_DIRECTORIES(gui_3rd_ezxml_headers ./)

set(gui_3rd_ezxml_headers ${gui_3rd_ezxml_headers} CACHE INTERNAL  "headers")



if(gui_debug_enable)  
   message(--------------gui 3rd stb-------------)
foreach(ITEM ${gui_3rd_ezxml_sources})
   message(${ITEM})
endforeach()
endif()

