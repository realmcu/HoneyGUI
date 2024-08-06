file(GLOB gui_engine_core_sources *.c )
set(gui_engine_core_sources ${gui_engine_core_sources} CACHE INTERNAL  "gui input sources ")

HEADER_DIRECTORIES(gui_engine_core_headers ./)
set(gui_engine_core_headers ${gui_engine_core_headers} CACHE INTERNAL  "gui input headers ")
