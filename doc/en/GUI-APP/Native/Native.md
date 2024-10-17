# C-APP Application

- In this chapter, we will explore the creation and management of C-APPs within GUI framework. A C-APP is essentially a application that users can develop to craft interactive and visually appealing user interfaces. Each C-APP can be opened, closed, switched between, and can incorporate dynamic transition effects during switching.

- The displayed content within a C-APP is organized using a nested widget tree structure. This structure includes container widgets such as windows, scrollable pages, and switchable tabs, as well as content display widgets like text, images, and canvases.

- In addition to the default functions and effects, widgets within C-APPs offer a high degree of customization. Users can set up custom frame animations for widgets and bind events to execute their defined operations. This flexibility enables the creation of highly dynamic and interactive user interfaces tailored to specific needs and requirements.


## Define A C-APP

- Define app handle using a specific name with ```GUI_APP_DEFINE_NAME``` API 

```eval_rst

.. literalinclude:: ../../../../gui_engine/example/screen_454_454/gui_menu/apps_in_menu.c
   :language: c
   :start-after: /*Define a app with name APP_STOPWATCH*/
   :end-before: /*Define a app with name APP_STOPWATCH end*/


```

- Define app UI design entry function with ```GUI_APP_ENTRY``` API. 
- The UI design entry function will be executed once when the app is created.




```eval_rst

.. literalinclude:: ../../../../gui_engine/example/screen_454_454/gui_menu/apps_in_menu.c
   :language: c
   :start-after: /*define the app's ui design*/
   :end-before: /*define the app's ui design end*/


```

- There are also other ways available to define the app, such as  ```GUI_APP_DEFINEE``` or ```struct gui_app```

## Create The Widget Tree of A C-APP

- This is a clock app, serving as an example for this section. In the image below, you can see that the app interface has options for a stopwatch and a countdown timer. Clicking on these options allows you to switch between them.

<br>

<center><img width= "300" src="https://foruda.gitee.com/images/1727059552792713966/15519dc3_10088396.gif" /></center>
<br>

The graph below shows the widget tree structure simplified:
- SCREEN:APP_STOPWATCH: The main container for the stopwatch app.
   - WINDOW:LEFT_BUTTON: The window containing the left button.
      - CANVAS_RECT:LEFT_BUTTON: The background canvas of the left button.
      - TEXTBOX:LEFT_BUTTON: The text label for the left button.
   - WINDOW:RIGHT_BUTTON: The window containing the right button.
      - CANVAS_RECT:RIGHT_BUTTON: The background canvas of the right button.
      - TEXTBOX:RIGHT_BUTTON: The text label for the right button.
   - MULTI_LEVEL:0_0: A multi-level container.
      - MULTI_LEVEL:1_0: A sub-container within the multi-level container, for the stopwatch.
      - MULTI_LEVEL:1_1: Another sub-container within the multi-level container, for the countdown timer.

<br>

<center><img  src="https://foruda.gitee.com/images/1727332782414751516/5754a436_10088396.png" /></center>
<br>




## C-APP Operations

Here is how the earlier mentioned operations could be applied specifically to the Stopwatch app:
- ```GUI_APP_SHUTDOWN(APP_STOPWATCH)``` : This command will close the Stopwatch application. If the app is running a timer, it will stop the timer and close the interface. Any associated resources will be freed upon shutdown.
- ```GUI_APP_STARTUP(APP_STOPWATCH)``` : This command will initialize and start the Stopwatch application. The user interface will be displayed, and the app will be ready to start recording time.
- ```GUI_APP_SWAP(APP_STOPWATCH, APP_MAP)``` : This will switch from the currently running Stopwatch app to the Map app.



## API

```eval_rst

.. doxygenfile:: gui_app.h

```



