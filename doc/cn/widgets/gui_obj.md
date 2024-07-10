# 对象控件

<br>

对象控件实现了屏幕上控件的基本属性。屏幕控件是控件树的根节点。极坐标的原点在 Y 轴的负方向，极坐标的正方向为顺时针方向，屏幕坐标系设置如下所示。
<br>

<center><img src="https://foruda.gitee.com/images/1718762967150637512/872c46fa_13408154.png" /></center>

## 用法

|描述    |API       |
|:-------:|:-------:|
|构建控件    |[gui_obj_ctor(gui_obj_t *this, gui_obj_t *parent, const char *name, int16_t x, int16_t y, int16_t w, int16_t h)](#api)       |
|创建控件    |[gui_obj_create(void *parent, const char *name, int16_t x, int16_t y, int16_t w, int16_t h)](#api)       |
|添加事件    |[gui_obj_add_event_cb(void *obj, gui_event_cb_t event_cb, gui_event_t filter, void *user_data)](#api)       |
|设置事件    |[gui_obj_event_set(gui_obj_t *obj, gui_event_t event_code)](#api)      |
|释放树（从根节点到叶节点，递归地释放控件树)    |[gui_obj_tree_free(void *obj)](#api)       |
|打印树（从根节点到叶节点，递归地打印控件树）   |[gui_obj_tree_print(gui_obj_t *obj)](#api)       |
|获取树中某种类型小部件的数量   |[gui_obj_tree_count_by_type(gui_obj_t *obj, T_OBJ_TYPE type, int *count)](#api)       |
|隐藏/显示   |[gui_obj_tree_show(gui_obj_t *obj)](#api) 或 [gui_tree_not_show(gui_obj_t *obj, bool enable)](#api)       |
|能否显示    |[gui_obj_show(void *obj, bool show_info)](#api)       |
|获取根节点    |[gui_obj_t *gui_obj_tree_get_root(gui_obj_t *obj)](#api)       |
|获取子节点    |[gui_obj_get_child_handle(gui_obj_t *obj, T_OBJ_TYPE child_type)](#api)       |
|判断是否在矩形范围内    |[gui_obj_in_rect(gui_obj_t *obj, int16_t x,int16_t y,int16_t w,int16_t h)](#api)       |
|跳过父对象控件的所有操作（左/右/下/上拖动保持动作）    |[gui_obj_skip_all_parent_left_hold(gui_obj_t *obj)](#api),<br/>[gui_obj_skip_all_parent_right_hold(gui_obj_t *obj)](#api),<br/>[gui_obj_skip_all_parent_down_hold(gui_obj_t *obj)](#api),<br/>[gui_obj_skip_all_parent_up_hold(gui_obj_t *obj)](#api)       |
|跳过子对象控件的所有操作（左/右/下/上拖动保持动作）    |[gui_obj_skip_all_child_left_hold(gui_obj_t *obj)](#api),<br/>[gui_obj_skip_all_child_right_hold(gui_obj_t *obj)](#api),<br/>[gui_obj_skip_all_child_down_hold(gui_obj_t *obj)](#api),<br/>[gui_obj_skip_all_child_up_hold(gui_obj_t *obj)](#api)       |
|跳过其他对象控件的所有操作（左/右/下/上拖动保持动作）    |[gui_obj_skip_other_left_hold(gui_obj_t *obj)](#api),<br/>[gui_obj_skip_other_right_hold(gui_obj_t *obj)](#api),<br/>[gui_obj_skip_other_down_hold(gui_obj_t *obj)](#api),<br/>[gui_obj_skip_other_up_hold(gui_obj_t *obj)](#api)       |
|获取显示区域    |[gui_obj_get_area(gui_obj_t *obj, int16_t *x, int16_t *y, int16_t *w, int16_t *h)](#api)       |
|矩形范围内的点检测    |[gui_obj_point_in_obj_rect(gui_obj_t *obj, int16_t x, int16_t y)](#api)       |
|CRC校验    |[gui_obj_checksum(uint8_t seed, uint8_t *data, uint8_t len)](#api)       |
|通过名称在树中获取小部件    |[gui_obj_tree_get_widget_by_name(gui_obj_t *obj, const char *name, gui_obj_t **output)](#api)       |
|通过类型在树中获取小部件    |[gui_obj_tree_get_widget_by_type(gui_obj_t *root, T_OBJ_TYPE type, gui_obj_t **output)](#api)       |
|刷新动画效果    |[animate_frame_update(gui_animate_t *animate, gui_obj_t *obj)](#api)       |
|设定动画效果    |[gui_obj_set_animate(gui_animate_t *animate, uint32_t dur, int repeat_count, void *callback,void *p)](#api)       |
|以广度优先搜索的方式打印树    |[gui_obj_tree_print_bfs(gui_obj_t *root)](#api)       |

<br>

<span id="api">

## API

</span>

```eval_rst

.. doxygenfile:: gui_obj.h

```
