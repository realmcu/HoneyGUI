Rainbow Analog Clock
=====================

The example creates a scene for a "Rainbow Analog Clock" using graphical display and animation effects. This application integrates the Box2D physics engine to simulate the movement of capsule objects along a circular path. These capsules undergo dynamic movement and collision simulation based on user interactions and defined physical rules, creating a rich and dynamic visual experience.

.. raw:: html

   <br>
   <div style="text-align: center"><img src="https://docs.realmcu.com/HoneyGUI/image/sample/rainbow_analog/rainbow_analog.gif" width= "400" /></div>
   <br>


Core Features
-------------

Analog Clock Display
~~~~~~~~~~~~~~~~~~~~
- Displays an analog clock with hour, minute, and second hands, which update their positions in real time with system time.
- The clock hands are drawn using vector graphics, and their color dynamically changes based on interactions with the capsules.

Capsule Animation
~~~~~~~~~~~~~~~~~
- Capsules of different colors and images rotate along a circular path, creating vibrant animation effects.
- Each capsule moves towards the center of the circle, with added random angular offsets to increase movement randomness and realism.
- Users can interact by clicking to apply external force to capsules, triggering a visual explosion effect.

User Interaction
~~~~~~~~~~~~~~~~
- Users can click a specific location on the screen to trigger an outward bouncing effect for capsules within a certain radius from the click point.
- After user interaction, capsules gradually return to their original path.

Physics Engine Integration
~~~~~~~~~~~~~~~~~~~~~~~~~~
- The application uses Box2D for dynamic physics simulation to handle the rotation and movement of capsules.

Implementation
--------------

Code
~~~~
The function ``static void app_rainbow_analog_ui_design(gui_view_t *view)`` is located in the file ``example/application/screen_410_502/app_box2d_rainbow_analog.cpp``.


