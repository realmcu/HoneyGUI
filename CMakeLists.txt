set(GUI_TARGET_NAME guiLib)

if(ZEPHYR_BASE)
  message(STATUS "ZEPHYR_BASE is set to: ${ZEPHYR_BASE}")
  add_library(${GUI_TARGET_NAME} STATIC)
  target_link_libraries(${GUI_TARGET_NAME} PRIVATE zephyr_interface)

  target_compile_options(${GUI_TARGET_NAME} PRIVATE -Wno-unused-variable)
  target_compile_options(${GUI_TARGET_NAME} PRIVATE -Wno-maybe-uninitialized)
  target_compile_options(${GUI_TARGET_NAME} PRIVATE -Wno-missing-braces)
  target_compile_options(${GUI_TARGET_NAME} PRIVATE -Wno-char-subscripts)
  target_compile_options(${GUI_TARGET_NAME} PRIVATE -Wno-double-promotion)
  add_subdirectory(realgui)
else()
  add_library(${GUI_TARGET_NAME})
  add_subdirectory(realgui)
endif()

message(STATUS "[Complie info] target: ${GUI_TARGET_NAME}")