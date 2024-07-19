file(GLOB gui_input_sources *.c )
set(gui_input_sources ${gui_input_sources} CACHE INTERNAL  "gui input sources ")

HEADER_DIRECTORIES(gui_input_headers ./)
set(gui_input_headers ${gui_input_headers} CACHE INTERNAL  "gui input headers ")

if(gui_debug_enable)  
	foreach(ITEM ${gui_input_sources})
		message([gui][input srcs]${ITEM})
	endforeach()
	
	foreach(ITEM ${gui_input_headers})
		message([gui][input headers]${ITEM})
	endforeach()
endif()


