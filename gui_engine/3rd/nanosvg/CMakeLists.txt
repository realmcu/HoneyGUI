

HEADER_DIRECTORIES(gui_3rd_nanosvg_headers ./)
set(gui_3rd_nanosvg_headers ${gui_3rd_nanosvg_headers} CACHE INTERNAL  "gui engine headers ")

if(gui_debug_enable)  
   message(--------------gui 3rd stb-------------)
foreach(ITEM ${${sources_name}})
   message(${ITEM})
endforeach()
endif()

