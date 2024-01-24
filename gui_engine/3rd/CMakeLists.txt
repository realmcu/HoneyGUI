add_subdirectory(nanovg)
add_subdirectory(ezXML)
add_subdirectory(js)
add_subdirectory(nanosvg)
add_subdirectory(painter-engine)
add_subdirectory(stb)


list(APPEND gui_3rd_sources 
			${gui_3rd_common_sources} 
			${gui_3rd_ezxml_sources}
			${gui_3rd_js_sources} 
			${gui_3rd_nanosvg_sources}
			${gui_3rd_nanovg_sources} 
			${gui_3rd_px_sources} 
			${gui_3rd_stb_sources})
set(gui_3rd_sources ${gui_3rd_sources} CACHE INTERNAL  "gui engine 3rd ")

list(APPEND gui_3rd_headers
			${gui_3rd_common_headers} 
			${gui_3rd_ezxml_sources}
			${gui_3rd_js_headers} 
			${gui_3rd_nanosvg_headers}
			${gui_3rd_nnaovg_headers} 
			${gui_3rd_px_headers} 
			${gui_3rd_stb_headers})
set(gui_3rd_headers ${gui_3rd_headers} CACHE INTERNAL  "gui 3rd headers ")


HEADER_DIRECTORIES(gui_3rd_headers ./)
set(gui_3rd_headers ${gui_3rd_headers} CACHE INTERNAL  "gui 3rd headers ")

foreach(ITEM ${gui_3rd_sources})
   message(${ITEM})
endforeach()