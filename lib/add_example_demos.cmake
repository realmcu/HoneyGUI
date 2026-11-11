#
# Copyright (c) 2026, Realtek Semiconductor Corporation
#
# SPDX-License-Identifier: MIT
#

# Adds the example/ demos a .config selected, and nothing else.
#
# Usage (after the generated config.cmake is included):
#   include(${CMAKE_CURRENT_SOURCE_DIR}/../../lib/add_example_demos.cmake)
#   gui_add_example_demos()
#
# example/ holds one demo behind each option of the "Select HoneyGUI Demo"
# choice in Kconfig.gui. Without a selection the directory yields no sources,
# so the simulators must not add_subdirectory() it unconditionally: that is
# what made the browser simulator look dependent on the demo applications
# although it compiles none of them. Adding it only when a demo is selected
# keeps a demo-free engine free of the whole tree.
#
# The option names are read from Kconfig.gui instead of being listed here, so
# a demo added to the choice is picked up without touching this file.

# Resolved here rather than inside the function: CMAKE_CURRENT_LIST_DIR there
# is the caller's directory, not this file's.
get_filename_component(GUI_KCONFIG "${CMAKE_CURRENT_LIST_DIR}/../Kconfig.gui" ABSOLUTE)
get_filename_component(GUI_EXAMPLE_DIR "${CMAKE_CURRENT_LIST_DIR}/../example" ABSOLUTE)

function(_gui_demo_choice_options OUT_VAR)
    if(NOT EXISTS "${GUI_KCONFIG}")
        message(FATAL_ERROR "Kconfig not found: ${GUI_KCONFIG}")
    endif()

    file(STRINGS "${GUI_KCONFIG}" _lines)
    set(_in_choice FALSE)
    set(_found_choice FALSE)
    set(_options "")
    foreach(_line IN LISTS _lines)
        # Leading spaces are not significant; the keywords are not indented.
        string(STRIP "${_line}" _line)
        if(_line STREQUAL "choice")
            set(_in_choice TRUE)
            set(_found_choice TRUE)
        elseif(_in_choice AND _line STREQUAL "endchoice")
            set(_in_choice FALSE)
        elseif(_in_choice AND _line MATCHES "^config[ \t]+([A-Za-z0-9_]+)")
            list(APPEND _options "${CMAKE_MATCH_1}")
        endif()
    endforeach()

    if(NOT _found_choice)
        message(FATAL_ERROR "No 'choice' block found in ${GUI_KCONFIG}")
    endif()

    set(${OUT_VAR} "${_options}" PARENT_SCOPE)
endfunction()

function(gui_add_example_demos)
    _gui_demo_choice_options(_demo_options)

    set(_selected "")
    foreach(_option IN LISTS _demo_options)
        if(CONFIG_${_option})
            list(APPEND _selected "${_option}")
        endif()
    endforeach()

    if(NOT _selected)
        message(STATUS "No demo application selected, not adding example/")
        return()
    endif()

    message(STATUS "Demo application: ${_selected}")
    add_subdirectory(${GUI_EXAMPLE_DIR} example)
endfunction()
