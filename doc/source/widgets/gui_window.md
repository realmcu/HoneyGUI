# Window

## Overview
The Window Widget is a rectangular container widget.

## API 

-  `gui_win_create()` 
        
        create a window widget

|param  | description  |type|
|--|--|--|
|`parent`|the father widget the window nested in|`void *`|  
|`filename`|the window widget name|`const char *`|
|`x`|the X-axis coordinate|`int`|
|`y`|the Y-axis coordinate|`int`|
|`w`|the width|`int`|
|`h`|the height|`int`|
|`return`|the widget object pointer|`gui_win_t`|  


-  `gui_obj_add_event_cb()` 
        
        monitor gestures only within the window size range 

|param  | description  |type|
|--|--|--|
|`obj`|the window object|`gui_win_t *`|  
|`event_cb`|callback function for the event|`gui_event_cb_t`|
|`filter`|respond to this type of event|`gui_event_t`|
|`user_data`|user data that needs to be transmitted|`void *`|


## Example 
### The following code is used to create a window and add an image to it.
```cpp
static void app_launcher_ui_design(gui_app_t *app)
{
    gui_win_t *win = gui_win_create(&(app->screen), "win", 0, 0, 320, 320);
    gui_tab_t *tb_watch = gui_tab_create(win, "tb_watch", 0, 0, 0, 0, 0, 0);
    extern void page_tb_watch(void *parent);
    page_tb_watch(tb_watch);
}
```
![win](https://foruda.gitee.com/images/1694169886660683122/5a0b4b9e_13408154.png "win.PNG") 



### 8 types of gestures  
- left slide and its callback function
```cpp
static void callback1(){
     gui_log("enter on left\n");
}
```
```cpp
gui_obj_add_event_cb(win, (gui_event_cb_t)callback1, GUI_EVENT_1, 0);
```

- right slide
```cpp
static void callback2(){
     gui_log("enter on right\n");
}
```
```cpp
gui_obj_add_event_cb(win, (gui_event_cb_t)callback2, GUI_EVENT_2, 0);
```
- slide up
```cpp
static void callback3(){
     gui_log("enter on up\n");
}
```
```cpp
gui_obj_add_event_cb(win, (gui_event_cb_t)callback3, GUI_EVENT_3, 0);
```
- slide down
```cpp
static void callback4(){
     gui_log("enter on down\n");
}
```
```cpp
gui_obj_add_event_cb(win, (gui_event_cb_t)callback4, GUI_EVENT_4, 0);
```
- press
```cpp
static void callback5(){
     gui_log("enter on press\n");
}
```
```cpp
gui_obj_add_event_cb(win, (gui_event_cb_t)callback5, GUI_EVENT_TOUCH_PRESSED, 0);
```
- release
```cpp
static void callback6(){
     gui_log("enter on release\n");
}
```
```cpp
gui_obj_add_event_cb(win, (gui_event_cb_t)callback6, GUI_EVENT_TOUCH_RELEASED, 0);
```
- long touch
```cpp
static void callback7(){
     gui_log("enter on long touch\n");
}
```
```cpp
gui_obj_add_event_cb(win, (gui_event_cb_t)callback7, GUI_EVENT_TOUCH_LONG, 0);
```
- click
```cpp
static void callback8(){
     gui_log("enter on click\n");
}
```
```cpp
gui_obj_add_event_cb(win, (gui_event_cb_t)callback8, GUI_EVENT_TOUCH_CLICKED, 0);
```

### gesture test video

[![Watch the video](https://foruda.gitee.com/images/1694169886660683122/5a0b4b9e_13408154.png)](https://drive.google.com/file/d/11g0-r2ntHIZG5vmdE5hANysPnQtJoUAD/view?usp=drive_link)


