# Obj

The Object implements the basic properties of widgets on a screen. The screen widget is the root node of a control tree. The screen coordinate system is set as follows. The origin of the polar coordinates is the negative direction of the Y axis, and the positive direction of the polar coordinates is clockwise:
<br>

<center><img width="200" img src="https://foruda.gitee.com/images/1718762967150637512/872c46fa_13408154.png" /></center>

## Usage

|Description    |API       |
|:-------:|:-------:|
|Constructor object    |[gui_obj_ctor(gui_obj_t *this, gui_obj_t *parent, const char *name, int16_t x, int16_t y, int16_t w, int16_t h)](#gui_obj_ctor)       |
|Create object    |[gui_obj_create(void *parent, const char *name, int16_t x, int16_t y, int16_t w, int16_t h)](#gui_obj_create)       |
|Add event    |[gui_obj_add_event_cb(void *obj, gui_event_cb_t event_cb, gui_event_t filter, void *user_data)](#gui_obj_add_event_cb)       |
|Set event    |[gui_obj_event_set(gui_obj_t *obj, gui_event_t event_code)](#gui_obj_event_set)       |
|Free the widget tree recursively from the root to the leaves (Preorder traversal)    |[gui_obj_tree_free(void *obj)](#gui_obj_tree_free)       |
|Print the widget tree recursively from the root to the leaves    |[gui_obj_tree_print(gui_obj_t *obj)](#gui_obj_tree_print)       |
|Get the count of one type of widget in the tree    |[gui_obj_tree_count_by_type(gui_obj_t *obj, T_OBJ_TYPE type, int *count)](#gui_obj_tree_count_by_type)       |
|Hide/Show widget    |[gui_obj_tree_show(gui_obj_t *obj, bool enable)](#gui_obj_tree_show)      |
|Enable object show or not    |[gui_obj_show(void *obj, bool show_info)](#gui_obj_show)       |
|Get the root object of tree    |[gui_obj_t *gui_obj_tree_get_root(gui_obj_t *obj)](#gui_obj_tree_get_root)       |
|Get the child object of tree    |[gui_obj_get_child_handle(gui_obj_t *obj, T_OBJ_TYPE child_type)](#gui_obj_get_child_handle)       |
|Judge the object if in range of the rect    |[gui_obj_in_rect(gui_obj_t *obj, int16_t x, int16_t y, int16_t w, int16_t h)](#gui_obj_in_rect)       |
|Skip all actions of the parent object(left/right/down/up slide hold actions)    |[gui_obj_skip_all_parent_left_hold(gui_obj_t *obj)](#gui_obj_skip_all_parent_left_hold) , <br/> [gui_obj_skip_all_parent_right_hold(gui_obj_t *obj)](#gui_obj_skip_all_parent_right_hold) , <br/> [gui_obj_skip_all_parent_down_hold(gui_obj_t *obj)](#gui_obj_skip_all_parent_down_hold) , <br/> [gui_obj_skip_all_parent_up_hold(gui_obj_t *obj)](#gui_obj_skip_all_parent_up_hold)       |
|Skip all actions of the child object(left/right/down/up slide hold actions)    |[gui_obj_skip_all_child_left_hold(gui_obj_t *obj)](#gui_obj_skip_all_child_left_hold) , <br/> [gui_obj_skip_all_child_right_hold(gui_obj_t *obj)](#gui_obj_skip_all_child_right_hold) , <br/> [gui_obj_skip_all_child_down_hold(gui_obj_t *obj)](#gui_obj_skip_all_child_down_hold) , <br/> [gui_obj_skip_all_child_up_hold(gui_obj_t *obj)](#gui_obj_skip_all_child_up_hold)       |
|Skip actions of the other object(left/right/down/up slide hold actions)    |[gui_obj_skip_other_left_hold(gui_obj_t *obj)](#gui_obj_skip_other_left_hold) , <br/> [gui_obj_skip_other_right_hold(gui_obj_t *obj)](#gui_obj_skip_other_right_hold) , <br/> [gui_obj_skip_other_down_hold(gui_obj_t *obj)](#gui_obj_skip_other_down_hold) , <br/> [gui_obj_skip_other_up_hold(gui_obj_t *obj)](#gui_obj_skip_other_up_hold)       |
|Get area of the object    |[gui_obj_get_area(gui_obj_t *obj, int16_t *x, int16_t *y, int16_t *w, int16_t *h)](#gui_obj_get_area)       |
|Point-in-Rectangle Range Check    |[gui_obj_point_in_obj_rect(gui_obj_t *obj, int16_t x, int16_t y)](#gui_obj_point_in_obj_rect)       |
|CRC check    |[gui_obj_checksum(uint8_t seed, uint8_t *data, uint8_t len)](#gui_obj_checksum)       |
|Get widget in tree by name    |[gui_obj_tree_get_widget_by_name(gui_obj_t *obj, const char *name, gui_obj_t **output)](#gui_obj_tree_get_widget_by_name)       |
|Get widget in tree by type    |[gui_obj_tree_get_widget_by_type(gui_obj_t *root, T_OBJ_TYPE type, gui_obj_t **output)](#gui_obj_tree_get_widget_by_type)       |
|Update animate    |[animate_frame_update(gui_animate_t *animate, gui_obj_t *obj)](#animate_frame_update)       |
|Set animate    |[gui_obj_set_animate(gui_animate_t *animate, uint32_t dur, int repeat_count, void *callback, void *p)](#gui_obj_set_animate)       |
|Print the tree in a breadth-first search manner    |[gui_obj_tree_print_bfs(gui_obj_t *root)](#gui_obj_tree_print_bfs)       |

<br>

<span id="api">

## API

</span>

```eval_rst

.. doxygenfile:: gui_obj.h

```
