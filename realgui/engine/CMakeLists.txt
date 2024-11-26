file(GLOB gui_engine_sources *.c)
file(GLOB_RECURSE gui_engine_remove_sources  acc_sw_ppe_sim.c font_freetype.c acc_rtk_ppe.c acc_rtk_ppev2.c acc_vg_lite.c acc_arm_2d.c image_compress.c image_rgb_fs.c image_rgb_mem.c)
list(REMOVE_ITEM gui_engine_sources ${gui_engine_remove_sources})
set(gui_engine_sources ${gui_engine_sources} CACHE INTERNAL  "gui engine sources ")

HEADER_DIRECTORIES(gui_engine_headers ./)
set(gui_engine_headers ${gui_engine_headers} CACHE INTERNAL  "gui engine headers ")

if(gui_debug_enable)  
    foreach(ITEM ${gui_engine_sources} )
		message(STATUS [GUI][engine_srcs]${ITEM})
	endforeach()
	
	foreach(ITEM ${gui_engine_headers} )
		message(STATUS [GUI][engine_srcs]${ITEM})
	endforeach()
endif()

