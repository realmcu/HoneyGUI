file(GLOB gui_SaaA_sources *.c js_ui/*.c)
set(gui_SaaA_sources ${gui_SaaA_sources} CACHE INTERNAL  "gui server sources ")

HEADER_DIRECTORIES(gui_SaaA_headers ./)
set(gui_SaaA_headers ${gui_SaaA_headers} CACHE INTERNAL  "gui server headers ")

target_compile_definitions(gui PRIVATE ENABLE_RTK_GUI_SCRIPT_AS_A_APP)
if(CONFIG_LIB_MODE)
target_compile_definitions(HoneyGUI PRIVATE ENABLE_RTK_GUI_SCRIPT_AS_A_APP)
endif() 

