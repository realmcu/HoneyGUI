#
# Copyright (c) 2026, Realtek Semiconductor Corporation
#
# SPDX-License-Identifier: MIT
#

if(ZEPHYR_BASE)
  if(CONFIG_REALTEK_HONEYGUI) #set to 1 to enable or variable to for dependency
	  message(STATUS "ZEPHYR_BASE is set to: ${ZEPHYR_BASE}")
	  set(GUI_TARGET_NAME guiLib)
	  zephyr_library_named(${GUI_TARGET_NAME})
	  target_compile_options(${GUI_TARGET_NAME} PRIVATE -Wno-unused-variable)
	  target_compile_options(${GUI_TARGET_NAME} PRIVATE -Wno-maybe-uninitialized)
	  target_compile_options(${GUI_TARGET_NAME} PRIVATE -Wno-missing-braces)
	  target_compile_options(${GUI_TARGET_NAME} PRIVATE -Wno-char-subscripts)
	  target_compile_options(${GUI_TARGET_NAME} PRIVATE -Wno-double-promotion)
	  add_subdirectory(realgui)
	  add_subdirectory(example)

	  # Compile the ENTIRE GUI library with -Ofast.
	  # NOTE: target_compile_options(${GUI_TARGET_NAME} PRIVATE -Ofast) does NOT work
	  # here -- Zephyr appends its interface-level -Os AFTER per-target options, so
	  # -Os would win (GCC takes the last -O). Instead we attach -Ofast as a
	  # source-file COMPILE_OPTIONS to every source of guiLib; CMake emits per-source
	  # options last on the compile line, so -Ofast overrides the inherited -Os.
	  # Done here (guiLib's own directory) after all add_subdirectory() calls, so the
	  # SOURCES list is complete and the property lands in the right directory scope.
	  get_target_property(_gui_all_srcs ${GUI_TARGET_NAME} SOURCES)
	  if(_gui_all_srcs)
	      set_source_files_properties(${_gui_all_srcs} PROPERTIES COMPILE_OPTIONS "-Ofast")
	  endif()
  endif()
elseif(CONFIG_AMEBAGREEN2)
  ameba_internal_library(honeygui)
  message(STATUS "The value of Honeygui CURRENT_LIB_NAME is: ${CURRENT_LIB_NAME}")
  set(GUI_TARGET_NAME ${CURRENT_LIB_NAME})
  add_subdirectory(realgui)
  add_subdirectory(example)
else ()
  set(GUI_TARGET_NAME guiLib)
  add_library(${GUI_TARGET_NAME})
  add_subdirectory(realgui)
  add_subdirectory(example)
endif()

message(STATUS "[Complie info] target: ${GUI_TARGET_NAME}")