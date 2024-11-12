#  Calculator

This example demostrates how to develop a simple "Calculator APP", from which you can learn and understand the basic methods and processes of developing a ui application.
The "Calculator" works just like a traditional calculator, using button widget for user input and text widget for display. Watch the demo video below to see its full functionality.

<br>
<div style="text-align: center"><img src="https://docs.realmcu.com/HoneyGUI/image/sample/Calculator/calculator.gif" width = "400" /></div>
<br>

## Source File
To help learn and be familiar with the development, you can find all source files you may need in path `gui_engine\example\screen_448_368`. The source file for this demostration is `app_calculator.c`, you can find it in the path mentioned for more details.


## Two Steps 
__Step 1:  Declare the app structure__

The app structure saves all the information of ui. Developers should initialize the app structure with the app name and ui design function.

```c
#include <gui_app.h>
static void app_calculator_ui_design(gui_app_t *app);

static gui_app_t calculator =
{
    .screen =
    {
        .name = "calculator",
    },
    .ui_design = app_calculator_ui_design,
};

/*
 * Public API to get app structure
 */
gui_app_t *get_app_calculator(void)
{
    return &calculator;
}
```
__Step 2:  Declare the app ui design function__

The app ui design function adds all the widgets required to form a complex ui. In this example, we add a window widgets and draw the calculator ui. 

```c
static void app_calculator_ui_design(gui_app_t *app)
{
    gui_win_t *win = gui_win_create(&app->screen, "back_win", 0, 0, gui_get_screen_width(),
                                    gui_get_screen_height());

    gui_calculator_create(win, "calculator", 0, 0, 454, 454);
}
```

