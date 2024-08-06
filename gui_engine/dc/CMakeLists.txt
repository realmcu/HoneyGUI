file(GLOB gui_dc_sources *.c )
set(gui_dc_sources ${gui_dc_sources} CACHE INTERNAL  "gui dc sources ")

HEADER_DIRECTORIES(gui_dc_headers ./)
set(gui_dc_headers ${gui_dc_headers} CACHE INTERNAL  "gui dc headers ")

if(gui_debug_enable)  
	foreach(ITEM ${gui_dc_sources})
		message([gui][dc srcs]${ITEM})
	endforeach()
	
	foreach(ITEM ${gui_dc_headers})
		message([gui][dc headers]${ITEM})
	endforeach()
endif()