# Tabview

The gui_tabview widget is the container to store and depends on user selection to change the tab for different functions. As shown in the following figure, the "Green Area" represents the actual display region, and there're five rectangles with the red dashed line as the whole gui_tabview area. And each rectangle is the gui_tab widget. The "idx" stands for the index on the X-axis, and the "idy" stands for the index on Y-axis.

<br>
<center><img src="https://foruda.gitee.com/images/1700117108278339023/652a6a38_10641540.png" /></center>
<br>

## Usage

### Create tabview widget
If developers need to construct a container to house tabs, they may utilize the [gui_tabview_create(parent, filename, x, y, w, h)](#gui_tabview_create) function to establish a tabview.

### Switch the gui_tab

[gui_tabview_jump_tab(parent_tabview, idx, idy)](#gui_tabview_create ) is used to set up a jump to a specified tab, and through this function, users can quickly locate the desired tab.

### Set tabview style
Developers can use [gui_tabview_set_style(this, style)](#gui_tabview_create) to set the desired style of tabview. By default, the classic style is employed. The styles are illustrated in the subsequent enumeration.

```eval_rst

.. literalinclude:: ../../../gui_engine/widget/gui_tabview.h
   :language: c
   :start-after: /* T_SLIDE_STYLE enum start*/
   :end-before: /* T_SLIDE_STYLE enum end*/

```

### Set tabview loop

The Tabview loop function is a clever feature that allows users to easily navigate through all the tabs that have been created. By enabling the reverse loop display function using [gui_tabview_loop_x(tabview, loop)](#gui_tabview_create) for the x-axis and [gui_tabview_loop_y(tabview, loop)](#gui_tabview_create) for the y-axis, as shown in Figure-a, you will see a blue outer box indicating the visible area of the screen for the tab control. The direction of the arrow represents the direction of tab switching. When swiping left, once all tabs have moved to the left side of the visible area, another left swipe will cycle back to the leftmost tab that was created. Conversely, as shown in Figure-b, when swiping right again, the display will cycle from the rightmost tab.

<br>
<center><img src="https://foruda.gitee.com/images/1707118586546869079/2d4816ec_10641540.png" /></center>
<center>Figure-a</center>
<br>

<br>
<center><img src="https://foruda.gitee.com/images/1707118609354334424/c6a664b2_10641540.png" /></center>
<center>Figure-b</center>
<br>

## Example

Please refer to the section: [tab](./gui_tab.md)


## API


```eval_rst

.. doxygenfile:: gui_tabview.h

```
