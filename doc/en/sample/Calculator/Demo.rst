==========================
Calculator
==========================

This example demostrates how to develop a simple "Calculator APP", from which you can learn and understand the basic methods and processes of developing a ui application.
The "Calculator" works just like a traditional calculator, using button widget for user input and text widget for display. Watch the demo video below to see its full functionality.

.. raw:: html

   <br>
   <div style="text-align: center"><img src="https://docs.realmcu.com/HoneyGUI/image/sample/Calculator/calculator.gif" width= "400" /></div>
   <br>


Source File
==========================
To help learn and be familiar with the development, you can find all source files you may need in path :file:`realgui\\example\\screen_448_368\\`. The source file for this demostration is :file:`app_calculator.c`, you can find it in the path mentioned for more details.


Two Steps 
==========================

1. Declare the app structure
-----------------------------

The app initialization function creates all the widgets required to form a complex ui. In this example, we add a window widget and draw the calculator ui. 

.. code-block:: c

    static int app_init(void)
    {
        gui_win_t *win = gui_win_create(gui_obj_get_root(), "back_win", 0, 0, gui_get_screen_width(),
                                        gui_get_screen_height());

        gui_calculator_create(win, "calculator", 0, 0, 454, 454);
        return 0;
    }

    GUI_INIT_APP_EXPORT(app_init);

