set(CURRENT_PATH "${CMAKE_CURRENT_SOURCE_DIR}")

file(GLOB gui_misc_tlsf_sources *.c)
set(gui_misc_tlsf_sources ${gui_misc_tlsf_sources} CACHE INTERNAL  "gui server sources ")

HEADER_DIRECTORIES(gui_misc_tlsf_headers ./)
set(gui_misc_tlsf_headers ${gui_misc_tlsf_headers} CACHE INTERNAL  "gui server headers ")



message("HoneyGUI base path ${CURRENT_PATH}")