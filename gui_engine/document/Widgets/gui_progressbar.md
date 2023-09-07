# Progressbar

## Overview
create a picture progressbar

|function  | description  |
|--|--|
|`gui_progressbar_ctor()`| |
|`gui_progressbar_v_ctor()`| |
|`gui_progressbar_h_ctor()`| |
|`gui_progressbar_create()`|Create a progressbar widget|
|`gui_progressbar_v_create()`|Create a progressbar widget|
|`gui_progressbar_picture_create()`|Create a progressbar widget|
|`gui_progressbar_create()`|create a  based on pictures frame progressbar|
|`gui_progressbar_img_v_create()`|create a vertical based on picture progressbar|
|`gui_progressbar_img_h_create()`|Create a horizontal based on picture progressbar|
|`gui_progressbar_movie_create()`|create a  based on pictures frame progressbar|
|`gui_progressbar_set_progress()`|set progress|
|`gui_progressbar_get_progress()`|get progress|
|`gui_progressbar_set_percentage()`|set_percentage|
|`gui_progressbar_get_percentage()`|get_percentage|
|`gui_progressbar_get_max()`|get_max|
|--|--|

## API

+  `gui_progressbar_picture_create()` 
        
        create a progressbar widget
|Parameter  | Description  |Type|
|--|--|--|
|`parent`|The father widget the progressbar nested in|`void *`|
|`picture_addr`|The picture address|`void  *`|
|`x`|The  X-axis coordinate of the widget|`int16_t`|
|`y`|The  Y-axis coordinate of the widget|`int16_t`|
|`w`|The width of the widget.|`int16_t`|
|`h`|The height of the widget.|`int16_t`|
|`return`|the widget object pointer|`gui_progressbar_t`|

+  `gui_progressbar_create()` 
        
        create a progressbar widget
|Parameter  | Description  |Type|
|--|--|--|
|`parent`|The father widget the progressbar nested in|`void *`|
|`name`|The progressbar name|`const char  *`|
|`x`|The  X-axis coordinate of the widget|`int16_t`|
|`y`|The  Y-axis coordinate of the widget|`int16_t`|
|`w`|The width of the widget.|`int16_t`|
|`h`|The height of the widget.|`int16_t`|
|`return`|the widget object pointer|`gui_progressbar_t`|

+ `gui_progressbar_img_h_create()`

        create a horizontal based on picture progressbar
|Parameter  | Description  |Type|
|--|--|--|
|`parent`|The father widget the progressbar nested in|`void *`|
|`picture`|The picture background|`void  *`|
|`x`|The  X-axis coordinate of the widget|`int16_t`|
|`y`|The  Y-axis coordinate of the widget|`int16_t`|
|`return`|the widget object pointer|`gui_progressbar_t`|

+ `gui_progressbar_movie_create()`

        create a  based on pictures frame progressbar
|Parameter  | Description  |Type|
|--|--|--|
|`parent`|The father widget the progressbar nested in|`void *`|
|`picture_array`|The pictures frame|`void  *`|
|`array_length`|picture_array length| `uint16_t`|
|`x`|The  X-axis coordinate of the widget|`int16_t`|
|`y`|The  Y-axis coordinate of the widget|`int16_t`|
|`return`|the widget object pointer|`gui_progressbar_t`|

+ `gui_progressbar_set_progress()`

        set progress
|Parameter  | Description  |Type|
|--|--|--|
|`this`| |`gui_progressbar_t *`|
|`progress`|The pictures frame|`size_t`|
|`return`|the widget object pointer|`void`|

+ `gui_progressbar_get_progress()`

        get progress
|Parameter  | Description  |Type|
|--|--|--|
|`this`| |`gui_progressbar_t *`|
|`return`|the widget object pointer|`void`|

+ `gui_progressbar_set_percentage()`

        set progress
|Parameter  | Description  |Type|
|--|--|--|
|`this`| |`gui_progressbar_t *`|
|`percentage`|The pictures frame|`float`|
|`return`|the widget object pointer|`void`|

+ `gui_progressbar_get_percentage()`

        get progress
|Parameter  | Description  |Type|
|--|--|--|
|`this`| |`gui_progressbar_t *`|
|`return`|the widget object pointer|`void`|

## Example



