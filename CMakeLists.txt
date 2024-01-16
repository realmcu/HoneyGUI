set(gui_debug_enable OFF)

add_subdirectory(gui_engine)
add_subdirectory(lvgl)

set(gui_sources ${gui_sources} CACHE INTERNAL  "gui sources ")
#gui_sources was set as cache variable
#if gui sources is desired, for example
#   target_source(example_target PRIVATE ${gui_sources})
list(APPEND gui_sources 
			${gui_app_sources} 
			${gui_dc_sources} 
			${gui_engine_sources}
			${gui_input_sources} 
			${gui_server_sources} 
			${gui_widget_sources})

if(gui_debug_enable)
foreach(ITEM ${gui_sources})
   message(${ITEM})
endforeach()
endif()


