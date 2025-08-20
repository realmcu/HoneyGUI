Weather
=================

This example implements a weather application that combines different animation effects to simulate various weather conditions such as sunny, cloudy, rainy, snowy, stormy, and windy.

.. raw:: html

   <br>
   <div style="text-align: center"><img src="https://docs.realmcu.com/HoneyGUI/image/sample/weather/weather.gif" width= "400" /></div>
   <br>


Core Features
-------------

Weather List Scrolling
~~~~~~~~~~~~~~~~~~~~~~~~
- Users can switch the currently displayed weather type by scrolling.
- Different weather categories will trigger corresponding animation effects.

Implementation of Weather Animations
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
- Sunny: Animations with light changes; the light source image changes transparency and position over time.
- Cloudy: Simple image display, including sunlight and clouds.
- Rainy: Raindrop icons fall from the top of the screen and reappear after disappearing at the bottom.
- Snowy: Snowflake icons fall slowly while drifting horizontally, and regenerate when reaching a certain position.
- Stormy: Lightning icons appear and disappear gradually, with different lightning images using different opacity cycles.
- Windy: Leaf icons move horizontally and rotate, and regenerate when they go off the screen.

Image Creation and Usage
~~~~~~~~~~~~~~~~~~~~~~~~
- Creates and displays weather icons and effects using different image binary data.
- Images can be manipulated through position, size, transparency, and rotation.

Other Interface Elements
~~~~~~~~~~~~~~~~~~~~~~~~
- Panels use flip effects and transparency changes for visual display.

Implementation
--------------

Code
~~~~
The function ``static void weather_app(gui_view_t *view)`` is located in the file ``example/application/screen_410_502/app_weather.c``.

Widget Tree Design
~~~~~~~~~~~~~~~~~~~

.. figure:: https://foruda.gitee.com/images/1755671160191008924/e32fe511_13408154.png
   :width: 1000px
   :align: center
   :name: Widget Tree Design
   
   Widget Tree Design

