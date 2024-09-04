# Menu cellular

The menu cellular widget display a icons set as a honeycomb shape menu.

## Usage

### Create menu cellular widget

Creating function is [gui_menu_cellular_create(void *parent, int icon_size, uint32_t *icon_array[],int array_size)](#gui_menu_cellular_create). Users need to enter icons set's image file array.
### Config offset

Function is [void gui_menu_cellular_offset(gui_obj_t *menu_cellular, int offset_x, int offset_y)](#gui_menu_cellular_create). Users are able to adjust horizontal and vertical offset for better dispaly.

## Example
<br>
<div style="text-align: center"><img src="https://docs.realmcu.com/HoneyGUI/image/widgets/menu_cellular.gif"  /></div>
<br>



```eval_rst

.. literalinclude:: ../../../gui_engine/example/screen_454_454/gui_menu/apps_in_menu.c
   :language: c
   :start-after: /*Define APP_MENU's entry func */
   :end-before: /*Define APP_MENU's entry func end*/
```



<span id = "gui_menu_cellular_create">

## API

</span>

```eval_rst

.. doxygenfile:: gui_menu_cellular.h

```
