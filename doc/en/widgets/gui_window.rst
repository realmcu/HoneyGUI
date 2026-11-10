======
Window
======

The widget provides a virtual area for the developer to place application-required widgets. The developer can depend on the requirement to create the space relative to the screen.
For example, Figure-1 creates an area the same as the screen dimension, and the developer can create a space with different sizes, as in Figure-2.

.. figure:: https://foruda.gitee.com/images/1701081169144847122/2f0a8469_13671147.png
   :align: center
   :width: 400px
   :name: Figure-1

.. figure:: https://foruda.gitee.com/images/1701081183476854396/dec93062_13671147.png
   :align: center
   :width: 400px
   :name: Figure-2

And the following widgets will take the Window's widget left-top corner as the initial coordinates in the following figure.

.. figure:: https://foruda.gitee.com/images/1701081206134160709/80ae8874_13671147.png
   :align: center
   :width: 400px


Usage
-----

Create Widget
~~~~~~~~~~~~~

Create a win widget using this API :cpp:any:`gui_win_create`.

Set Animation
~~~~~~~~~~~~~

Using this API :cpp:any:`gui_win_set_animate` to set the animation and implement the animation effect in the corresponding callback function.

Register Callback Function
~~~~~~~~~~~~~~~~~~~~~~~~~~

Register the left/right/up/down slide event of the win widget using the following APIs: :cpp:any:`gui_win_left` , :cpp:any:`gui_win_right` , :cpp:any:`gui_win_up` , :cpp:any:`gui_win_down`.
And you can also register the press/release/long press/click event of the win widget by follow APIs: :cpp:any:`gui_win_press` , :cpp:any:`gui_win_release` , :cpp:any:`gui_win_long` , :cpp:any:`gui_win_click`.

Set 'hold_tp' State
~~~~~~~~~~~~~~~~~~~

Set the 'hold_tp' state by :cpp:any:`gui_win_hold_tp`.

Get Progress
~~~~~~~~~~~~

Using :cpp:any:`gui_win_get_animation_progress_percent` to get the animation progress percentage.

Set Scale Rate
~~~~~~~~~~~~~~

Set the scale rate for the window both horizontally and vertically by :cpp:any:`gui_win_set_scale_rate`.

Enable/Disable Scope
~~~~~~~~~~~~~~~~~~~~

Enable or disable the scope of the win widget using :cpp:any:`gui_win_set_scope`.

Set Opacity
~~~~~~~~~~~

Set the opacity value for the win widget with :cpp:any:`gui_win_set_opacity`.

Animation Check
~~~~~~~~~~~~~~~

Check if the animation is at its end frame by :cpp:any:`gui_win_is_animation_end_frame`.

Start Animation
~~~~~~~~~~~~~~~

Use :cpp:any:`gui_win_start_animation` to start the animation.

Stop Animation
~~~~~~~~~~~~~~

Stop the animation with :cpp:any:`gui_win_stop_animation`.

Prepare
~~~~~~~

Win widget preparation in :cpp:any:`gui_win_prepare`.

Append An Animation
~~~~~~~~~~~~~~~~~~~

Using :cpp:any:`gui_win_append_animate` to append an animation to a GUI window.

API
---

.. doxygenfile:: gui_win.h
