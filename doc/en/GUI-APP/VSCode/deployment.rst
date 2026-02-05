Embedded Deployment
===================

This document describes deployment solutions for HoneyGUI Design projects to embedded platforms, including three different deployment methods and their applicable scenarios.

Overview
--------

HoneyGUI Design supports multiple deployment methods, allowing selection of the most appropriate solution based on project requirements:

.. list-table::
   :header-rows: 1
   :widths: 15 20 15 20 30

   * - Solution
     - Export Format
     - Firmware Changes
     - Resource Usage
     - Use Cases
   * - Solution 1
     - C Code
     - Every time
     - Minimal
     - Resource-constrained, fixed UI
   * - Solution 2
     - HML (XML)
     - Not needed
     - Medium (~10KB parser)
     - OTA UI updates needed
   * - Solution 3
     - HML (BIN)
     - Not needed
     - Small (~2KB parser)
     - OTA + resource-constrained

Solution 1: Generate C Code
----------------------------

Description
~~~~~~~~~~~

The designer converts HML designs to C code, compiled directly into firmware. This is the most common approach.

Export Products
~~~~~~~~~~~~~~~

.. code-block:: text

   export/c/
   ├── ui/
   │   ├── main_ui.h           # UI declarations
   │   └── main_ui.c           # UI implementation
   ├── callbacks/
   │   ├── main_callbacks.h    # Callback declarations
   │   └── main_callbacks.c    # Callback implementation (protected)
   └── assets/
       └── *.bin               # Binary resources

Generated Code Example
~~~~~~~~~~~~~~~~~~~~~~

.. code-block:: c

   // main_ui.c
   #include "main_ui.h"

   gui_obj_t *button1 = NULL;
   gui_obj_t *label1 = NULL;

   void main_ui_create(gui_obj_t *parent) {
       gui_obj_t *view = gui_view_create(parent, "main_view", 0, 0, 480, 272);

       button1 = gui_button_create(view, "button1", 10, 10, 100, 40);
       gui_button_set_text(button1, "Click");
       gui_obj_set_event_cb(button1, GUI_EVENT_CLICK, on_button1_click);

       label1 = gui_text_create(view, "label1", 10, 60, 200, 30);
       gui_text_set_text(label1, "Hello");
   }

Advantages
~~~~~~~~~~

* Minimal ROM/RAM usage
* Fastest execution speed
* Easy debugging (can step through)
* Flexible event handling (pure C code)

Disadvantages
~~~~~~~~~~~~~

* UI changes require firmware recompilation
* Cannot OTA update UI

Use Cases
~~~~~~~~~

* Resource-constrained MCUs (Flash < 256KB)
* UI design finalized, no changes needed
* High startup speed requirements

Deployment Steps
~~~~~~~~~~~~~~~~

1. **Generate Code**:

   Click "Generate Code" button in VSCode to generate C code files.

2. **Integrate into Firmware Project**:

   Copy files from ``src/`` directory to firmware project:

   .. code-block:: bash

      # Assume firmware project is at /path/to/firmware
      cp -r src/ui /path/to/firmware/app/gui/
      cp -r src/callbacks /path/to/firmware/app/gui/
      cp -r src/user /path/to/firmware/app/gui/
      cp src/*Entry.c /path/to/firmware/app/

3. **Add Resource Files**:

   .. code-block:: bash

      cp -r assets/*.bin /path/to/firmware/resources/

4. **Modify Build Scripts**:

   Add source files in firmware's Makefile or CMakeLists.txt:

   .. code-block:: cmake

      # CMakeLists.txt example
      file(GLOB GUI_SOURCES
           "app/gui/ui/*.c"
           "app/gui/callbacks/*.c"
           "app/gui/user/*.c"
           "app/*Entry.c")

      target_sources(firmware PRIVATE ${GUI_SOURCES})

5. **Compile Firmware**:

   .. code-block:: bash

      cd /path/to/firmware
      mkdir build && cd build
      cmake ..
      make

6. **Flash Firmware**:

   Use platform-specific flashing tools to flash firmware to device.

Solution 2: Generate HML (XML)
-------------------------------

Description
~~~~~~~~~~~

The designer exports HML files (XML format), which embedded devices parse at runtime to create UI.

Export Products
~~~~~~~~~~~~~~~

.. code-block:: text

   export/hml/
   ├── ui/
   │   └── main.hml            # HML file
   ├── assets/
   │   ├── images/             # Image resources
   │   └── fonts/              # Font resources
   └── manifest.json           # Resource manifest

HML File Example
~~~~~~~~~~~~~~~~

.. code-block:: xml

   <?xml version="1.0" encoding="UTF-8"?>
   <hg_screen id="main_screen" w="480" h="272">
     <hg_view id="main_view" x="0" y="0" w="480" h="272">
       <hg_button id="button1" x="10" y="10" w="100" h="40"
                  text="Click"
                  onClick="on_button1_click" />
       <hg_text id="label1" x="10" y="60" w="200" h="30"
                text="Hello"
                fontSize="16"
                color="#FFFFFF" />
     </hg_view>
   </hg_screen>

Embedded Implementation
~~~~~~~~~~~~~~~~~~~~~~~

Requires XML parser integration (recommend ezxml, ~10KB):

.. code-block:: c

   #include "ezxml.h"
   #include "gui_hml_loader.h"

   // Load HML file
   gui_obj_t *ui = gui_hml_load("/flash/ui/main.hml");

   // Callback function registry (pre-compiled in firmware)
   const callback_entry_t g_callbacks[] = {
       {"on_button1_click", on_button1_click},
       {"on_button1_long_press", on_button1_long_press},
       {NULL, NULL}
   };

Event Binding Mechanism
~~~~~~~~~~~~~~~~~~~~~~~~

Declare event binding in HML:

.. code-block:: xml

   <hg_button id="btn1" onClick="on_btn1_click" />

Implement callback in firmware:

.. code-block:: c

   void on_btn1_click(gui_obj_t *obj) {
       // Complex business logic
       ble_send_data(...);
       update_status(...);
   }

Advantages
~~~~~~~~~~

* UI layout can be OTA updated
* Good readability, easy to debug
* Supports data binding (``text="{data.title}"``)

Disadvantages
~~~~~~~~~~~~~

* Requires XML parser (~10KB ROM)
* Slower parsing speed
* Higher runtime memory usage

Use Cases
~~~~~~~~~

* OTA UI updates needed
* Sufficient Flash (> 256KB)
* Development debugging phase

Recommended XML Parsers
~~~~~~~~~~~~~~~~~~~~~~~

.. list-table::
   :header-rows: 1
   :widths: 20 20 60

   * - Parser
     - ROM
     - Features
   * - ezxml
     - ~10KB
     - DOM mode, simple API
   * - yxml
     - ~3KB
     - SAX streaming, zero dynamic memory
   * - mxml
     - ~30KB
     - Full-featured

Solution 3: Generate HML (BIN)
-------------------------------

Description
~~~~~~~~~~~

The designer compiles HML to binary format, which embedded devices load directly without XML parsing.

Export Products
~~~~~~~~~~~~~~~

.. code-block:: text

   export/bin/
   ├── ui/
   │   └── main.bin            # Binary UI description
   ├── assets/
   │   └── *.bin               # Binary resources
   └── manifest.bin            # Binary resource manifest

Binary Format Design
~~~~~~~~~~~~~~~~~~~~

.. code-block:: c

   // File header
   struct hml_bin_header {
       uint32_t magic;            // 0x484D4C42 "HMLB"
       uint16_t version;          // Format version
       uint16_t component_count;  // Component count
       uint32_t string_table_offset;  // String table offset
       uint32_t data_offset;      // Data area offset
   };

   // Component descriptor
   struct hml_bin_component {
       uint8_t  type;             // Component type (HG_BUTTON=1, HG_TEXT=2...)
       uint8_t  parent_index;     // Parent component index
       uint16_t id_offset;        // ID string offset
       int16_t  x, y, w, h;       // Position and size
       uint16_t props_count;      // Property count
       uint32_t props_offset;     // Property data offset
       uint16_t events_count;     // Event count
       uint32_t events_offset;    // Event data offset
   };

   // Property descriptor
   struct hml_bin_prop {
       uint16_t key_offset;       // Property name offset
       uint16_t value_offset;     // Property value offset
       uint8_t  value_type;       // Value type (STRING=0, INT=1, FLOAT=2...)
   };

   // Event descriptor
   struct hml_bin_event {
       uint8_t  event_type;       // Event type (CLICK=1, LONG_PRESS=2...)
       uint16_t handler_offset;   // Handler function name offset
   };

Embedded Implementation
~~~~~~~~~~~~~~~~~~~~~~~

Binary loader (~2KB):

.. code-block:: c

   #include "gui_hml_bin_loader.h"

   // Direct memory-mapped loading
   gui_obj_t *ui = gui_hml_bin_load("/flash/ui/main.bin");

   // Or load from memory
   extern const uint8_t main_ui_bin[];
   gui_obj_t *ui = gui_hml_bin_load_mem(main_ui_bin);

Advantages
~~~~~~~~~~

* Tiny parser (~2KB ROM)
* Fast loading (can use direct memory mapping)
* UI can be OTA updated
* Smaller file size than XML

Disadvantages
~~~~~~~~~~~~~

* Not human-readable, difficult to debug
* Requires designer support for compilation
* Format upgrades need compatibility handling

Use Cases
~~~~~~~~~

* Resource-constrained but OTA needed
* Startup speed requirements
* Production release phase

Event Handling Mechanism (Common)
----------------------------------

All three solutions use **function name binding** mechanism:

Configure Events in Designer
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

Configure event handler function names for components in property panel.

Generate Callback Stubs on Export
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

.. code-block:: c

   // main_callbacks.c (first generation, protected afterwards)

   /* @protected start on_button1_click */
   void on_button1_click(gui_obj_t *obj) {
       // TODO: Implement click logic
   }
   /* @protected end on_button1_click */

   /* @protected start on_button1_long_press */
   void on_button1_long_press(gui_obj_t *obj) {
       // TODO: Implement long press logic
   }
   /* @protected end on_button1_long_press */

Register Callback Table in Firmware
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

.. code-block:: c

   // callback_registry.c
   #include "main_callbacks.h"

   const callback_entry_t g_callbacks[] = {
       {"on_button1_click", on_button1_click},
       {"on_button1_long_press", on_button1_long_press},
       // ... other callbacks
       {NULL, NULL}
   };

   // Find callback function
   void *find_callback(const char *name) {
       for (int i = 0; g_callbacks[i].name; i++) {
           if (strcmp(g_callbacks[i].name, name) == 0) {
               return g_callbacks[i].func;
           }
       }
       return NULL;
   }

Data Binding (Solutions 2 & 3 Support)
---------------------------------------

Use Data Binding in HML
~~~~~~~~~~~~~~~~~~~~~~~~

.. code-block:: xml

   <hg_text id="title" text="{data.title}" />
   <hg_text id="battery" text="{data.battery}%" />
   <hg_image id="icon" src="{data.icon_path}" />

Bind Data on Embedded Side
~~~~~~~~~~~~~~~~~~~~~~~~~~~

.. code-block:: c

   // Define data structure
   typedef struct {
       char title[32];
       int battery;
       char icon_path[64];
   } ui_data_t;

   // Bind data
   ui_data_t data = {"Message Notification", 85, "/assets/msg.bin"};
   gui_hml_bind_data(ui, &data);

Solution Selection Guide
-------------------------

.. code-block:: text

                  ┌─────────────────┐
                  │ Need OTA UI     │
                  │ updates?        │
                  └────────┬────────┘
                           │
            ┌──────────────┴──────────────┐
            │ No                          │ Yes
            ▼                             ▼
    ┌───────────────┐           ┌─────────────────┐
    │ Solution 1:   │           │ Flash sufficient?│
    │ C Code        │           └────────┬────────┘
    └───────────────┘                    │
                          ┌──────────────┴──────────────┐
                          │ Yes                         │ No
                          ▼                             ▼
                  ┌───────────────┐           ┌───────────────┐
                  │ Solution 2:   │           │ Solution 3:   │
                  │ HML (XML)     │           │ HML (BIN)     │
                  │ (Easy debug)  │           │ (Small size)  │
                  └───────────────┘           └───────────────┘

Performance and Resource Comparison
------------------------------------

.. list-table::
   :header-rows: 1
   :widths: 20 20 20 20 20

   * - Metric
     - Solution 1 (C)
     - Solution 2 (XML)
     - Solution 3 (BIN)
     - Notes
   * - ROM Usage
     - Minimal
     - +10KB
     - +2KB
     - Parser size
   * - Startup Speed
     - Fastest
     - Slow
     - Fast
     - Parsing time needed
   * - OTA Support
     - ✗
     - ✓
     - ✓
     - Remote update capability
   * - Debug Difficulty
     - Low
     - Low
     - High
     - Readability
   * - File Size
     - N/A
     - Large
     - Medium
     - UI description file

Development Recommendations
---------------------------

Development Phase
~~~~~~~~~~~~~~~~~

* Use **Solution 1 (C Code)** for rapid iteration on PC simulator
* Use VSCode plugin build and simulation features
* Leverage protected regions to preserve custom logic

Testing Phase
~~~~~~~~~~~~~

* Choose final solution based on project requirements
* If OTA needed, switch to **Solution 2 (XML)** for testing
* Validate resource usage and performance metrics

Release Phase
~~~~~~~~~~~~~

* Sufficient resources: Use **Solution 2 (XML)** for flexibility
* Resource-constrained: Use **Solution 3 (BIN)** or **Solution 1 (C)**
* Ensure all callback functions are fully implemented

Common Questions
----------------

Q: How to switch between the three solutions?
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

A: Designer can select export format when generating code. Existing projects can be re-exported to different formats.

Q: Can Solutions 2 and 3 be mixed?
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

A: Yes. For example, use XML during development, compile to BIN for release. Data structures are compatible.

Q: Is OTA UI update secure?
~~~~~~~~~~~~~~~~~~~~~~~~~~~~

A: Signature verification mechanism must be implemented in firmware to ensure UI files come from trusted sources.

Q: How significant are performance differences?
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

A: Solution 1 is fastest (direct execution), Solution 3 next (simple parsing), Solution 2 slowest (XML parsing). Actual difference depends on UI complexity, typically tens of milliseconds.

Next Steps
----------

* See :doc:`code_generation` for code generation details
* Learn :doc:`build_simulation` to test on PC
* Refer to :doc:`resource_management` to optimize resource usage
* Read HoneyGUI SDK porting documentation for platform adaptation
