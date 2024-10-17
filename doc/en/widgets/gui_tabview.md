# Tabview

The tabview widget is the container to store and depends on user selection to change the tab for different functions. As shown in the following figure, each rectangle represents a different tab widget, and 9 tab widgets together form a tab container, with the blue area representing the current display area. The 'idx' stands for the index on the X-axis, and the 'idy' stands for the index on Y-axis.

<br>
<center><img width= "600" img src="https://foruda.gitee.com/images/1727058851305177488/b3542c64_13408154.png" /></center>
<br>

## Usage

### Create Tabview Widget
The [gui_tabview_create(void *parent, const char *name, int16_t x, int16_t y, int16_t w, int16_t h)](#gui_tabview_create) function can be used to establish a tabview.

### Switch the Tab

In usage scenarios, tab switching is generally accomplished by horizontal or vertical swiping. However, if users wish to quickly navigate to a specific tab page, they can use the function [gui_tabview_jump_tab(gui_tabview_t *parent_tabview, int8_t idx, int8_t idy)](#gui_tabview_jump_tab ) to achieve this. This function allows for jumping to a specified tab page by using the indices `idx` and `idy` to specify the position of the tab page.

### Set Tabview Style
Developers can use [gui_tabview_set_style(gui_tabview_t *this, T_SLIDE_STYLE style)](#gui_tabview_set_style) to set the desired style of tabview. By default, the classic style is employed. The available switching effects include the following:

```eval_rst

.. literalinclude:: ../../../gui_engine/widget/gui_tabview.h
   :language: c
   :start-after: /* T_SLIDE_STYLE enum start*/
   :end-before: /* T_SLIDE_STYLE enum end*/

```

### Set Tabview Loop

The tabview loop function is a clever feature that allows users to easily navigate through all the tabs that have been created. By enabling the reverse loop display function using [gui_tabview_loop_x(gui_tabview_t *tabview, bool loop)](#gui_tabview_loop_x) for the x-axis and [gui_tabview_loop_y(gui_tabview_t *tabview, bool loop)](#gui_tabview_loop_y) for the y-axis, as shown in Figure-a, you will see a blue outer box indicating the visible area of the screen for the tab control. The direction of the arrow represents the direction of tab switching. When swiping left, once all tabs have moved to the left side of the visible area, another left swipe will cycle back to the leftmost tab that was created. Conversely, as shown in Figure-b, when swiping right again, the display will cycle from the rightmost tab.

<br>
<center><img src="https://foruda.gitee.com/images/1707118586546869079/2d4816ec_10641540.png" /></center>
<center>Figure-a</center>
<br>

<br>
<center><img src="https://foruda.gitee.com/images/1707118609354334424/c6a664b2_10641540.png" /></center>
<center>Figure-b</center>
<br>

## Example

Please refer to the section: [Tab](./gui_tab.md)


## API


```eval_rst

.. doxygenfile:: gui_tabview.h

```
