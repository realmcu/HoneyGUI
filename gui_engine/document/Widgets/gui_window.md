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
        
        gesture monitoring  

|param  | description  |type|
|--|--|--|
|`obj`|the window object|`gui_win_t *`|  
|`event_cb`|callback function for the event|`gui_event_cb_t`|
|`filter`|respond to this type of event|`gui_event_t`|
|`user_data`|user data that needs to be transmitted|`void *`|


## Example 
##### The code is to create a window as follows.
```cpp
static void app_launcher_ui_design(gui_app_t *app)
{
   gui_win_t *win = gui_win_create(&(app->screen), "win", 0, 0, 320, 320);
}

```
![win](https://foruda.gitee.com/images/1694143778858247915/4a433567_13408154.png "win.PNG") 



##### 8 types of gestures  
- left slide
```cpp
gui_obj_add_event_cb(win, (gui_event_cb_t)callback, GUI_EVENT_1, 0);
```
- right slide
```cpp
gui_obj_add_event_cb(win, (gui_event_cb_t)callback, GUI_EVENT_2, 0);
```
- slide up
```cpp
gui_obj_add_event_cb(win, (gui_event_cb_t)callback, GUI_EVENT_3, 0);
```
- slide down
```cpp
gui_obj_add_event_cb(win, (gui_event_cb_t)callback, GUI_EVENT_4, 0);
```
- press
```cpp
gui_obj_add_event_cb(win, (gui_event_cb_t)callback, GUI_EVENT_TOUCH_PRESSED, 0);
```
- release
```cpp
gui_obj_add_event_cb(win, (gui_event_cb_t)callback, GUI_EVENT_TOUCH_RELEASED, 0);
```
- long touch
```cpp
gui_obj_add_event_cb(win, (gui_event_cb_t)callback, GUI_EVENT_TOUCH_LONG, 0);
```
- click
```cpp
gui_obj_add_event_cb(win, (gui_event_cb_t)callback, GUI_EVENT_TOUCH_CLICKED, 0);
```

[gesture test video](https://drive.google.com/file/d/12yvcmnR7ZturEVhRu5yeFG6ac0Boy_mS/view?usp=sharing)