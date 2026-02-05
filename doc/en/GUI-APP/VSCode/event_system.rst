Event System
============

The HoneyGUI Design event system adopts the **Event-Action** pattern, allowing users to configure event responses for components in the designer without writing code to implement common interaction logic.

Core Concepts
-------------

Event
~~~~~

Interactive behavior triggered by components.

Action
~~~~~~

Operations executed after an event is triggered. An event can be configured with multiple actions that execute in sequence.

Message
~~~~~~~

A global broadcast mechanism where components can send messages and other components can listen and respond.

UI Design Changes
-----------------

Unified Property Panel Tabs
~~~~~~~~~~~~~~~~~~~~~~~~~~~~

The original "View Switch" functionality has been unified into the Event-Action system:

.. code-block:: text

   ┌─────────────────────────────────────┐
   │ [Properties] [Events]               │  ← Used uniformly by all components
   ├─────────────────────────────────────┤

* **Properties**: Basic component attributes (position, size, style, etc.)
* **Events**: Event-Action configuration (includes original "View Switch" functionality)

Event Types
-----------

Common Events (Supported by All Components)
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

.. list-table::
   :header-rows: 1
   :widths: 20 20 60

   * - Event Name
     - Identifier
     - Description
   * - Click
     - ``onClick``
     - Triggered when released after clicking
   * - Long Press
     - ``onLongPress``
     - Triggered when held beyond threshold time
   * - Touch Down
     - ``onTouchDown``
     - Triggered when finger touches down
   * - Touch Up
     - ``onTouchUp``
     - Triggered when finger lifts up

Swipe Events (Supported by Container Components)
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

.. list-table::
   :header-rows: 1
   :widths: 20 20 40 20

   * - Event Name
     - Identifier
     - Description
     - Supported Components
   * - Swipe Left
     - ``onSwipeLeft``
     - Triggered by left swipe
     - hg_view, hg_window
   * - Swipe Right
     - ``onSwipeRight``
     - Triggered by right swipe
     - hg_view, hg_window
   * - Swipe Up
     - ``onSwipeUp``
     - Triggered by upward swipe
     - hg_view, hg_window
   * - Swipe Down
     - ``onSwipeDown``
     - Triggered by downward swipe
     - hg_view, hg_window

State Events (Supported by Specific Components)
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

.. list-table::
   :header-rows: 1
   :widths: 20 20 40 20

   * - Event Name
     - Identifier
     - Description
     - Supported Components
   * - Value Change
     - ``onValueChange``
     - Triggered when value changes
     - hg_slider, hg_progressbar, hg_input
   * - Checked Change
     - ``onCheckedChange``
     - Checked state change
     - hg_checkbox, hg_switch, hg_radio

Lifecycle Events (Supported by Container Components)
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

.. list-table::
   :header-rows: 1
   :widths: 20 20 40 20

   * - Event Name
     - Identifier
     - Description
     - Supported Components
   * - Show
     - ``onShow``
     - Triggered when component is shown
     - hg_view, hg_window
   * - Hide
     - ``onHide``
     - Triggered when component is hidden
     - hg_view, hg_window

Message Events (Supported by All Components)
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

.. list-table::
   :header-rows: 1
   :widths: 20 20 60

   * - Event Name
     - Identifier
     - Description
   * - Message Listener
     - ``onMessage``
     - Triggered when specified message is received

Component Supported Events
---------------------------

.. list-table::
   :header-rows: 1
   :widths: 30 70

   * - Component Type
     - Supported Events
   * - hg_view
     - onClick, onLongPress, onTouchDown, onTouchUp, onSwipeLeft, onSwipeRight, onSwipeUp, onSwipeDown, onShow, onHide, onMessage
   * - hg_window
     - onClick, onLongPress, onTouchDown, onTouchUp, onSwipeLeft, onSwipeRight, onSwipeUp, onSwipeDown, onShow, onHide, onMessage
   * - hg_button
     - onClick, onLongPress, onTouchDown, onTouchUp, onMessage
   * - hg_image
     - onClick, onLongPress, onTouchDown, onTouchUp, onMessage
   * - hg_text
     - onClick, onLongPress, onMessage
   * - hg_switch
     - onClick, onCheckedChange, onMessage
   * - hg_checkbox
     - onClick, onCheckedChange, onMessage
   * - hg_radio
     - onClick, onCheckedChange, onMessage
   * - hg_slider
     - onClick, onValueChange, onMessage
   * - hg_progressbar
     - onClick, onValueChange, onMessage
   * - hg_input
     - onClick, onValueChange, onMessage
   * - hg_video
     - onClick, onLongPress, onMessage
   * - hg_3d
     - onClick, onLongPress, onMessage

Action Types
------------

View Switch Actions
~~~~~~~~~~~~~~~~~~~

.. list-table::
   :header-rows: 1
   :widths: 20 20 30 30

   * - Action Name
     - Identifier
     - Parameters
     - Description
   * - Switch View
     - ``switchView``
     - target, switchOutStyle, switchInStyle
     - View switch with animation

**switchOutStyle Options**:

* Translation: ``SWITCH_OUT_TO_LEFT/RIGHT/TOP/BOTTOM_USE_TRANSLATION``
* Cube: ``SWITCH_OUT_TO_LEFT/RIGHT/TOP/BOTTOM_USE_CUBE``
* Others: ``SWITCH_OUT_NONE_ANIMATION``, ``SWITCH_OUT_ANIMATION_FADE``

**switchInStyle Options**:

* Translation: ``SWITCH_IN_FROM_LEFT/RIGHT/TOP/BOTTOM_USE_TRANSLATION``
* Cube: ``SWITCH_IN_FROM_LEFT/RIGHT/TOP/BOTTOM_USE_CUBE``
* Others: ``SWITCH_IN_NONE_ANIMATION``, ``SWITCH_IN_ANIMATION_FADE``

Message Actions
~~~~~~~~~~~~~~~

.. list-table::
   :header-rows: 1
   :widths: 20 20 20 40

   * - Action Name
     - Identifier
     - Parameters
     - Description
   * - Send Message
     - ``sendMessage``
     - message
     - Broadcast message

Custom Actions
~~~~~~~~~~~~~~

.. list-table::
   :header-rows: 1
   :widths: 20 20 20 40

   * - Action Name
     - Identifier
     - Parameters
     - Description
   * - Call Function
     - ``callFunction``
     - functionName
     - Call user-defined C function

Message System
--------------

Concept
~~~~~~~

The message system provides a decoupled communication mechanism between components:

* **Sender**: Broadcasts messages through ``sendMessage`` action
* **Receiver**: Listens for messages through ``onMessage`` event

Use Cases
~~~~~~~~~

1. **Cross-view Communication**: Operations in view A affect display in view B
2. **Global State Sync**: Multiple components respond to the same state change
3. **Module Decoupling**: Avoid direct references between components

Data Structures
---------------

Event Configuration in HML
~~~~~~~~~~~~~~~~~~~~~~~~~~~

**Button Click to Jump**:

.. code-block:: xml

   <hg_button id="btn1" x="10" y="10" w="100" h="40" text="Click">
     <events>
       <event type="onClick">
         <action type="switchView" target="screen2" />
       </event>
     </events>
   </hg_button>

**View Swipe Switch (Original view_switch functionality)**:

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

TypeScript Type Definitions
~~~~~~~~~~~~~~~~~~~~~~~~~~~~

.. code-block:: typescript

   // Event types
   type EventType =
     // Common events
     | 'onClick' | 'onLongPress' | 'onTouchDown' | 'onTouchUp'
     // Swipe events
     | 'onSwipeLeft' | 'onSwipeRight' | 'onSwipeUp' | 'onSwipeDown'
     // State events
     | 'onValueChange' | 'onCheckedChange'
     // Lifecycle events
     | 'onShow' | 'onHide'
     // Message events
     | 'onMessage';

   // Action types
   type ActionType =
     // View switch (with animation)
     | 'switchView'
     // Message
     | 'sendMessage'
     // Custom
     | 'callFunction';

   // Action definition
   interface Action {
     type: ActionType;
     target?: string;           // Target component/view ID
     message?: string;          // Message name (for sendMessage)
     functionName?: string;     // Function name (for callFunction)
     // switchView specific
     switchOutStyle?: string;   // Exit animation
     switchInStyle?: string;    // Enter animation
   }

   // Event definition
   interface EventConfig {
     type: EventType;
     message?: string;          // Only needed for onMessage event
     actions: Action[];
   }

C Code Generation Implementation
---------------------------------

View Switch (switchView)
~~~~~~~~~~~~~~~~~~~~~~~~~

No independent callback function is generated; instead, registration is done directly in the ``switch_in`` callback of the ``GUI_VIEW_INSTANCE`` macro using ``gui_view_switch_on_event``:

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

Other Events
~~~~~~~~~~~~

Callback functions are generated and bound when components are created:

.. code-block:: c

   // ui/main_ui.c
   gui_obj_add_event_cb(btn1, on_btn1_click, GUI_EVENT_CLICKED, NULL);

   // callbacks/main_callbacks.c
   /* @protected start on_btn1_click */
   void on_btn1_click(gui_obj_t *obj) {
       // User implementation logic
   }
   /* @protected end on_btn1_click */

UI Design
---------

Property Panel - Events Tab
~~~~~~~~~~~~~~~~~~~~~~~~~~~~

.. code-block:: text

   ┌─────────────────────────────────────┐
   │ [Properties] [Events]               │
   ├─────────────────────────────────────┤
   │                                     │
   │ ▼ onSwipeLeft               [Delete]│
   │ ┌─────────────────────────────────┐ │
   │ │ 1. switchView                   │ │
   │ │    Target View: [screen2    ▼]  │ │
   │ │    Exit Anim:   [Slide Left ▼]  │ │
   │ │    Enter Anim:  [From Right ▼]  │ │
   │ │                         [×]     │ │
   │ └─────────────────────────────────┘ │
   │ [+ Add Action]                      │
   │                                     │
   │ [+ Add Event]                       │
   │                                     │
   └─────────────────────────────────────┘

Practical Examples
------------------

Example 1: Button Click to Jump
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

**Scenario**: Click "Next" button to jump to page 2

**HML Configuration**:

.. code-block:: xml

   <hg_button id="next_btn" x="190" y="200" w="100" h="40" text="Next">
     <events>
       <event type="onClick">
         <action type="switchView" target="page2"
                 switchOutStyle="SWITCH_OUT_TO_LEFT_USE_TRANSLATION"
                 switchInStyle="SWITCH_IN_FROM_RIGHT_USE_TRANSLATION" />
       </event>
     </events>
   </hg_button>

**Generated C Code**:

.. code-block:: c

   // Generate callback in callbacks file
   void on_next_btn_click(gui_obj_t *obj) {
       gui_switch_app(gui_current_app(), gui_app_get_by_name("page2"));
   }

Example 2: Swipe Left to Switch View
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

**Scenario**: Swipe left on main view to enter settings view

**HML Configuration**:

.. code-block:: xml

   <hg_view id="main_view" x="0" y="0" w="480" h="272">
     <events>
       <event type="onSwipeLeft">
         <action type="switchView" target="settings"
                 switchOutStyle="SWITCH_OUT_TO_LEFT_USE_TRANSLATION"
                 switchInStyle="SWITCH_IN_FROM_RIGHT_USE_TRANSLATION" />
       </event>
     </events>
   </hg_view>

**Generated C Code**:

.. code-block:: c

   static void main_view_switch_in(gui_view_t *view)
   {
       gui_view_switch_on_event(view, "settings",
                                SWITCH_OUT_TO_LEFT_USE_TRANSLATION,
                                SWITCH_IN_FROM_RIGHT_USE_TRANSLATION,
                                GUI_EVENT_TOUCH_MOVE_LEFT);
       // ...
   }

Example 3: Message Communication
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

**Scenario**: Button sends message, other components listen and respond

**Sender HML**:

.. code-block:: xml

   <hg_button id="refresh_btn" x="10" y="10" w="100" h="40" text="Refresh">
     <events>
       <event type="onClick">
         <action type="sendMessage" message="refresh_data" />
       </event>
     </events>
   </hg_button>

**Receiver HML**:

.. code-block:: xml

   <hg_text id="data_label" x="10" y="60" w="200" h="30" text="Data">
     <events>
       <event type="onMessage" message="refresh_data">
         <action type="callFunction" function="on_refresh_data" />
       </event>
     </events>
   </hg_text>

Example 4: Custom Function Call
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

**Scenario**: Button click executes complex business logic

**HML Configuration**:

.. code-block:: xml

   <hg_button id="save_btn" x="10" y="10" w="100" h="40" text="Save">
     <events>
       <event type="onClick">
         <action type="callFunction" function="save_settings" />
       </event>
     </events>
   </hg_button>

**User Implementation**:

.. code-block:: c

   // callbacks/main_callbacks.c
   /* @protected start save_settings */
   void save_settings(gui_obj_t *obj) {
       // Read various settings
       bool wifi_enabled = get_wifi_switch_state();
       int brightness = get_brightness_value();

       // Save to flash
       flash_write_config(wifi_enabled, brightness);

       // Show notification
       show_toast("Settings saved");
   }
   /* @protected end save_settings */

Best Practices
--------------

Event Naming Conventions
~~~~~~~~~~~~~~~~~~~~~~~~~

* Use clear function names: ``on_save_click``, ``handle_login``
* Include event type: ``on_button_click``, ``on_slider_change``
* Describe business logic: ``save_user_settings``, ``refresh_data``

Performance Optimization
~~~~~~~~~~~~~~~~~~~~~~~~

* Avoid time-consuming operations in frequently triggered events
* Use message system to decouple complex interactions
* Properly use debouncing and throttling

Debugging Tips
~~~~~~~~~~~~~~

* Add log output in callback functions
* Use breakpoints to verify event flow
* Check generated C code to confirm event binding

Next Steps
----------

* Learn :doc:`code_generation` to understand code generation mechanism
* See :doc:`hml_format` to understand HML file format
* Refer to :doc:`build_simulation` to build and test applications
* Read :doc:`deployment` for embedded deployment solutions
