HML File Format
===============

HML (HoneyGUI Markup Language) is an XML-based interface description language used to define the structure and properties of HoneyGUI interfaces.

Overview
--------

HML files are the underlying storage format for the visual designer, featuring the following characteristics:

* **Human Readable**: Based on XML format, easy to read and edit
* **Version Control Friendly**: Plain text format, suitable for Git and other version control tools
* **Complete Description**: Contains all information including component structure, properties, events, etc.
* **Platform Independent**: Can be converted to code for different target platforms (C, Python, etc.)

Basic Structure
---------------

The basic structure of an HML file is as follows:

.. code-block:: xml

   <?xml version="1.0" encoding="UTF-8"?>
   <hml>
       <meta>
           <project name="MyApp" resolution="480X272" />
       </meta>
       <view>
           <!-- Interface component definitions -->
       </view>
   </hml>

Root Elements
~~~~~~~~~~~~~

* **hml**: Document root element
* **meta**: Metadata containing project name, resolution and other configuration
* **view**: Interface content container holding all component definitions

Component Definition
--------------------

Basic Syntax
~~~~~~~~~~~~

Each component is represented by an XML element:

.. code-block:: xml

   <hg_button id="btn1" x="10" y="20" w="100" h="40" text="Click Me"/>

Components can be nested:

.. code-block:: xml

   <hg_view id="container" x="0" y="0" w="480" h="272">
       <hg_label id="title" x="10" y="10" w="200" h="30" text="Hello"/>
       <hg_button id="btn1" x="10" y="50" w="100" h="40" text="OK"/>
   </hg_view>

Required Attributes
~~~~~~~~~~~~~~~~~~~

All components must include the following attributes:

* **id**: Unique component identifier

  * Rules: Letters, numbers, underscores; cannot start with a number
  * Examples: ``main_view``, ``login_button``, ``user_avatar``

Position and Size Attributes
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

* **x**: Horizontal position (pixels)
* **y**: Vertical position (pixels)
* **width**: Width (pixels)
* **height**: Height (pixels)

.. note::
   Position is relative to the upper left corner of the parent component.

Component Types
---------------

Container Components
~~~~~~~~~~~~~~~~~~~~

hg_view
"""""""

View container for organizing layout:

.. code-block:: xml

   <hg_view id="main_view" x="0" y="0" width="480" height="272" entry="true">
       <!-- Child components -->
   </hg_view>

Attributes:

* ``entry``: Whether this is the entry view (at least one per project)
* ``backgroundColor``: Background color (for designer display only)

hg_window
"""""""""

Window container supporting multi-window management:

.. code-block:: xml

   <hg_window id="popup" x="100" y="100" width="280" height="160">
       <!-- Window content -->
   </hg_window>

hg_canvas
"""""""""

Custom drawing canvas:

.. code-block:: xml

   <hg_canvas
       id="drawing"
       x="0" y="0" width="480" height="272"/>

hg_list
"""""""

List container:

.. code-block:: xml

   <hg_list
       id="menu"
       x="0" y="50" width="480" height="200"
       itemWidth="480" itemHeight="50"
       direction="VERTICAL"
       noteNum="5"
       inertia="true"
       loop="false"/>

hg_menu_cellular
""""""""""""""""

Cellular menu container:

.. code-block:: xml

   <hg_menu_cellular
       id="main_menu"
       x="0" y="0" width="480" height="502"
       iconFolder="assets/feature"
       iconSize="64"/>

Basic Widgets
~~~~~~~~~~~~~

hg_button
"""""""""

Button component:

.. code-block:: xml

   <hg_button
       id="submit_btn"
       x="10" y="10" width="100" height="40"
       text="Submit"
       color="#FFFFFF"
       imageOn="assets/button_on.png"
       imageOff="assets/button_off.png"/>

hg_label
""""""""

Text label:

.. code-block:: xml

   <hg_label
       id="title"
       x="0" y="0" width="480" height="50"
       text="Welcome to HoneyGUI"
       fontSize="24"
       color="#333333"
       fontFile="assets/font/Inter24pt-Medium.ttf"
       hAlign="CENTER"
       vAlign="MID"/>

hg_image
""""""""

Image display:

.. code-block:: xml

   <hg_image
       id="logo"
       x="50" y="50" width="100" height="100"
       src="assets/logo.png"/>

Input Widgets
~~~~~~~~~~~~~

hg_input
""""""""

Text input field:

.. code-block:: xml

   <hg_input
       id="username"
       x="20" y="100" width="200" height="40"
       placeholder="Enter username"/>

hg_checkbox and hg_radio
""""""""""""""""""""""""

Checkbox and radio button are not yet implemented in the SDK (TODO) and are not currently supported.

Multimedia Components
~~~~~~~~~~~~~~~~~~~~~

hg_video
""""""""

Video player:

.. code-block:: xml

   <hg_video
       id="player"
       x="0" y="0" width="480" height="272"
       src="assets/video.mp4"
       autoplay="false"
       loop="false"/>

hg_gif
""""""

GIF animation:

.. code-block:: xml

   <hg_gif
       id="anim"
       x="50" y="50" width="150" height="150"
       src="assets/anim.gif"/>

hg_3d
"""""

3D model display:

.. code-block:: xml

   <hg_3d
       id="model"
       x="50" y="50" width="380" height="380"
       modelPath="assets/models/cube.bin"/>

Event Configuration
-------------------

Events are defined using child elements, following the Event-Action pattern.

Basic Syntax
~~~~~~~~~~~~

.. code-block:: xml

   <hg_button id="btn1" x="10" y="10" width="100" height="40" text="Next">
       <events>
           <event type="onClick">
               <action type="switchView" target="page2"
                       switchOutStyle="SWITCH_OUT_TO_LEFT_USE_TRANSLATION"
                       switchInStyle="SWITCH_IN_FROM_RIGHT_USE_TRANSLATION"/>
           </event>
       </events>
   </hg_button>

Event Types
~~~~~~~~~~~

* **onClick**: Click event
* **onLongPress**: Long press event
* **onTouchDown**: Touch down event
* **onTouchUp**: Touch up event
* **onSwipeLeft / onSwipeRight / onSwipeUp / onSwipeDown**: Swipe events (container components)
* **onMessage**: Message listener event

Action Types
~~~~~~~~~~~~

switchView
""""""""""

Switch to another view:

.. code-block:: xml

   <action type="switchView" target="main_menu"
           switchOutStyle="SWITCH_OUT_TO_LEFT_USE_TRANSLATION"
           switchInStyle="SWITCH_IN_FROM_RIGHT_USE_TRANSLATION"/>

switchOutStyle / switchInStyle options:

* Translation: ``SWITCH_OUT_TO_LEFT/RIGHT/TOP/BOTTOM_USE_TRANSLATION``
* Cube: ``SWITCH_OUT_TO_LEFT/RIGHT/TOP/BOTTOM_USE_CUBE``
* Others: ``SWITCH_OUT_NONE_ANIMATION``, ``SWITCH_OUT_ANIMATION_FADE``

sendMessage
"""""""""""

Send custom message:

.. code-block:: xml

   <action type="sendMessage" message="user_logout"/>

callFunction
""""""""""""

Call user-defined C function:

.. code-block:: xml

   <action type="callFunction" functionName="on_button_click"/>

.. note::
   Users need to implement this function in the generated callback file.

Multiple Events
~~~~~~~~~~~~~~~

A component can respond to multiple events:

.. code-block:: xml

   <hg_button id="btn1" x="10" y="10" w="100" h="40" text="Action">
       <events>
           <event type="onClick">
               <action type="callFunction" functionName="handle_click"/>
           </event>
           <event type="onLongPress">
               <action type="callFunction" functionName="handle_long_press"/>
           </event>
       </events>
   </hg_button>

Complete Examples
-----------------

Simple Login Interface
~~~~~~~~~~~~~~~~~~~~~~

.. code-block:: xml

   <?xml version="1.0" encoding="UTF-8"?>
   <hml>
       <meta>
           <project name="LoginApp" resolution="480X272" />
       </meta>
       <view>
           <hg_view id="login_view" x="0" y="0" width="480" height="272" entry="true">
               <!-- Title -->
               <hg_label
                   id="title"
                   x="0" y="20" width="480" height="40"
                   text="User Login"
                   fontSize="24"
                   color="#333333"
                   hAlign="CENTER"
                   vAlign="MID"/>

               <!-- Username input -->
               <hg_label
                   id="username_label"
                   x="40" y="80" width="100" height="30"
                   text="Username:"
                   fontSize="16"/>
               <hg_input
                   id="username_input"
                   x="150" y="80" width="280" height="40"
                   placeholder="Enter username"/>

               <!-- Password input -->
               <hg_label
                   id="password_label"
                   x="40" y="130" width="100" height="30"
                   text="Password:"
                   fontSize="16"/>
               <hg_input
                   id="password_input"
                   x="150" y="130" width="280" height="40"
                   placeholder="Enter password"/>

               <!-- Login button -->
               <hg_button
                   id="login_btn"
                   x="150" y="220" width="180" height="40"
                   text="Login">
                   <events>
                       <event type="onClick">
                           <action type="callFunction" functionName="handle_login"/>
                       </event>
                   </events>
               </hg_button>
           </hg_view>
       </view>
   </hml>

Multi-View Navigation
~~~~~~~~~~~~~~~~~~~~~

.. code-block:: xml

   <?xml version="1.0" encoding="UTF-8"?>
   <hml>
       <meta>
           <project name="NavApp" resolution="480X272" />
       </meta>
       <view>
           <hg_view id="home_view" x="0" y="0" width="480" height="272" entry="true">
               <hg_label
                   id="home_title"
                   x="0" y="20" width="480" height="40"
                   text="Home Page"
                   fontSize="24"
                   hAlign="CENTER"
                   vAlign="MID"/>

               <hg_button
                   id="to_settings"
                   x="190" y="100" width="100" height="40"
                   text="Settings">
                   <events>
                       <event type="onClick">
                           <action type="switchView" target="settings_view"
                                   switchOutStyle="SWITCH_OUT_TO_LEFT_USE_TRANSLATION"
                                   switchInStyle="SWITCH_IN_FROM_RIGHT_USE_TRANSLATION"/>
                       </event>
                   </events>
               </hg_button>

               <hg_button
                   id="to_about"
                   x="190" y="160" width="100" height="40"
                   text="About">
                   <events>
                       <event type="onClick">
                           <action type="switchView" target="about_view"
                                   switchOutStyle="SWITCH_OUT_ANIMATION_FADE"
                                   switchInStyle="SWITCH_IN_ANIMATION_FADE"/>
                       </event>
                   </events>
               </hg_button>
           </hg_view>
       </view>
   </hml>

Manual HML Editing
------------------

Although the visual designer is recommended, you can also directly edit HML files:

1. Right-click on an ``.hml`` file in VSCode
2. Select :guilabel:`Open With Text Editor`
3. Directly modify the XML content
4. After saving, you can reopen it with the designer to see the effect

.. warning::
   When manually editing, ensure the XML format is correct, otherwise the designer may not be able to open it.

Validating HML
~~~~~~~~~~~~~~

The designer automatically validates HML format when opening:

* Check XML syntax
* Verify required attributes
* Check component ID uniqueness
* Validate nesting relationships

If validation fails, detailed error information will be displayed.

Best Practices
--------------

Naming Conventions
~~~~~~~~~~~~~~~~~~

* Use lowercase letters and underscores: ``main_view``, ``login_button``
* Avoid starting with numbers: Use ``view_1`` instead of ``1view``
* Use meaningful names: ``submit_btn`` rather than ``btn1``

Structural Organization
~~~~~~~~~~~~~~~~~~~~~~~

* Use containers to organize related components
* Maintain appropriate nesting levels (no more than 5 levels)
* Use consistent naming patterns for similar components

Comments
~~~~~~~~

HML supports XML comments:

.. code-block:: xml

   <!-- This is a comment -->
   <hg_view id="main">
       <!-- Header area -->
       <hg_view id="header">
           <!-- ... -->
       </hg_view>

       <!-- Content area -->
       <hg_view id="content">
           <!-- ... -->
       </hg_view>
   </hg_view>

Version Control
~~~~~~~~~~~~~~~

* HML is plain text format, suitable for Git management
* Commit changes regularly
* Write clear commit messages
* Use branches for experimental designs

Next Steps
----------

* Learn :doc:`code generation mechanism <code_generation>` to understand how to convert to C code
* Master the :doc:`event system <event_system>` to implement complex interactions
* Explore :doc:`resource management <resource_management>` to manage images and other resources
