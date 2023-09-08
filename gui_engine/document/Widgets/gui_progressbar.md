# Progressbar

## Overview

|function  | description  |
|--|--|
|`gui_progressbar_img_v_create()`|create a vertical based on picture progressbar|
|`gui_progressbar_img_h_create()`|Create a horizontal based on picture progressbar|
|`gui_progressbar_movie_create()`|create a  based on pictures frame progressbar|
|`gui_progressbar_set_progress()`|set progress|
|`gui_progressbar_get_progress()`|get progress|
|`gui_progressbar_set_percentage()`|set_percentage|
|`gui_progressbar_get_percentage()`|get_percentage|
|`gui_progressbar_get_max()`|get_max|
|......  |......  |
|--|--|

## API

-  `gui_progressbar_img_v_create()` 
    create a vertical based on picture progressbar

|param  | description  |type|
|--|--|--|
|`parent`|the father widget the progressbar nested in|`gui_obj_t  *`|
|`picture`|the picture background|`void  *`|
|`addr_entry`|get the first address of the image storage address information|`int16_t *`|
|`x`|the X-axis coordinate of the widget|`int16_t `|
|`y`|the Y-axis coordinate of the widget|`int16_t `|
|`return`|the widget object pointer|`gui_progressbar_t `|

-  `gui_progressbar_movie_create()` 
    create a  based on pictures frame progressbar

|param  | description  |type|
|--|--|--|
|`parent`|the father widget the progressbar nested in|`gui_obj_t  *`|
|`picture_array`|the pictures frame|`void  **`|
|`array_length`|picture_array length|`int16_t *`|
|`x`|the X-axis coordinate of the widget|`int16_t `|
|`y`|the Y-axis coordinate of the widget|`int16_t `|
|`return`|the widget object pointer|`gui_progressbar_t `|

-  `gui_progressbar_set_percentage()` 
    set_percentage of the progressbar

|param  | description  |type|
|--|--|--|
|`this`|this|`gui_progressbar_t *`|
|`percentage`|the pictures frame|`float `|
|`return`|the widget object pointer|`void`|

-  `gui_progressbar_get_percentage()` 
    get_percentage of the progressbar

|param  | description  |type|
|--|--|--|
|`this`|this|`gui_progressbar_t *`|
|`return`|the widget object pointer|`float`|

-  `gui_progressbar_get_max()` 
    get_max of the progressbar

|param  | description  |type|
|--|--|--|
|`this`|this|`gui_progressbar_t *`|
|`return`|the widget object pointer|`size_t`|

## Example
The code is to create a progressbaras follows.
+  vertical style

```
gui_progressbar_t *IMG=gui_progressbar_img_h_create(&(app->screen), picture, 0, 0);
```
![vertical](https://foruda.gitee.com/images/1694154259257637618/08ffb2a3_13406851.png "create_v.png")
+ horizontal style

```
gui_progressbar_t *IMG=gui_progressbar_img_h_create(&(app->screen), picture, 0, 0);
```
![horizontal](https://foruda.gitee.com/images/1694154279857524204/57d3e021_13406851.png "create_h.png")

+ set percentage

```
gui_progressbar_set_percentage(IMG, 0.5);
```
![percentage = 50%](https://foruda.gitee.com/images/1694154303391505761/f3d00de0_13406851.png "set_progressbar_percentage.png")
+ get percentage

```
int pro = gui_progressbar_get_percentage(IMG);
```
+ get_max progressbar

```
int pro = gui_progressbar_get_max(IMG);
```



