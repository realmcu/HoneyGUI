3D Watch Face
======================

The Koi Clock is a dynamic watch face application designed for smartwatches, perfectly combining traditional time display with vivid 3D koi fish animations to provide users with a unique visual experience and interactive fun.

.. raw:: html

   <br>
   <div style="text-align: center"><img src="https://docs.realmcu.com/HoneyGUI/image/sample/koiclock/koiPank.gif" width= "400" /></div>
   <br>

Core Features
-------------

Dynamic Koi Display
~~~~~~~~~~~~~~~~~~~
- **3 independently swimming koi fish**: Each with unique swimming paths and speeds
- **Realistic swimming animation**: Natural body movements simulating real fish behavior
- **Click interaction**: Tap on koi to trigger acceleration effects

Time Display
~~~~~~~~~~~~
- **Clear time presentation**: Large font for current time display
- **Elegant clock design**: Seamlessly integrated with the koi scene

Visual Effects
~~~~~~~~~~~~~~
- **Water ripple effect**: Produces spreading ripples when touching the screen
- **Light and shadow effects**: Dynamic shadows that follow the koi's movement

Implementation
--------------

Code
~~~~
Function ``static void app_ui_koiclock_design(gui_view_t *view)`` is located in file ``example\application\screen_410_502\app_3d_koiClock.c``.

Widget Tree Design
~~~~~~~~~~~~~~~~~~~
.. figure:: https://foruda.gitee.com/images/1755677527679678751/6a1df64f_13406851.png
   :width: 400px
   :align: center
   :name: Widget Tree Design
   
   Widget Tree Design