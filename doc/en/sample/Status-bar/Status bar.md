#  Status bar

This is a new style status bar. In the non-pull-down state, only the real-time time in small fonts is displayed at the top of the screen. Click on the top to pull down the status bar. When pulling down, the mask color gradually becomes opaque and the time text becomes larger. After pulling down to a certain extent, the status bar becomes fully expanded, and the date and message notification will be displayed.

<br>
<div style="text-align: center"><img src="https://docs.realmcu.com/HoneyGUI/image/sample/Status-bar/status_bar.gif"  /></div>
<br>

##  Implementation
###  File
 Function ```static void status_bar(void *parent, gui_obj_t *ignore_gesture)``` is located in file ```gui_engine\example\screen_454_454\gui_menu\apps_in_menu.c```.
###  Design

```eval_rst
.. raw:: html

    <style>
    .center {
        display: flex;
        justify-content: center;
    }
    </style>

.. raw:: html

    <div class="center">

.. mermaid::

    graph TB
        A[Parent] --> B[win]
        B --> D[Rect hidden]
        B --> E[Text Time]
        B --> H[win hidden]
        H --> F[Text Data]
        H --> G[Text NOTIFS]

.. raw:: html

    </div>
```
* In this status bar, the window widget is the root node. A white semi-transparent background of the status bar is drawn using the rectangle drawing function. Three text boxes are nested, representing time, date, and notification messages, respectively. Among them, the time text box uses a function to cache into an image, because the time display needs to be scaled.The rectangular background and the text of the date and notification messages are initially hidden. Touch screen interactive effects are implemented in the animation callback function of the root node window widget.
* In the animation callback function of the window, first update the contents of the text box for time and date to real-time time and date, in the formats of "07:55" and "Tue, Apr 16" respectively. Then, read touchpad data, determining the display effect of the status bar based on current touch screen information such as gestures, for instance, whether to hide the background, whether to hide the date and notifications, change background transparency, time text box reduction scale, and so on.
* The ```status_bar``` function has a parameter ```ignore_gesture```, which takes the pointer to a widget. This parameter is used to resolve conflicts between gestures on the widget and the status bar. When such a conflict is encountered, the gesture interaction of the respective widget is deactivated through this piece of code: ```if (ignore_gesture) { ignore_gesture->gesture = 1; }```. Here, setting the ```gesture``` attribute to '1' turns off the gesture response of the widget in question.




