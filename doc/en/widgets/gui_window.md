# Window

The widget provides a virtual area for the developer to place application-required widgets. The developer can depend on the requirement to create the space relative to the screen.
For example, Figure-1 creates an area the same as the screen dimension, and the developer can create a space with different sizes, as in Figure-2.

<center><img src="https://foruda.gitee.com/images/1701081169144847122/2f0a8469_13671147.png" /></center>
<br>
<center>Figure-1</center>
<br>

<center><img src="https://foruda.gitee.com/images/1701081183476854396/dec93062_13671147.png" /></center>
<br>
<center>Figure-2</center>
<br>

And the following widgets will take the Window's widget left-top corner as the initial coordinates in Figure-3.
<br>

<center><img src="https://foruda.gitee.com/images/1701081206134160709/80ae8874_13671147.png" /></center>
<br>
<center>Figure-3</center>
<br>

## Usage

### Create widget

You can create a win widget by this api [`gui_win_create(void *parent, const char *name, int16_t x, int16_t y, int16_t w, int16_t h)`](#api).
This `w/h` are the width and height of the win widget.

### Add event

To add an event of widget by this api [gui_obj_add_event_cb(void *obj, gui_event_cb_t event_cb, gui_event_t filter, void *user_data)](#api).
`obj` is the selected widget, `event_cb` is the switching events, `filter` is the way how to trigger event, and `user_data` is the data to transmit.
<br>

<span id="api">

## API

</span>

```eval_rst

.. doxygenfile:: gui_win.h

```
