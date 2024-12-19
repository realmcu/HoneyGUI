
set(gui_3rd_stb_sources  CACHE INTERNAL  "sources ")
set(gui_3rd_stb_headers ${CMAKE_CURRENT_SOURCE_DIR} CACHE INTERNAL  "headers ")

target_compile_definitions(gui PRIVATE STBIDEF=extern)