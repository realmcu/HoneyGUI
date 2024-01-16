file(GLOB gui_widget_sources *.c *.h)
set(gui_widget_sources ${gui_widget_sources} CACHE INTERNAL  "gui widget sources ")

if(gui_debug_enable)  
   message(--------------gui widget sources-------------)
foreach(ITEM ${gui_widget_sources})
   message(${ITEM})
endforeach()
endif()
