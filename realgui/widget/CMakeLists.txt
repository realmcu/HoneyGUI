file(GLOB gui_widget_sources *.c)
file(GLOB_RECURSE gui_widget_remove_sources gui_px.c gui_u8g2.c)
list(REMOVE_ITEM gui_widget_sources ${gui_widget_remove_sources})
set(gui_widget_sources ${gui_widget_sources} CACHE INTERNAL  "gui widget sources ")

HEADER_DIRECTORIES(gui_widget_headers ./)
set(gui_widget_headers ${gui_widget_headers} CACHE INTERNAL  "gui widget headers ")

if(gui_debug_enable)  
	foreach(ITEM ${gui_widget_sources})
		message([gui][widget srcs]${ITEM})
	endforeach()
	
	foreach(ITEM ${gui_widget_headers})
		message([gui][widget headers]${ITEM})
	endforeach()
endif()
