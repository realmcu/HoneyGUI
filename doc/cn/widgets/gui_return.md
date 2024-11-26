# 返回 (Return)

可以从屏幕左侧拖出的泡泡形状曲线用于执行返回操作。

## 用法

### 创建返回控件

创建函数是 [gui_return_create(void *parent, const uint32_t *frame_array[], int array_size, void *return_cb, gui_obj_t *ignore_gesture_widget)](#gui_return_create)。用户需要提供曲线动画帧图像文件数组，以及返回操作函数。

## 示例

```c
const uint32_t *gui_app_return_array[] =
{
    PATH04_BIN,
    PATH05_BIN,
    PATH07_BIN,
    PATH08_BIN,
    PATH09_BIN,
    PATH11_BIN,
    PATH12_BIN,
    PATH14_BIN,
    PATH15_BIN,
    PATH16_BIN,
    PATH18_BIN,
    PATH19_BIN,
    PATH20_BIN,
    PATH22_BIN,
    PATH23_BIN,
    PATH24_BIN,
    PATH25_BIN,
};
gui_return_create(GUI_APP_ROOT_SCREEN, gui_app_return_array,
                      sizeof(gui_app_return_array) / sizeof(uint32_t *), win_cb, (void *)cell);
```
<br>
<div style="text-align: center"><img src="https://docs.realmcu.com/HoneyGUI/image/widgets/return.gif"  /></div>
<br>


```eval_rst

.. note::
  详细信息请参阅 ```realgui\example\screen_454_454\gui_menu\apps.c```。

```



<span id = "gui_return_create">

## API

</span>

```eval_rst

.. doxygenfile:: gui_return.h

```
