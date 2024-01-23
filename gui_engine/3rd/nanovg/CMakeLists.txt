

file(GLOB gui_3rd_nanovg_sources base/*.c agge/*.cpp)

set(gui_3rd_nanovg_sources ${gui_3rd_nanovg_sources} CACHE INTERNAL  "sources")

HEADER_DIRECTORIES(gui_3rd_nanovg_headers ./)

set(gui_3rd_nanovg_headers ${gui_3rd_nanovg_headers} CACHE INTERNAL  "headers")


if(gui_debug_enable)  
   message(--------------gui 3rd stb-------------)
foreach(ITEM ${gui_3rd_nanovg_sources})
   message(${ITEM})
endforeach()
endif()

