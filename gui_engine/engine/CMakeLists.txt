file(GLOB gui_engine_sources *.c)
file(GLOB_RECURSE  gui_engine_remove_sources  acc_rtk_ppe.c acc_rtk_ppev2.c acc_vg_lite.c)


#if(1)
list(REMOVE_ITEM gui_engine_sources  ${gui_engine_remove_sources})
#endif()

set(gui_engine_sources ${gui_engine_sources} CACHE INTERNAL  "gui engine sources ")


HEADER_DIRECTORIES(gui_engine_headers ./)
set(gui_engine_headers ${gui_engine_headers} CACHE INTERNAL  "gui engine headers ")

if(gui_debug_enable)  
   message(--------------gui engine sources-------------)
foreach(ITEM ${gui_engine_sources})
   message(${ITEM})
endforeach()
endif()

