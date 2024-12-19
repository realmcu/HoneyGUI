


set(realgui_port_src
${CMAKE_CURRENT_SOURCE_DIR}/gui_port_acc.c
${CMAKE_CURRENT_SOURCE_DIR}/gui_port_dc.c
${CMAKE_CURRENT_SOURCE_DIR}/gui_port_filesystem.c
${CMAKE_CURRENT_SOURCE_DIR}/gui_port_ftl.c
${CMAKE_CURRENT_SOURCE_DIR}/gui_port_os.c
${CMAKE_CURRENT_SOURCE_DIR}/gui_queue.c
 CACHE INTERNAL  "gui sources ")

set(realgui_port_inc
${CMAKE_CURRENT_SOURCE_DIR} 
${CMAKE_CURRENT_SOURCE_DIR}/../../RTE/SDL2-2.26.0/x86_64-w64-mingw32/include/SDL2/
${CMAKE_CURRENT_SOURCE_DIR}/../../RTE/SDL2-2.26.0/x86_64-w64-mingw32/include/
${CMAKE_CURRENT_SOURCE_DIR}/../../RTE/SDL2-2.26.0/x86_64-w64-mingw32
${CMAKE_CURRENT_SOURCE_DIR}/../../RTE/SDL2-2.26.0/
${CMAKE_CURRENT_SOURCE_DIR}/../../RTE/
CACHE INTERNAL "gui headers ")

