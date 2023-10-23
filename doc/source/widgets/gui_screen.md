# Screen

## Overview
The root widget of the widget tree.
* The screen widget is the root node of a control tree. The screen coordinate system is set as follows. The origin of the polar coordinates is the negative direction of the Y axis, and the positive direction of the polar coordinates is clockwise:
![screen](https://foruda.gitee.com/images/1669963637201743951/88578c71_10088396.jpeg "screen")
## structure 

-  `gui_obj_t` 
        
        base widget structure

|member  | description  |type|
|--|--|--|
|`name`|widget instance's name|`const char *`|  
|`parent`|parent widget pointer|`struct _gui_obj_t *`|
|`x`|the X-axis coordinate relative to parent widget|`int`|
|`y`|the Y-axis coordinater elative to parent widget|`int`|
|`w`|the width|`int`|
|`h`|the height|`int`|

```c
typedef struct _gui_obj_t
{
    const char *name;
    struct _gui_obj_t *parent;//point to father obj

    int16_t x;
    int16_t y;
    int16_t w;
    int16_t h;
    gui_list_t  child_list;
    gui_list_t  brother_list;

    //run time
    void (* obj_update_att)(struct _gui_obj_t *obj);
    void (* obj_prepare)(struct _gui_obj_t *obj);
    void (* obj_draw)(struct _gui_obj_t *obj);
    void (* obj_end)(struct _gui_obj_t *obj);
    void (* obj_destory)(struct _gui_obj_t *obj);

    //void (* obj_cb)(struct _gui_obj_t *obj); todo
    //run time
    obj_type_t type; //no need this , only use name
    uint16_t active         : 1;    // this flag means obj location in screen
    uint16_t not_show       : 1;
    uint16_t cover          : 1;
    uint16_t create_done    : 1;
    uint16_t has_animate    : 1;
    uint16_t event_dsc_cnt  : 5;
    gui_event_dsc_t *event_dsc;
    int16_t dx;//for touch
    int16_t dy;//for touch
    int16_t ax;//absolute value
    int16_t ay;//absolute value
    float sx;
    float sy;
    int16_t tx;
    int16_t ty;
    unsigned char opacity_value;
    rtgui_matrix_t *matrix;

} gui_obj_t;
```

