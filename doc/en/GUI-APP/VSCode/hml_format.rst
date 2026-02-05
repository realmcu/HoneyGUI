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
   <hone:HoneyGUI xmlns:hone="http://www.honeygui.com">
       <HoneyGUI version="1.0">
           <!-- Interface component definitions -->
       </HoneyGUI>
   </hone:HoneyGUI>

Namespace
~~~~~~~~~

* **xmlns:hone**: HoneyGUI namespace declaration
* **version**: HML format version number

Root Elements
~~~~~~~~~~~~~

* **hone:HoneyGUI**: Document root element
* **HoneyGUI**: Actual content container

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
* **w**: Width (pixels)
* **h**: Height (pixels)

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

   <hg_view id="main_view" x="0" y="0" w="480" h="272">
       <!-- Child components -->
   </hg_view>

hg_window
"""""""""

Window container supporting multi-window management:

.. code-block:: xml

   <hg_window id="popup" x="100" y="100" w="280" h="160">
       <!-- Window content -->
   </hg_window>

Basic Widgets
~~~~~~~~~~~~~

hg_button
"""""""""

Button component:

.. code-block:: xml

   <hg_button
       id="submit_btn"
       x="10" y="10" w="100" h="40"
       text="Submit"
       fontSize="16"
       textColor="#FFFFFF"
       backgroundColor="#007AFF"
       normalImage="assets/button_normal.bin"
       pressedImage="assets/button_pressed.bin"/>

hg_label
""""""""

Text label:

.. code-block:: xml

   <hg_label
       id="title"
       x="0" y="0" w="480" h="50"
       text="Welcome to HoneyGUI"
       fontSize="24"
       fontColor="#333333"
       fontFamily="assets/font.bin"
       textAlign="center"/>

hg_image
""""""""

Image display:

.. code-block:: xml

   <hg_image
       id="logo"
       x="50" y="50" w="100" h="100"
       src="assets/logo.bin"
       mode="fit"
       rotation="0"/>

hg_switch
"""""""""

Toggle switch:

.. code-block:: xml

   <hg_switch
       id="wifi_switch"
       x="300" y="20" w="60" h="30"
       checked="true"
       onColor="#34C759"
       offColor="#8E8E93"/>

Input Widgets
~~~~~~~~~~~~~

hg_input
""""""""

Text input field:

.. code-block:: xml

   <hg_input
       id="username"
       x="20" y="100" w="200" h="40"
       placeholder="Enter username"
       maxLength="20"
       fontSize="14"/>

hg_checkbox
"""""""""""

Checkbox:

.. code-block:: xml

   <hg_checkbox
       id="agree"
       x="20" y="150" w="24" h="24"
       checked="false"
       label="I agree to the terms"/>

hg_radio
""""""""

Radio button:

.. code-block:: xml

   <hg_radio
       id="option1"
       x="20" y="180" w="24" h="24"
       groupName="options"
       checked="true"
       label="Option 1"/>

Advanced Widgets
~~~~~~~~~~~~~~~~

hg_progressbar
""""""""""""""

Progress bar:

.. code-block:: xml

   <hg_progressbar
       id="loading"
       x="50" y="200" w="380" h="10"
       min="0" max="100" value="75"
       color="#007AFF"
       backgroundColor="#E5E5EA"/>

hg_slider
"""""""""

Slider:

.. code-block:: xml

   <hg_slider
       id="volume"
       x="50" y="220" w="380" h="30"
       min="0" max="100" value="50"
       thumbColor="#007AFF"
       trackColor="#E5E5EA"/>

hg_canvas
"""""""""

Custom drawing canvas:

.. code-block:: xml

   <hg_canvas
       id="drawing"
       x="0" y="0" w="480" h="272"
       backgroundColor="#FFFFFF"/>

hg_list
"""""""

List container:

.. code-block:: xml

   <hg_list
       id="menu"
       x="0" y="50" w="480" h="200"
       itemHeight="50"
       scrollable="true">
       <!-- List items -->
   </hg_list>

Multimedia Components
~~~~~~~~~~~~~~~~~~~~~

hg_video
""""""""

Video player:

.. code-block:: xml

   <hg_video
       id="player"
       x="0" y="0" w="480" h="272"
       src="assets/video.mp4"
       autoplay="false"
       loop="false"/>

hg_3d
"""""

3D model display:

.. code-block:: xml

   <hg_3d
       id="model"
       x="50" y="50" w="380" h="380"
       modelSrc="assets/model.bin"
       rotationX="0" rotationY="0" rotationZ="0"/>

Event Configuration
-------------------

Events are defined using child elements, following the Event-Action pattern.

Basic Syntax
~~~~~~~~~~~~

.. code-block:: xml

   <hg_button id="btn1" x="10" y="10" w="100" h="40" text="Next">
       <events>
           <event type="onClick">
               <action type="switchView" target="page2" animation="slideLeft"/>
           </event>
       </events>
   </hg_button>

Event Types
~~~~~~~~~~~

* **onClick**: Click event
* **onLongPress**: Long press event (default 1000ms)
* **onSwipe**: Swipe event (up, down, left, right)
* **onChange**: Value change event (for input fields, sliders, etc.)
* **onFocus**: Gain focus
* **onBlur**: Lose focus

Action Types
~~~~~~~~~~~~

switchView
""""""""""

Switch to another view:

.. code-block:: xml

   <action type="switchView" target="main_menu" animation="fade"/>

Animation options:

* ``fade``: Fade in/out
* ``slideLeft``: Slide from right to left
* ``slideRight``: Slide from left to right
* ``slideUp``: Slide from bottom to top
* ``slideDown``: Slide from top to bottom
* ``none``: No animation

sendMessage
"""""""""""

Send custom message:

.. code-block:: xml

   <action type="sendMessage" message="user_logout"/>

callFunction
""""""""""""

Call user-defined C function:

.. code-block:: xml

   <action type="callFunction" function="on_button_click"/>

.. note::
   Users need to implement this function in the generated callback file.

Multiple Events
~~~~~~~~~~~~~~~

A component can respond to multiple events:

.. code-block:: xml

   <hg_button id="btn1" x="10" y="10" w="100" h="40" text="Action">
       <events>
           <event type="onClick">
               <action type="callFunction" function="handle_click"/>
           </event>
           <event type="onLongPress">
               <action type="callFunction" function="handle_long_press"/>
           </event>
       </events>
   </hg_button>

Complete Examples
-----------------

Simple Login Interface
~~~~~~~~~~~~~~~~~~~~~~

.. code-block:: xml

   <?xml version="1.0" encoding="UTF-8"?>
   <hone:HoneyGUI xmlns:hone="http://www.honeygui.com">
       <HoneyGUI version="1.0">
           <hg_view id="login_view" x="0" y="0" w="480" h="272">
               <!-- Title -->
               <hg_label
                   id="title"
                   x="0" y="20" w="480" h="40"
                   text="User Login"
                   fontSize="24"
                   fontColor="#333333"
                   textAlign="center"/>

               <!-- Username input -->
               <hg_label
                   id="username_label"
                   x="40" y="80" w="100" h="30"
                   text="Username:"
                   fontSize="16"/>
               <hg_input
                   id="username_input"
                   x="150" y="80" w="280" h="40"
                   placeholder="Enter username"
                   maxLength="20"/>

               <!-- Password input -->
               <hg_label
                   id="password_label"
                   x="40" y="130" w="100" h="30"
                   text="Password:"
                   fontSize="16"/>
               <hg_input
                   id="password_input"
                   x="150" y="130" w="280" h="40"
                   placeholder="Enter password"
                   type="password"
                   maxLength="20"/>

               <!-- Remember password -->
               <hg_checkbox
                   id="remember"
                   x="150" y="180" w="24" h="24"
                   label="Remember me"/>

               <!-- Login button -->
               <hg_button
                   id="login_btn"
                   x="150" y="220" w="180" h="40"
                   text="Login"
                   backgroundColor="#007AFF"
                   textColor="#FFFFFF">
                   <events>
                       <event type="onClick">
                           <action type="callFunction" function="handle_login"/>
                       </event>
                   </events>
               </hg_button>
           </hg_view>
       </HoneyGUI>
   </hone:HoneyGUI>

Multi-View Navigation
~~~~~~~~~~~~~~~~~~~~~

.. code-block:: xml

   <?xml version="1.0" encoding="UTF-8"?>
   <hone:HoneyGUI xmlns:hone="http://www.honeygui.com">
       <HoneyGUI version="1.0">
           <hg_view id="home_view" x="0" y="0" w="480" h="272">
               <hg_label
                   id="home_title"
                   x="0" y="20" w="480" h="40"
                   text="Home Page"
                   fontSize="24"
                   textAlign="center"/>

               <hg_button
                   id="to_settings"
                   x="190" y="100" w="100" h="40"
                   text="Settings">
                   <events>
                       <event type="onClick">
                           <action type="switchView" target="settings_view" animation="slideLeft"/>
                       </event>
                   </events>
               </hg_button>

               <hg_button
                   id="to_about"
                   x="190" y="160" w="100" h="40"
                   text="About">
                   <events>
                       <event type="onClick">
                           <action type="switchView" target="about_view" animation="fade"/>
                       </event>
                   </events>
               </hg_button>
           </hg_view>
       </HoneyGUI>
   </hone:HoneyGUI>

Manual HML Editing
------------------

Although the visual designer is recommended, you can also directly edit HML files:

1. Right-click on an ``.hml`` file in VSCode
2. Select **Open With Text Editor**
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
* Avoid starting with numbers: Use ``view_1`` instead of ``view1``
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
