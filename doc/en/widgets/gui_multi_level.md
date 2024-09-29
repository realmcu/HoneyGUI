# Multi Level

This widget can be used in GUI applications that require managing complex hierarchical structures, such as nested menu structures and layered display objects. With this widget, developers can flexibly manage and manipulate multi-level subwindows, and achieve the hiding and displaying of specific level and order objects, greatly enhancing the dynamism and interactivity of the interface.

## Usage

### Create Widget

Creating function is [gui_multi_level_create(void *parent, const char *widget_name, void (*ui_design)(gui_obj_t *))](#gui_multi_level_create). This function creates and initializes a new instance of the gui_multi_level_t widget.
### Jump to (Level, Index)

Function is [gui_multi_level_jump(gui_multi_level_t *this, int level, int index)](#gui_multi_level_jump). This function facilitates the jump operation to a specific level and index within the multi-level GUI structure.

## Example
<br>
<div style="text-align: center"><img src="https://docs.realmcu.com/HoneyGUI/image/widgets/multi_level.gif"  /></div>
<br>

- Create multi levels in 0~2 level, according to the nested structure. 
```eval_rst

.. literalinclude:: ../../../gui_engine/example/screen_454_454/gui_menu/apps_in_menu.c
   :language: c
   :start-after: /* Create multi levels in 0~2 level*/
   :end-before: /* GUI_APP_ENTRY(APP_SETTING) end */


```
- The ui_design_xx function will create the dispaly in the specific (level, index) Multi level window, when this window is displayed.
```eval_rst

.. literalinclude:: ../../../gui_engine/example/screen_454_454/gui_menu/apps_in_menu.c
   :language: c
   :start-after: /* define of ui_design_0 of (0,0)*/
   :end-before: /* define of ui_design_1_0 of (1,0)*/


```
<span id = "gui_multi_level_create">

## API

</span>

```eval_rst

.. doxygenfile:: gui_multi_level.h

```
