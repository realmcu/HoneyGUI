file(GLOB gui_misc_tlsf_sources ./tlsf/*.c)
set(gui_misc_tlsf_sources ${gui_misc_tlsf_sources} CACHE INTERNAL  "gui server sources ")

HEADER_DIRECTORIES(gui_misc_tlsf_headers ./)
set(gui_misc_tlsf_headers ${gui_misc_tlsf_headers} CACHE INTERNAL  "gui server headers ")


