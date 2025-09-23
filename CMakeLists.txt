set(GUI_TARGET_NAME guiLib)

if(ZEPHYR_BASE)
  if(CONFIG_REALTEK_HONEYGUI) #set to 1 to enable or variable to for depnedency
	  message(STATUS "ZEPHYR_BASE is set to: ${ZEPHYR_BASE}")
	  zephyr_library_named(${GUI_TARGET_NAME})
	  target_compile_options(${GUI_TARGET_NAME} PRIVATE -Wno-unused-variable)
	  target_compile_options(${GUI_TARGET_NAME} PRIVATE -Wno-maybe-uninitialized)
	  target_compile_options(${GUI_TARGET_NAME} PRIVATE -Wno-missing-braces)
	  target_compile_options(${GUI_TARGET_NAME} PRIVATE -Wno-char-subscripts)
	  target_compile_options(${GUI_TARGET_NAME} PRIVATE -Wno-double-promotion)
	  add_subdirectory(realgui)
	  add_subdirectory(example)
	  
  endif()
else()
  add_library(${GUI_TARGET_NAME})
  add_subdirectory(realgui)
  add_subdirectory(example)
endif()

message(STATUS "[Complie info] target: ${GUI_TARGET_NAME}")