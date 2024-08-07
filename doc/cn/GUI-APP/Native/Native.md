# C-APP

用GUI API编写C语言代码，然后将其与GUI代码一起编译，以实现预期的显示，称为C-APP。

## 语法
### 使用特定名称定义应用程序

- 使用 ```GUI_APP_DEFINE_NAME``` API定义应用程序句柄
```eval_rst

.. literalinclude:: ../../../../gui_engine/example/screen_454_454/gui_menu/apps_in_menu.c
   :language: c
   :start-after: /*Define a app with name APP_STOPWATCH*/
   :end-before: /*Define a app with name APP_STOPWATCH end*/


```
- 使用 ```GUI_APP_ENTRY``` API定义应用程序UI设计的入口函数



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
### 使用特定名称和入口函数定义应用程序
- 使用名称 ```APP_MENU``` 和入口函数 ```app_menu```，调用 ```GUI_APP_DEFINEE``` API定义应用程序句柄
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



