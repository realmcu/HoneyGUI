file(GLOB gui_engine_sources *.c)
file(GLOB_RECURSE gui_engine_remove_sources  acc_sw_ppe_sim.c font_freetype.c acc_rtk_ppe.c acc_vg_lite.c acc_arm_2d.c)
list(REMOVE_ITEM gui_engine_sources ${gui_engine_remove_sources})
set(gui_engine_sources ${gui_engine_sources} CACHE INTERNAL  "gui engine sources ")

HEADER_DIRECTORIES(gui_engine_headers ./)
set(gui_engine_headers ${gui_engine_headers} CACHE INTERNAL  "gui engine headers ")

if(CONFIG_RTL87X3EP)
    add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/hw_acc/rtl8773e)
endif()

if(gui_debug_enable)  
    foreach(ITEM ${gui_engine_sources} )
		message(STATUS [GUI][engine_srcs]${ITEM})
	endforeach()
	
	foreach(ITEM ${gui_engine_headers} )
		message(STATUS [GUI][engine_srcs]${ITEM})
	endforeach()
endif()

