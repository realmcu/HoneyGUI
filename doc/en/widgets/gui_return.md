# Return

The bubble-shaped curve that can be dragged out on the left side of the screen is used to perform a return operation.

## Usage

### Create A Return Widget

Creating function is [gui_return_create](#gui_return_create). Users need to enter curve animation frame images file array ,and the return operation function.

## Example

```eval_rst

.. literalinclude:: ../../../gui_engine/example/screen_454_454/gui_menu/apps_in_menu.c
   :language: c
   :start-after: /*Define gui_app_return_array*/
   :end-before: /*Define gui_app_return_array end*/

.. literalinclude:: ../../../gui_engine/example/screen_454_454/gui_menu/apps_in_menu.c
   :language: c
   :start-after: /*gui_return_create*/
   :end-before: /*gui_return_create end*/

```
<br>
<div style="text-align: center"><img src="https://docs.realmcu.com/HoneyGUI/image/widgets/return.gif"  /></div>
<br>


```eval_rst

.. note::
  Please refer to ``gui_engine\example\screen_454_454\gui_menu\apps_in_menu.c`` for details.

```


<span id = "gui_return_create">

## API

</span>

```eval_rst

.. doxygenfile:: gui_return.h

```
