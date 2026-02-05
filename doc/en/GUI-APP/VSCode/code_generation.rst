Code Generation
===============

This document describes the code generation functionality of HoneyGUI Design, including architecture design, usage methods, and implementation details.

Overview
--------

Generate C code that calls HoneyGUI APIs from the HML designer, enabling conversion from visual design to embedded GUI code.

Code Generation Flow
--------------------

.. code-block:: text

   HML Files → Parser → Component Tree → C Code Generator → HoneyGUI API Call Code

Architecture Design
-------------------

Core Components
~~~~~~~~~~~~~~~

.. code-block:: text

   CodeGenerator (Service Layer)
     ├─ Scan all HML files in the project
     ├─ Generate project entry file (EntryFileGenerator)
     ├─ Generate build script (SConscriptGenerator)
     └─ Generate code for each HML (HoneyGuiCCodeGenerator)

   EntryFileGenerator (Independent Tool)
     └─ Generate {ProjectName}Entry.c

   SConscriptGenerator (Independent Tool)
     └─ Generate src/SConscript

   HoneyGuiCCodeGenerator (Core Generator)
     ├─ Generate {hmlFileName}_ui.h
     ├─ Generate {hmlFileName}_ui.c
     ├─ Generate {hmlFileName}_callbacks.h
     ├─ Generate {hmlFileName}_callbacks.c (with protected regions)
     ├─ Generate {hmlFileName}_user.h
     └─ Generate {hmlFileName}_user.c

File Naming and Directory Rules
--------------------------------

Naming Rules
~~~~~~~~~~~~

* Use HML file name as the base name for generated files
* Example: ``main.hml`` → ``main.h``, ``main.c``, ``main_callbacks.h``, ``main_callbacks.c``

Directory Mapping
~~~~~~~~~~~~~~~~~

* HML files in ``ui/xxx/`` directory → C code generated to corresponding subdirectories in ``src/`` directory
* Each HML file generates to an independent subdirectory
* Example: ``ui/main/main.hml`` → ``src/ui/main_ui.c``, ``src/callbacks/main_callbacks.c``, ``src/user/main_user.c``
* Automatically create subdirectories if they don't exist

Generated File Structure
~~~~~~~~~~~~~~~~~~~~~~~~

.. code-block:: text

   project/
   ├── ui/
   │   ├── main/
   │   │   └── main.hml
   │   └── settings/
   │       └── settings.hml
   └── src/
       ├── {ProjectName}Entry.c    # Project entry file
       ├── SConscript              # Build script
       ├── ui/                     # UI code (overwritten each time)
       │   ├── main_ui.h
       │   ├── main_ui.c
       │   ├── settings_ui.h
       │   └── settings_ui.c
       ├── callbacks/              # Callback code (protected regions)
       │   ├── main_callbacks.h
       │   ├── main_callbacks.c
       │   ├── settings_callbacks.h
       │   └── settings_callbacks.c
       └── user/                   # User code (generated once)
           ├── main_user.h
           ├── main_user.c
           ├── settings_user.h
           └── settings_user.c

Usage
-----

Generate Code
~~~~~~~~~~~~~

1. Open any HML file in the designer
2. Click the "Generate Code" button in the toolbar
3. Automatically scan all HML files in the project and generate code
4. Display generation progress and results

**Generated Content**:

* C code files for all designs
* Project entry file ``{ProjectName}Entry.c``
* Build script ``SConscript``

**Shortcuts**:

* Command Palette: ``Ctrl+Shift+P`` → ``HoneyGUI: Generate Code``
* Toolbar Button: Click "Generate Code"

Code Protected Regions
-----------------------

Protected Region Syntax
~~~~~~~~~~~~~~~~~~~~~~~

.. code-block:: c

   // HONEYGUI PROTECTED START [unique_id]
   // Code here will be preserved during regeneration
   // HONEYGUI PROTECTED END [unique_id]

Examples
~~~~~~~~

**First Generation**:

.. code-block:: c

   // HONEYGUI PROTECTED START [on_start_click]
   void on_start_click(gui_obj_t *obj) {
       // TODO: Implement event handling logic
   }
   // HONEYGUI PROTECTED END [on_start_click]

**After User Modification**:

.. code-block:: c

   // HONEYGUI PROTECTED START [on_start_click]
   void on_start_click(gui_obj_t *obj) {
       printf("Starting application...\n");
       init_app_state();
       load_user_settings();
   }
   // HONEYGUI PROTECTED END [on_start_click]

**After Regeneration**: Code inside protected regions is preserved, other code is updated.

Generated Code Structure
------------------------

Header File (.h)
~~~~~~~~~~~~~~~~

.. code-block:: c

   #ifndef MAIN_H
   #define MAIN_H

   #include "gui_api.h"

   // Component declarations
   extern gui_obj_t *mainView;
   extern gui_obj_t *titleLabel;
   extern gui_obj_t *startButton;

   // Initialization function
   void main_init(void);

   // Update function
   void main_update(void);

   #endif

Implementation File (.c)
~~~~~~~~~~~~~~~~~~~~~~~~

.. code-block:: c

   #include "main.h"
   #include "main_callbacks.h"

   // Component definitions
   gui_obj_t *mainView = NULL;
   gui_obj_t *titleLabel = NULL;
   gui_obj_t *startButton = NULL;

   // View switch callback (switch_in)
   static void main_view_switch_in(gui_view_t *view)
   {
       // Register events
       gui_view_switch_on_event(view, "screen2", SWITCH_OUT_TO_LEFT_USE_TRANSLATION,
                                SWITCH_IN_FROM_RIGHT_USE_TRANSLATION, GUI_EVENT_TOUCH_MOVE_LEFT);

       // Create child components
       titleLabel = gui_text_create(view, "titleLabel", 10, 10, 460, 40);
       // ...
   }

   GUI_VIEW_INSTANCE("mainView", false, main_view_switch_in, main_view_switch_out);

Callback Files (_callbacks.h / _callbacks.c)
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

.. code-block:: c

   // main_callbacks.c
   #include "main_callbacks.h"

   /* @protected start on_start_click */
   void on_start_click(gui_obj_t *obj) {
       // User implementation logic
   }
   /* @protected end on_start_click */

Important Notes
---------------

1. **Directory Structure**: HML files must be in subdirectories under the ``ui/`` directory
2. **File Naming**: HML file names should use valid C identifiers
3. **Protected Regions**: Do not manually modify protected region markers
4. **Regeneration**: User code is preserved when regenerating after HML modifications
5. **Multiple Files**: Each HML generates independently without affecting others

View Switch Implementation
--------------------------

For view switching (switchView) of ``hg_view`` components, the code generator no longer generates independent callback functions. Instead, it directly registers using the HoneyGUI SDK's ``GUI_VIEW_INSTANCE`` macro and ``gui_view_switch_on_event`` function in the ``switch_in`` callback. This greatly simplifies the code structure and reduces unnecessary protected regions.

Example
~~~~~~~

View switch event configured in HML file:

.. code-block:: xml

   <hg_view id="main_view" x="0" y="0" w="480" h="272">
       <events>
           <event type="onSwipeLeft">
               <action type="switchView" target="screen2"
                       switchOutStyle="SWITCH_OUT_TO_LEFT_USE_TRANSLATION"
                       switchInStyle="SWITCH_IN_FROM_RIGHT_USE_TRANSLATION" />
           </event>
       </events>
   </hg_view>

Generated C code:

.. code-block:: c

   static void main_view_switch_in(gui_view_t *view)
   {
       // Register left swipe event
       gui_view_switch_on_event(view, "screen2",
                                SWITCH_OUT_TO_LEFT_USE_TRANSLATION,
                                SWITCH_IN_FROM_RIGHT_USE_TRANSLATION,
                                GUI_EVENT_TOUCH_MOVE_LEFT);

       // Create child components...
   }

   GUI_VIEW_INSTANCE("main_view", false, main_view_switch_in, main_view_switch_out);

Building Generated Code
-----------------------

After generating code, you can compile the project using the SCons build system:

.. code-block:: bash

   cd win32_sim
   scons

For detailed build and simulation instructions, please refer to :doc:`build_simulation`.

Next Steps
----------

* Understand :doc:`event_system` to learn how to handle component events
* Learn :doc:`resource_management` to manage project resources
* See :doc:`build_simulation` to learn how to build and run
* Refer to :doc:`deployment` for embedded deployment solutions
