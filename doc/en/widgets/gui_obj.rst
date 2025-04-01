===
Obj
===

The Object implements the basic properties of widgets on a screen. The screen widget is the root node of a widget tree. The screen coordinate system is set as follows. The origin of the polar coordinates is the negative direction of the Y axis, and the positive direction of the polar coordinates is clockwise:

.. raw:: html

   <br>
   <div style="text-align: center"><img src="https://foruda.gitee.com/images/1736157403669807769/b83ae9f8_13671147.png" width= "300" /></div>
   <br>

Usage
-----

.. table:: Gui_Obj Table
   :widths: 100 100
   :align: center
   :name: Gui_Obj_Table

   +--------------------------------------------------------------------------------+---------------------------------------------------+
   | Description                                                                    | API                                               |
   +================================================================================+===================================================+
   | Get the root object                                                            | :cpp:any:`gui_obj_get_root`                       |
   +--------------------------------------------------------------------------------+---------------------------------------------------+
   | Create object                                                                  | :cpp:any:`gui_obj_create`                         |
   +--------------------------------------------------------------------------------+---------------------------------------------------+
   | Add event                                                                      | :cpp:any:`gui_obj_add_event_cb`                   |
   +--------------------------------------------------------------------------------+---------------------------------------------------+
   | Set event                                                                      | :cpp:any:`gui_obj_enable_event`                   |
   +--------------------------------------------------------------------------------+---------------------------------------------------+
   | Free the widget tree recursively from the root to the leaves                   | :cpp:any:`gui_obj_tree_free`                      |
   +--------------------------------------------------------------------------------+---------------------------------------------------+
   | Print the widget tree recursively from the root to the leaves                  | :cpp:any:`gui_obj_tree_print`                     |
   +--------------------------------------------------------------------------------+---------------------------------------------------+
   | Get the count of one type of widget in the tree                                | :cpp:any:`gui_obj_tree_count_by_type`             |
   +--------------------------------------------------------------------------------+---------------------------------------------------+
   | Hide/Show widget                                                               | :cpp:any:`gui_obj_tree_show`                      |
   +--------------------------------------------------------------------------------+---------------------------------------------------+
   | Enable object show or not                                                      | :cpp:any:`gui_obj_show`                           |
   +--------------------------------------------------------------------------------+---------------------------------------------------+
   | Get the root object of tree                                                    | :cpp:any:`gui_obj_tree_get_root`                  |
   +--------------------------------------------------------------------------------+---------------------------------------------------+
   | Get the child object of tree                                                   | :cpp:any:`gui_obj_get_child_handle`               |
   +--------------------------------------------------------------------------------+---------------------------------------------------+
   |Judge the object if in range of the rect                                        | :cpp:any:`gui_obj_in_rect`                        |
   +--------------------------------------------------------------------------------+---------------------------------------------------+
   | Skip all actions of the parent object(left/right/down/up slide hold actions)   | + :cpp:any:`gui_obj_skip_all_parent_left_hold`    |
   |                                                                                | + :cpp:any:`gui_obj_skip_all_parent_right_hold`   |
   |                                                                                | + :cpp:any:`gui_obj_skip_all_parent_down_hold`    |
   |                                                                                | + :cpp:any:`gui_obj_skip_all_parent_up_hold`      |
   +--------------------------------------------------------------------------------+---------------------------------------------------+
   | Skip all actions of the child object(left/right/down/up slide hold actions)    | + :cpp:any:`gui_obj_skip_all_child_left_hold`     |
   |                                                                                | + :cpp:any:`gui_obj_skip_all_child_right_hold`    |
   |                                                                                | + :cpp:any:`gui_obj_skip_all_child_down_hold`     |
   |                                                                                | + :cpp:any:`gui_obj_skip_all_child_up_hold`       |
   +--------------------------------------------------------------------------------+---------------------------------------------------+
   | Skip actions of the other object(left/right/down/up slide hold actions)        | + :cpp:any:`gui_obj_skip_other_left_hold`         |
   |                                                                                | + :cpp:any:`gui_obj_skip_other_right_hold`        |
   |                                                                                | + :cpp:any:`gui_obj_skip_other_down_hold`         |
   |                                                                                | + :cpp:any:`gui_obj_skip_other_up_hold`           |
   +--------------------------------------------------------------------------------+---------------------------------------------------+
   | Get area of the object                                                         | :cpp:any:`gui_obj_get_area`                       |
   +--------------------------------------------------------------------------------+---------------------------------------------------+
   | Point-in-Rectangle Range Check                                                 | :cpp:any:`gui_obj_point_in_obj_rect`              |
   +--------------------------------------------------------------------------------+---------------------------------------------------+
   | CRC check                                                                      | :cpp:any:`gui_obj_checksum`                       |
   +--------------------------------------------------------------------------------+---------------------------------------------------+
   | Get widget in tree by name                                                     | :cpp:any:`gui_obj_tree_get_widget_by_name`        |
   +--------------------------------------------------------------------------------+---------------------------------------------------+
   | Get widget in tree by type                                                     | :cpp:any:`gui_obj_tree_get_widget_by_type`        |
   +--------------------------------------------------------------------------------+---------------------------------------------------+
   | Update animate                                                                 | :cpp:any:`animate_frame_update`                   |
   +--------------------------------------------------------------------------------+---------------------------------------------------+
   | Print the tree in a breadth-first search manner                                | :cpp:any:`gui_obj_tree_print_bfs`                 |
   +--------------------------------------------------------------------------------+---------------------------------------------------+

API
---

.. doxygenfile:: gui_obj.h
