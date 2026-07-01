.. _Fruit Ninja:

==============
Fruit Ninja
==============

This example demonstrates how to develop a simple 'Fruit Ninja' :term:`APP`, from which you can learn and understand the basic methods and processes of developing a :term:`UI` application. Earn points by cutting fruits within one minute, and the game will be over if cut a bomb. Watch the demo video below to see its full functionality.

.. raw:: html

   <br>
   <div style="text-align: center"><img src="https://foruda.gitee.com/images/1753348341025882496/50867451_10737458.gif" width= "450" /></div>
   <br>

.. _Requirements:

Requirements
-------------
Refer to the Installation section of :ref:`Get Started` .

.. _Source File:

Source File
------------
To help learn and be familiar with the development, you can find all source files you may need in path ``example\application\screen_410_502``. The source file for this demostration is ``app_box2d_fruit_ninja.cpp``, you can find it in the path mentioned for more details.

.. _Configurations:

Configurations
---------------

.. literalinclude:: ../../../../example/application/screen_410_502/app_box2d_fruit_ninja.cpp
   :language: c
   :dedent:
   :start-after: /* fruit ninja screen config start */
   :end-before: /* fruit ninja screen config end */

.. _Usage Steps:

Usage Steps
------------

Step 1:  Declare Ui Design Function
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

.. literalinclude:: ../../../../example/application/screen_410_502/app_box2d_fruit_ninja.cpp
   :language: c
   :dedent:
   :start-after: /* fruit ninja design entry start */
   :end-before: /* fruit ninja design entry end */

Step 2:  Call Function
~~~~~~~~~~~~~~~~~~~~~~~~~

.. literalinclude:: ../../../../example/application/screen_410_502/app_box2d_fruit_ninja.cpp
   :language: c
   :dedent:
   :start-after: /* fruit ninja view instance start */
   :end-before: /* fruit ninja view instance end */

.. _Design Ideas:

Design Ideas
-------------
+ In this APP, box2d was used to create solids to simulate the movement of objects in a gravitational environment, given parameters such as the initial velocity of the x-axis and y-axis during initialization.

   .. literalinclude:: ../../../../example/application/screen_410_502/app_box2d_fruit_ninja.cpp
      :language: c
      :dedent:
      :start-after: /* fruit ninja dynamic body start */
      :end-before: /* fruit ninja dynamic body end */

+ The radius of the solid is set to a small value in order to minimize the effect of objects colliding with each other, since mutual collisions are detrimental to the gameplay.

   .. literalinclude:: ../../../../example/application/screen_410_502/app_box2d_fruit_ninja.cpp
      :language: c
      :dedent:
      :start-after: /* fruit ninja body shape start */
      :end-before: /* fruit ninja body shape end */

+ The position and rotation angle of the fruits (and bomb) are updated in the callback function using the solid's center point mapping and displayed in the image component. The position and initial velocity of the solid is reset when the position of the fruit is outside the display interface.

   .. literalinclude:: ../../../../example/application/screen_410_502/app_box2d_fruit_ninja.cpp
      :language: c
      :dedent:
      :start-after: /* fruit ninja get position start */
      :end-before: /* fruit ninja get position end */

+ Cutting fruit uses the structure touch_info, detecting the touch point release indicates the completion of a cut (to get the initial point of the touch screen and the displacement of the x-axis and y-axis), and the content of the cut will be judged.

   .. literalinclude:: ../../../../example/application/screen_410_502/app_box2d_fruit_ninja.cpp
      :language: c
      :dedent:
      :start-after: /* fruit ninja cut judgment start */
      :end-before: /* fruit ninja cut judgment end */

+ If there are two intersection points between the cut line and the picture rectangle, it means that the cut is valid.

   .. literalinclude:: ../../../../example/application/screen_410_502/app_box2d_fruit_ninja.cpp
      :language: c
      :dedent:
      :start-after: /* fruit ninja cut check start */
      :end-before: /* fruit ninja cut check end */

+ Note that when calculating the intersection point, the rotated endpoint information of the picture needs to be brought into the calculation of the rotation angle to be consistent with the display, so that the accuracy of the cutting judgment can be improved.

   .. literalinclude:: ../../../../example/application/screen_410_502/app_box2d_fruit_ninja.cpp
      :language: c
      :dedent:
      :start-after: /* fruit ninja rotate points start */
      :end-before: /* fruit ninja rotate points end */

+ Update the fruit picture to two pictures after cutting (corresponding to two gui_img_t pointers) and count the score. Multiple different objects can be cut in a single cut.

   .. literalinclude:: ../../../../example/application/screen_410_502/app_box2d_fruit_ninja.cpp
      :language: c
      :dedent:
      :start-after: /* fruit ninja refresh half cut start */
      :end-before: /* fruit ninja refresh half cut end */

+ Note that a flag can be used to mark the cut status of the fruit to prevent scoring errors as well as to facilitate updating the position of the cut picture.

+ When the cut fruit moves outside the display it will reset the position and initial velocity of the solid and restore the cutting effect.

   .. literalinclude:: ../../../../example/application/screen_410_502/app_box2d_fruit_ninja.cpp
      :language: c
      :dedent:
      :start-after: /* fruit ninja restore cut start */
      :end-before: /* fruit ninja restore cut end */
