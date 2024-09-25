# Window

The widget provides a virtual area for the developer to place application-required widgets. The developer can depend on the requirement to create the space relative to the screen.
For example, Figure-1 creates an area the same as the screen dimension, and the developer can create a space with different sizes, as in Figure-2.

<center><img src="https://foruda.gitee.com/images/1701081169144847122/2f0a8469_13671147.png" /></center>
<center>Figure-1</center>
<br>

<center><img src="https://foruda.gitee.com/images/1701081183476854396/dec93062_13671147.png" /></center>
<center>Figure-2</center>
<br>

And the following widgets will take the Window's widget left-top corner as the initial coordinates in the following figure.
<br>

<center><img src="https://foruda.gitee.com/images/1701081206134160709/80ae8874_13671147.png" /></center>
<br>


## Usage

### Create widget

Create a win widget using this API [gui_win_create(void *parent, const char *name, int16_t x, int16_t y, int16_t w, int16_t h)](#gui_win_create).

### Set Animation

Using this API [gui_win_set_animate(gui_win_t *_this, uint32_t dur, int repeat_count, void *callback, void *p)](#gui_win_set_animate) to set the animation and implement the animation effect in the corresponding callback function.

### Register Callback Function

Register the left/right/up/down slide event of the win widget using the following APIs: [gui_win_left(gui_win_t *_this, void *callback, void *parameter)](#gui_win_left) , [gui_win_right(gui_win_t *_this, void *callback, void *parameter)](#gui_win_right) , [gui_win_up(gui_win_t *_this, void *callback, void *parameter)](#gui_win_up) , [gui_win_down(gui_win_t *_this, void *callback, void *parameter)](#gui_win_down).<br/>
And you can also register the press/release/long press/click event of the win widget by follow APIs: [gui_win_press(gui_win_t *_this, void *callback, void *parameter)](#gui_win_press) , [gui_win_release(gui_win_t *_this, void *callback, void *parameter)](#gui_win_release) , [gui_win_long(gui_win_t *_this, void *callback, void *parameter)](#gui_win_long) , [gui_win_click(gui_win_t *_this, void *callback, void *parameter)](#gui_win_click).

### Set 'hold_tp' State

Set the 'hold_tp' state by [gui_win_hold_tp(gui_win_t *_this, bool hold_tp)](#gui_win_hold_tp).

### Get Progress

Using [gui_win_get_animation_progress_percent(gui_win_t *win)](#gui_win_get_animation_progress_percent) to get the animation progress percentage.

### Set Scale Rate

Set the scale rate for the window both horizontally and vertically by [gui_win_set_scale_rate(gui_win_t *win, float scale_rate_horizontal, float scale_rate_vertical)](#gui_win_set_scale_rate).

### Enable/Disable Scope

Enable or disable the scope of the win widget using [gui_win_set_scope(gui_win_t *win, bool enable)](#gui_win_set_scope).

### Set Opacity

Set the opacity value for the win widget with [gui_win_set_opacity(gui_win_t *win, unsigned char opacity_value)](#gui_win_set_opacity).

### Animation Check

Check if the animation is at its end frame by [gui_win_is_animation_end_frame(gui_win_t *win)](#gui_win_is_animation_end_frame).

### Start Animation

Use [gui_win_start_animation(gui_win_t *win)](#gui_win_start_animation) to start the animation.

### Stop Animation

Stop the animation with [gui_win_stop_animation(gui_win_t *win)](#gui_win_stop_animation).

### Prepare

Win widget preparation in [gui_win_prepare(gui_obj_t *obj)](#gui_win_prepare).

### Append An Animation

Using [gui_win_append_animate(gui_win_t *win, uint32_t dur, int repeat_count, void *callback, void *p, const char *name)](#gui_win_append_animate) to append an animation to a GUI window.
<br>

## API

```eval_rst

.. doxygenfile:: gui_win.h

```
