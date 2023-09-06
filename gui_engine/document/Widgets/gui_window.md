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


## Example 
The code is to create a window as follows.
```cpp
gui_win_t *win = gui_win_create(screen, "win", 0, 0, 320, 320);
```
![win](https://foruda.gitee.com/images/1693987348983821465/e2ffc23f_13408154.png "win.png")