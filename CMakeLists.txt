include(cmake/gui_build.cmake)
set(gui_debug_enable OFF)

add_subdirectory(gui_engine)
#add_subdirectory(lvgl)

#gui_sources/gui_headers was set as cache variable
#if gui sources is desired, for example
#   target_source(example_target PRIVATE ${gui_sources})
list(APPEND gui_sources 
			${gui_app_sources} 
			${gui_dc_sources} 
			${gui_engine_sources}
			${gui_input_sources} 
			${gui_server_sources} 
			${gui_widget_sources})
set(gui_sources ${gui_sources} CACHE INTERNAL  "gui sources ")

list(APPEND gui_headers
			${gui_app_headers} 
			${gui_dc_headers} 
			${gui_engine_headers}
			${gui_input_headers} 
			${gui_server_headers} 
			${gui_widget_headers}
			${gui_3rd_nanovg_headers}
			${gui_misc_headers}
                        ${gui_3rd_common_headers}
                        ${gui_3rd_headers})
set(gui_headers ${gui_headers} CACHE INTERNAL  "gui headers ")

if(gui_debug_enable)
foreach(ITEM ${gui_sources})
   message(${ITEM})
endforeach()
endif()
