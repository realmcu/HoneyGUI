# C-APP
## Introduction
Write C language code with GUI API, and then compile it together with GUI code to implement the expected display as C-APP.

## Syntax
### Define the app using a specific name

- Define app handle with ```GUI_APP_DEFINE_NAME``` API
```eval_rst

.. literalinclude:: ../../../../gui_engine/example/screen_454_454/gui_menu/apps_in_menu.c
   :language: c
   :start-after: /*Define a app with name APP_STOPWATCH*/
   :end-before: /*Define a app with name APP_STOPWATCH end*/


```
- Define app UI design entry function with ```GUI_APP_ENTRY``` API


```eval_rst

.. mermaid::
 
  graph TD;
      screen --> win;
      win --> text;
      screen --> status_bar;
      screen --> return_widget;
```


```eval_rst

.. literalinclude:: ../../../../gui_engine/example/screen_454_454/gui_menu/apps_in_menu.c
   :language: c
   :start-after: /*define the app's ui design*/
   :end-before: /*define the app's ui design end*/


```
### Define the app using specific name and entry function
- Define app handle with ```GUI_APP_DEFINEE``` API using name ```APP_MENU``` and entry functon ```app_menu```
```eval_rst

.. literalinclude:: ../../../../gui_engine/example/screen_454_454/gui_menu/apps_in_menu.c
   :language: c
   :start-after: /*Define a app with name APP_MENU and entry func app_menu*/
   :end-before: /*Define a app with name APP_MENU and entry func app_menu end*/


```
## API
```eval_rst

.. doxygenfile:: gui_app.h

```



