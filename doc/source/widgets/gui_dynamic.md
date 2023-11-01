# Dynamic

## Overview
make dynamic frame

## API
-  `gui_dynamic_create_from_mem()` 

|param  | description  |type|
|--|--|--|
|`parent`|the father widget the dynamic nested in|`gui_obj_t  *`|
|`name`|the dynamic name|`const char  *`|
|`addr_entry`|get the first address of the image storage address information|`void  *`|
|`x`|the X-axis coordinate|`int`|
|`y`|the Y-axis coordinate|`int`|
|`total_cnt`|total count frame|`uint8_t `|
|`interval_time_ms`|interval time|`uint32_t `|
|`duration_time_ms`|duration time|`uint32_t `|

## Example
+ classic style

```
gui_dynamic_img_t *dym = gui_dynamic_create_from_mem(&(app->screen), "demo", frame_list, 100, 100, 6, 50, 50*10000);
```

+ classic style video


```eval_rst

 
.. raw:: html

    <iframe src="https://drive.google.com/file/d/1tq2wCY16waEF7jcq0R3DhCDXSg5pRSvH/preview" width="640" height="480" allow="autoplay"></iframe>



 

```


