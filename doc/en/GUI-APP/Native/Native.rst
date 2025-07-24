C Application
================
- Users can develop in view widgets to create interactive and visually appealing user interfaces. Each view can be switched arbitrarily and can incorporate dynamic transition effects during switching.

- What is displayed in the C application is organized nested Widget tree structure, which uses the view widget as the parent node. This structure includes container widgets such as windows and scrollable lists, as well as content display widgets like text, images, and canvases.

- In addition to the default functions and effects, widgets within C application offer a high degree of customization. Users can set up custom timers for widgets and bind events to execute their defined operations. This flexibility enables the creation of highly dynamic and interactive user interfaces tailored to specific needs and requirements.



Create C Application Widget Tree
---------------------------------
- The root node of the number of widgets can be obtained through the ``gui_obj_get_root()`` function, and a new widget tree is created based on this. The root node of the widget tree is an object of type ``gui_obj_t``, representing the top-level container of the entire application.

- Through functions for creating widgets, such as ``gui_img_create_from_mem()``, various widgets can be created under the root node. Each widget is based on an object of type ``gui_obj_t``, representing a specific user interface element.

- The ``gui_obj_add_event_cb()`` function can add listening events and callback functions to each widget, such as clicks, scroll events, etc.

- The ``gui_obj_create_timer()`` function can add a timer to each widget to perform certain operations within a specific time interval.

- Use the ``GUI_INIT_APP_EXPORT`` macro to add the application's design function to the initialization function table. This function is called when the application starts to initialize and configure the widget tree.

.. literalinclude:: ../../../../example/widget/image/example_gui_img.c
    :language: c
    :start-after: /* gui image widget example start*/
    :end-before: /* gui image widget example end*/


C Application Transition Effect
---------------------------------
HoneyGUI Provides various transition effects, including hand sliding transition and non-hand animation transitions. This function requires to use view widgets. Please refer to :ref:`View` for details.