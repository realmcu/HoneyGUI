# Tabview

The gui_tabview widget is the container to store and depends on user selection to change the tab for different functions. As in Figure-1, the "Green Area" represents the actual display region, and there're five rectangles with the red dashed line as the whole gui_tabview area. And each rectangle is the gui_tab widget. The "idx" stands for the index on the X-axis, and the "idy" stands for the index on Y-axis.

<br>
<center><img src="https://foruda.gitee.com/images/1700117108278339023/652a6a38_10641540.png" /></center>
<center>Figure-1</center>
<br>

## Usage

### Create tabview widget
If developers need to construct a container to house tabs, they may utilize the [gui_tabview_create(parent, filename, x, y, w, h)](#gui_tabview_create) function to establish a tabview.

### Switch the gui_tab

[gui_tabview_jump_tab(parent_tabview, idx, idy)](#gui_tabview_create ) is used to set up a jump to a specified tab, and through this function, users can quickly locate the desired tab.

### Set tabview style
Developers can use [gui_tabview_set_style(this, style)](#gui_tabview_create) to set the desired style of tabview. By default, the classic style is employed. The styles are illustrated in the subsequent enumeration.

```c
typedef enum t_slide_style
{
    CLASSIC,
    REDUCTION,
    FADE,
    REDUCTION_FADE,
    STACKING,
} T_SLIDE_STYLE;
```

### Set tabview loop
The tabview loop function is a nifty feature that allows you to effortlessly traverse through all tabs that have been created. By enabling this function using [gui_tabview_loop(tabview, loop_or_not)](#gui_tabview_create), as indicated in Figure-2, you'll notice a blue outer box indicating the visible range of the screen. The arrow's direction signifies the direction of tab switching. When sliding left, once all tabs have shifted to the left side of the visible range, another leftward swipe will initiate cycling from the leftmost tab that was created. Conversely, Figure-3 demonstrates that when sliding right again, the display will begin cycling from the rightmost tab.

<br>
<center><img src="https://foruda.gitee.com/images/1707118586546869079/2d4816ec_10641540.png" /></center>
<center>Figure-2</center>
<br>

<br>
<center><img src="https://foruda.gitee.com/images/1707118609354334424/c6a664b2_10641540.png" /></center>
<center>Figure-3</center>
<br>

## Example

Please refer to the page below:

```eval_rst
`tab`_

.. _tab: gui_tab.html

```

<span id="gui_tabview_create">

## API

</span>

```eval_rst

.. doxygenfile:: gui_tabview.h

```
