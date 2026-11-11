Event System
=============

HoneyDesigner uses an **Event-Action** model. When a component fires an event, one or more actions run in order, so common interactions do not require hand-written C code.

Core Concepts
-------------

* **Event**: a component is clicked, long-pressed, swiped, receives a key, becomes visible, becomes hidden, or receives a message.
* **Action**: page switching, message publishing, calling a C function, or controlling a timer.
* **Message**: a global broadcast mechanism for decoupled component communication.

Use the **Events** tab in the property panel to configure events and actions. Use the **Properties** tab for component properties.

Event Types
-----------

Common Events
~~~~~~~~~~~~~

.. list-table::
   :header-rows: 1
   :widths: 22 22 56

   * - Event
     - Identifier
     - Description
   * - Click
     - ``onClick``
     - Fires on click release
   * - Long press
     - ``onLongPress``
     - Fires after the long-press threshold
   * - Touch down
     - ``onTouchDown``
     - Fires when the finger touches down
   * - Touch up
     - ``onTouchUp``
     - Fires on release; supports ``checkReleaseArea``

Key Events
~~~~~~~~~~

.. list-table::
   :header-rows: 1
   :widths: 22 22 56

   * - Event
     - Identifier
     - Description
   * - Key short press
     - ``onKeyShortPress``
     - Requires ``keyName``
   * - Key long press
     - ``onKeyLongPress``
     - Requires ``keyName``

``keyName`` values: ``Home``, ``Back``, ``Menu``, ``Power``, or a custom name.

Swipe Events
~~~~~~~~~~~~

.. list-table::
   :header-rows: 1
   :widths: 22 22 56

   * - Event
     - Identifier
     - Description
   * - Swipe left
     - ``onSwipeLeft``
     - Swipe toward the left
   * - Swipe right
     - ``onSwipeRight``
     - Swipe toward the right
   * - Swipe up
     - ``onSwipeUp``
     - Swipe upward
   * - Swipe down
     - ``onSwipeDown``
     - Swipe downward
   * - Quick swipe left
     - ``onSwipeLeftQuick``
     - Fast swipe from the left edge
   * - Quick swipe right
     - ``onSwipeRightQuick``
     - Fast swipe from the right edge
   * - Quick swipe up
     - ``onSwipeUpQuick``
     - Fast swipe from the top edge
   * - Quick swipe down
     - ``onSwipeDownQuick``
     - Fast swipe from the bottom edge

State, Lifecycle, and Message Events
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

.. list-table::
   :header-rows: 1
   :widths: 22 22 56

   * - Event
     - Identifier
     - Description
   * - Value change
     - ``onValueChange``
     - Fires when a value changes
   * - Checked change
     - ``onCheckedChange``
     - Fires when checked state changes
   * - Show
     - ``onShow``
     - ``hg_view`` only
   * - Hide
     - ``onHide``
     - ``hg_view`` only
   * - Message
     - ``onMessage``
     - Fires for a matching ``message``

Component Supported Events
---------------------------

.. list-table::
   :header-rows: 1
   :widths: 28 72

   * - Component
     - Supported events
   * - ``hg_view``
     - click, longPress, touchDown, touchUp, key, swipe, quick swipe, show, hide, message
   * - ``hg_window``
     - click, longPress, touchDown, touchUp, key, message
   * - ``hg_button``, ``hg_image``
     - click, longPress, touchDown, touchUp, key, message
   * - ``hg_label``
     - click, longPress, key, message
   * - ``hg_switch``, ``hg_checkbox``, ``hg_radio``
     - click, checkedChange, key, message
   * - ``hg_slider``, ``hg_progressbar``, ``hg_input``
     - click, valueChange, key, message
   * - ``hg_list``
     - click, longPress, swipe, quick swipe, key, message
   * - Other graphics and multimedia controls
     - click, longPress, key, message by default; the property panel is authoritative

Action Types
-------------

An event can run multiple actions in order. Common actions:

.. list-table::
   :header-rows: 1
   :widths: 22 30 48

   * - Action
     - Key parameters
     - Description
   * - ``switchView``
     - ``target``, ``switchOutStyle``, ``switchInStyle``
     - Switches pages; one event can contain at most one ``switchView``
   * - ``sendMessage``
     - ``message``
     - Broadcasts a message for other components
   * - ``callFunction``
     - ``functionName``
     - Calls a C function in the ``custom_functions`` protected region
   * - ``controlTimer``
     - ``timerTargets``
     - Starts, stops, or resets timers on other components

Actions are declared with ``<action>`` in HML; an event can contain multiple actions:

.. code-block:: xml

   <event type="onClick">
       <action type="switchView" target="view_page2" />
       <action type="callFunction" functionName="on_next_click" />
   </event>

See :doc:`HML File Format <hml_format>` for switch styles and ``controlTimer`` JSON parameters, and :doc:`Code Generation <code_generation>` for callback implementation.

Message System
--------------

The sender broadcasts with ``sendMessage``, and the receiver subscribes with ``onMessage``:

.. code-block:: xml

   <!-- Sender -->
   <hg_button id="btn_refresh" x="20" y="20" width="120" height="40" text="Refresh">
       <events>
           <event type="onClick">
               <action type="sendMessage" message="refresh_data" />
           </event>
       </events>
   </hg_button>

   <!-- Receiver -->
   <hg_label id="lbl_data" x="20" y="80" width="240" height="40" text="Data">
       <events>
           <event type="onMessage" message="refresh_data">
               <action type="callFunction" functionName="on_refresh_data" />
           </event>
       </events>
   </hg_label>

Generated code uses ``gui_msg_publish`` to broadcast and ``gui_msg_subscribe`` to subscribe. See :doc:`Code Generation <code_generation>` for details.

Generated Code
--------------

Event callbacks are generated under ``src/callbacks/``. Their bodies are overwritten on regeneration, so business logic belongs in the ``custom_functions`` protected region or under ``src/user``.

See :doc:`Code Generation <code_generation>` for callback signatures, generated ``switchView`` calls, and code protection details.

Examples
--------

Button Click Switches Page
~~~~~~~~~~~~~~~~~~~~~~~~~~

.. code-block:: xml

   <hg_button id="btn_next" x="20" y="200" width="120" height="48" text="Next">
       <events>
           <event type="onClick">
               <action type="switchView" target="view_page2" />
           </event>
       </events>
   </hg_button>

Swipe to Switch Pages
~~~~~~~~~~~~~~~~~~~~~

.. code-block:: xml

   <hg_view id="view_page1" x="0" y="0" width="480" height="272" entry="true">
       <events>
           <event type="onSwipeLeft">
               <action type="switchView" target="view_page2"
                       switchOutStyle="SWITCH_OUT_TO_LEFT_USE_TRANSLATION"
                       switchInStyle="SWITCH_IN_FROM_RIGHT_USE_TRANSLATION" />
           </event>
       </events>
   </hg_view>

Key Event
~~~~~~~~~

.. code-block:: xml

   <hg_view id="view_main" x="0" y="0" width="480" height="272" entry="true">
       <events>
           <event type="onKeyShortPress" keyName="Back">
               <action type="callFunction" functionName="on_back_key" />
           </event>
       </events>
   </hg_view>

Best Practices
--------------

* Use clear function names such as ``on_save_click`` and ``on_back_key``.
* Avoid long-running work in frequently triggered events.
* Prefer messages for cross-page communication.
* Verify that every ``switchView`` target exists.
* Keep complex logic in ``src/user`` or ``custom_functions`` so HML stays readable.

Next Steps
----------

* Learn :doc:`code_generation` to understand code generation mechanism
* Refer to :doc:`build_simulation` to build and test applications
* Read :doc:`deployment` for embedded deployment solutions
