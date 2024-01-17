
set(sources_name gui_3rd_common_sources)
set(headers_name gui_3rd_common_headers)



file(GLOB ${sources_name} *.c)

set(${sources_name} ${${sources_name}} CACHE INTERNAL  "sources")

HEADER_DIRECTORIES(${headers_name} ./)

set(${headers_name} ${${headers_name}} CACHE INTERNAL  "headers")



if(gui_debug_enable)  
   message(--------------gui 3rd stb-------------)
foreach(ITEM ${${sources_name}})
   message(${ITEM})
endforeach()
endif()

