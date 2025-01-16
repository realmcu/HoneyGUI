C-APP Application
=================

- In this chapter, we will explore the creation and management of C-APPs within GUI framework. A C-APP is essentially an application that users can develop to craft interactive and visually appealing user interfaces. Each C-APP can be opened, closed, switched between, and can incorporate dynamic transition effects during switching.

- The displayed content within a C-APP is organized using a nested widget tree structure. This structure includes container widgets such as windows, scrollable pages, and switchable tabs, as well as content display widgets like text, images, and canvases.

- In addition to the default functions and effects, widgets within C-APPs offer a high degree of customization. Users can set up custom frame animations for widgets and bind events to execute their defined operations. This flexibility enables the creation of highly dynamic and interactive user interfaces tailored to specific needs and requirements.

Define A C-APP
--------------

- Define app handle using a specific name with ``GUI_APP_DEFINE_NAME_ANIMATION`` API:

.. literalinclude:: ../../../../realgui/example/screen_454_454/gui_menu/apps_in_menu.c
   :language: c
   :start-after: /*Define a app with name APP_STOPWATCH*/
   :end-before: /*Define a app with name APP_STOPWATCH end*/

- There are also other ways available to define the app:

   - ``GUI_APP_DEFINE`` 
 
   - ``GUI_APP_DEFINE_NAME`` 

   - ``GUI_APP_DEFINE_NAME_ANIMATION_FUNC_CUSTOM``

   - ``struct gui_app``


- Define app UI design entry function with ``GUI_APP_ENTRY`` API. 

- The UI design entry function will be executed once when the app startup.

.. literalinclude:: ../../../../realgui/example/screen_454_454/gui_menu/apps_in_menu.c
   :language: c
   :start-after: /*define the app's ui design*/
   :end-before: /*define the app's ui design end*/



Create The Widget Tree of A C-APP
----------------------------------

- This is a clock app, serving as an example for this section. 

- In the image below, you can see that the app interface has options for a stopwatch and a countdown timer. 

- Clicking on these options allows you to switch between them.

.. raw:: html

   <br>
   <div style="text-align: center"><img src="https://foruda.gitee.com/images/1727059552792713966/15519dc3_10088396.gif" width= "400" /></div>
   <br>

The graph below shows the widget tree structure simplified:

- SCREEN:APP_STOPWATCH: The main container for the stopwatch app.

   - WINDOW:LEFT_BUTTON: The window containing the left button.

      - CANVAS_RECT:LEFT_BUTTON: The background canvas of the left button.

      - TEXTBOX:LEFT_BUTTON: The text label for the left button.

   - WINDOW:RIGHT_BUTTON: The window containing the right button.

      - CANVAS_RECT:RIGHT_BUTTON: The background canvas of the right button.

      - TEXTBOX:RIGHT_BUTTON: The text label for the right button.

   - MULTI_LEVEL:0_0: A multi-level container.

      - MULTI_LEVEL:1_0: A sub-container within the multi-level container, for the stopwatch.

      - MULTI_LEVEL:1_1: Another sub-container within the multi-level container, for the countdown timer.

.. image:: https://foruda.gitee.com/images/1727332782414751516/5754a436_10088396.png
   :align: center

C-APP Operations
-----------------

Here is how the earlier mentioned operations could be applied specifically to the Stopwatch app:

- ``GUI_APP_SHUTDOWN(APP_STOPWATCH)`` : This command will close the Stopwatch application. If the app is running a timer, it will stop the timer and close the interface. Any associated resources will be freed upon shutdown.

- ``GUI_APP_STARTUP(APP_STOPWATCH)`` : This command will initialize and start the Stopwatch application. The user interface will be displayed, and the app will be ready to start recording time.

- ``GUI_APP_SWAP(APP_STOPWATCH, APP_MAP)`` : This will switch from the currently running Stopwatch app to the Map app.

C-APP Transition Animation
----------------------------------


C-APP provides a robust feature set for managing transition animations between applications. It offers three main functionalities: ``built-in animations``, ``custom animations``, and ``layer management``. These features are designed to enhance the user experience by providing smooth and visually appealing transitions.

- Built-in Animations

   C-APP allows developers to easily implement built-in animations for app transitions using the ``GUI_APP_DEFINE_NAME_ANIMATION`` API. This API lets you specify the transition animations that occur when an app is opened or closed. The second parameter is used to define the animation for opening an app, while the third parameter specifies the animation for closing an app, such as ``GUI_APP_ANIMATION_1``. This straightforward API streamlines the process of integrating transition effects within your applications.

- Custom Animations

   For more complex or unique animation requirements, C-APP supports custom animations through the ``GUI_APP_DEFINE_NAME_ANIMATION_FUNC_CUSTOM`` API. This feature enables developers to set custom animation callback functions for both opening and closing transitions. The second parameter is the callback function for the opening animation, and the third parameter is for the closing animation. These callback functions are defined using the ``GUI_ANIMATION_CALLBACK_FUNCTION_DEFINE`` API. This API provides an animation structure ``gui_animate_t`` instance as an argument, which includes members that offer insights into the progress and status of the animation, allowing for fine-tuned control and customization.

- Layer Management

   C-APP also includes APIs for managing the layering of applications, which can be crucial for visual hierarchy and user experience. The ``gui_app_layer_top`` and ``gui_app_layer_bottom`` APIs allow developers to define the layer relationship between the currently active app and the app that is about to open. This functionality ensures the correct ordering of windows and can help in maintaining the intended focus and organization of the app interfaces.

Example
^^^^^^^^^^^^^

- Built-in Animations

   * Define a C-APP

      * Startup Animation: Zoom In from Screen Center (``GUI_APP_ANIMATION_1``)

      * Shutdown Animation: Zoom Out to Screen Center (``GUI_APP_ANIMATION_5``)

   * Swap to the C-APP

      * From app watch to APP_STOPWATCH


.. literalinclude:: ../../../../realgui/example/screen_454_454/gui_menu/apps_in_menu.c
   :language: c
   :start-after: /*Define a app with name APP_STOPWATCH*/
   :end-before: /*Define a app with name APP_STOPWATCH end*/


.. literalinclude:: ../../../../realgui/example/screen_454_454/gui_menu/menu_in_tab.c
   :language: c
   :start-after: /*swap to APP_STOPWATCH*/
   :end-before: /*swap to APP_STOPWATCH end*/


.. raw:: html

   <br>
   <div style="text-align: center"><img src="https://foruda.gitee.com/images/1736837380134649772/48ffca47_10088396.gif" width= "400" /></div>
   <br>



- Custom Animations

   * Define a C-APP

      * Startup Animation: Pop-Up from Bottom of Screen (``heart_rate_startup``)

      * Shutdown Animation:  Slide Down to Disappear (``heart_rate_shutdown``)

   * Swap to the C-APP

      * From app watch to APP_HEART_RATE




.. literalinclude:: ../../../../realgui/example/screen_454_454/gui_menu/apps_in_menu.c
   :language: c
   :start-after: /*Define a app with name APP_HEART_RATE*/
   :end-before: /*Define a app with name APP_HEART_RATE end*/
   

.. literalinclude:: ../../../../realgui/example/screen_454_454/gui_menu/menu_in_tab.c
   :language: c
   :start-after: /*swap to APP_HEART_RATE*/
   :end-before: /*swap to APP_HEART_RATE end*/

.. raw:: html

   <br>
   <div style="text-align: center"><img src="https://foruda.gitee.com/images/1736905537073287973/7ded32a2_10088396.gif" width= "400" /></div>
   <br>



API
---

.. doxygenfile:: gui_app.h