Code Generation
===============

This chapter describes the dual-engine code generation flow, output layout, and protected-region mechanism.

Overview
--------

The designer converts HML into C code for the target engine selected by ``targetEngine`` in ``project.json``:

* ``honeygui``: generates HoneyGUI API calls and uses SCons.
* ``lvgl``: generates LVGL API calls and uses CMake.

Flow
----

.. code-block:: text

   HML files
     -> HML parser
     -> component tree / events / timers
     -> CodeGeneratorFactory
     -> HoneyGuiCCodeGenerator or LvglCCodeGenerator
     -> C sources under src/

Architecture
------------

Core modules:

* ``CodeGeneratorFactory`` selects the generator from ``targetEngine``.
* ``HoneyGuiCCodeGenerator`` generates HoneyGUI UI, callbacks, user files, and SConscript.
* ``LvglCCodeGenerator`` generates LVGL UI, callbacks, entry files, and resource descriptors.
* ``EntryFileGenerator`` generates the HoneyGUI project entry ``{ProjectName}Entry.c``.
* ``SConscriptGenerator`` generates ``src/SConscript``.

Commands
--------

* Toolbar :guilabel:`Generate Code`.
* Command Palette: ``HoneyDesigner: Generate Code``.
* Shortcut: ``Ctrl+G``.
* The build flow can also run code generation as one of its steps.

Generation scans every HML file under the project ``ui/`` directory.

HoneyGUI Output
---------------

.. code-block:: text

   src/
   ├── {ProjectName}Entry.c      # Project entry with protected regions
   ├── SConscript                # Generated build script
   ├── ui/
   │   ├── {name}_ui.h           # Overwritten on every generation
   │   └── {name}_ui.c           # Overwritten on every generation
   ├── callbacks/
   │   ├── {name}_callbacks.h    # Regenerated declarations
   │   └── {name}_callbacks.c    # Regenerated while preserving protected regions
   └── user/
       ├── {name}_user.h         # Generated once only
       └── {name}_user.c         # Generated once only

Naming:

* ``{name}`` is the HML basename without the extension, for example ``dashboard.hml`` -> ``dashboard_ui.c``.
* All HML UI sources are written directly under ``src/ui``; they are not mirrored into subdirectories.
* Two HML files with the same basename in different folders produce the same C file name. Keep HML basenames unique.
* HML basenames should use valid C identifier characters.

LVGL Output
-----------

.. code-block:: text

   src/lvgl/
   ├── {name}_lvgl_ui.c/h
   ├── {name}_lvgl_callbacks.c/h
   ├── lvgl_generated_ui.c/h     # Multi-page entry
   ├── ui_resource.h             # external-bin resource macros
   └── lv_img_dsc_list.c/h       # external-bin image descriptors

LVGL resource deployment modes:

* ``c-array``: images and fonts are compiled into C arrays and linked into firmware.
* ``external-bin``: resources are packaged into romfs and loaded at runtime.

Generated Code Structure
------------------------

For HoneyGUI, one HML file generates a UI header, a UI implementation, and callback files. LVGL uses the same structure with LVGL APIs and ``{name}_lvgl_*`` file names.

Header File (.h)
~~~~~~~~~~~~~~~~~~~~~~~

.. code-block:: c

   #ifndef DASHBOARD_UI_H
   #define DASHBOARD_UI_H

   #include "gui_obj.h"
   #include "gui_view_instance.h"

   // Component handle declarations
   extern gui_obj_t *lbl_title;
   extern gui_obj_t *btn_next;

   #endif

Implementation File (.c)
~~~~~~~~~~~~~~~~~~~~~~~~

.. code-block:: c

   #include "dashboard_ui.h"
   #include "../callbacks/dashboard_callbacks.h"

   gui_obj_t *lbl_title = NULL;
   gui_obj_t *btn_next = NULL;

   static void view_home_switch_in(gui_view_t *view)
   {
       // Create child components and register switchView events
   }

   GUI_VIEW_INSTANCE("view_home", false, view_home_switch_in, view_home_switch_out, false);

Callback Files (_callbacks.h / _callbacks.c)
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

.. code-block:: c

   void on_btn_next_click(void *obj, gui_event_t *e)
   {
       GUI_UNUSED(obj);
       GUI_UNUSED(e);
       // Generated callback body; overwritten on regeneration
   }

   /* @protected start custom_functions */
   void on_next_click(void *obj, gui_event_t *e)
   {
       // User business logic
   }
   /* @protected end custom_functions */

``{name}_user.c/h`` is generated once. The entry file and ``SConscript`` are regenerated with the project. See the next section for protected regions and update rules.

Protected Regions
-----------------

The generator recognizes both marker styles:

.. code-block:: c

   /* @protected start custom_functions */
   /* user code */
   /* @protected end custom_functions */

.. code-block:: c

   /* USER CODE BEGIN custom_functions */
   /* user code */
   /* USER CODE END custom_functions */

Directory behavior:

* ``src/ui``: overwritten on every generation; do not edit manually.
* ``src/callbacks``: regenerated, but protected regions are preserved.
* ``src/user``: generated once and then owned by the user.
* ``src/{ProjectName}Entry.c``: regenerated with entry protected regions.

.. warning::
   Event callback bodies are not protected and are regenerated when HML changes. Put business logic in ``custom_functions`` and invoke it with ``callFunction``.

View Switching
--------------

``switchView`` on ``hg_view`` does not generate a standalone callback. It is registered in the ``GUI_VIEW_INSTANCE`` ``switch_in`` callback:

.. code-block:: c

   static void view_main_switch_in(gui_view_t *view)
   {
       gui_view_switch_on_event(view, "view_settings",
                                SWITCH_OUT_TO_LEFT_USE_TRANSLATION,
                                SWITCH_IN_FROM_RIGHT_USE_TRANSLATION,
                                GUI_EVENT_TOUCH_MOVE_LEFT);
   }

   GUI_VIEW_INSTANCE("view_main", false, view_main_switch_in, view_main_switch_out, false);

``GUI_VIEW_INSTANCE`` currently takes five arguments; the last one controls snapshots.

Multi-HML Projects
------------------

* Each HML produces its own ``{name}_ui.c``, ``{name}_callbacks.c``, and ``{name}_user.c``.
* All HML files share one project entry file and one ``SConscript``.
* ``switchView`` can jump across HML files; targets resolve by view id or name.
* Keep component IDs globally unique because generated IDs become C global variable names.

Notes
-----

1. Keep HML files under the project ``ui/`` directory.
2. Use valid C identifier characters in HML basenames.
3. Do not edit protected-region markers.
4. Regenerate after HML changes; protected user code is preserved.
5. Switching ``targetEngine`` does not automatically remove the old output directory. Clean before regenerating.

Next Steps
----------

* Learn :doc:`resource_management` to manage project resources
* See :doc:`build_simulation` to learn how to build and run
* Refer to :doc:`deployment` for embedded deployment solutions
