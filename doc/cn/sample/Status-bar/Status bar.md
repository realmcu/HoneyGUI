#  状态栏

这是一种新样式的状态栏。在非下拉状态时，仅在屏幕顶部用小字体显示实时时间。点击顶部可以下拉状态栏，随着下拉，遮罩颜色逐渐变为不透明，时间文字变大。下拉到一定程度后，状态栏完全展开，显示日期和消息通知。

<br>
<div style="text-align: center"><img src="https://docs.realmcu.com/HoneyGUI/image/sample/Status-bar/status_bar.gif"  /></div>
<br>

##  实现
###  文件
 函数 ```static void status_bar(void *parent, gui_obj_t *ignore_gesture)``` 位于 ```gui_engine\example\screen_454_454\gui_menu\apps.c``` 的第255行
###  设计

```eval_rst
.. mermaid::

    graph TB
        A[Parent] --> B[win]
        B --> D[Rect hidden]
        B --> E[Text Time]
        B --> H[win hidden]
        H --> F[Text Data]
        H --> G[Text NOTIFS]
```
* 在此状态栏中，以窗口控件为根节点。使用矩形绘制函数绘制状态栏的白色半透明背景。嵌套了三个文本框，分别表示时间、日期和通知消息。其中，时间文本框使用函数缓存成图像，因为时间显示需要缩放。矩形背景和日期、通知消息的文本初始时被隐藏。触摸屏交互效果在根节点窗口控件的动画回调函数中实现。
* 在窗口的动画回调函数中，首先将时间和日期文本框的内容更新为实时的时间和日期，格式分别为"07:55"和"Tue, Apr 16"。然后，读取触摸板数据，根据当前触屏信息如手势判断状态栏的显示效果，例如是否隐藏背景、是否隐藏日期和通知、更改背景透明度、时间文本框的缩放比例等。
* 'status_bar'函数有一个参数'ignore_gesture'，它接收一个指向控件的指针。此参数用于解决控件上的手势与状态栏之间的冲突。遇到这种冲突时，通过这段代码 ```if (ignore_gesture) { ignore_gesture->gesture = 1; }``` 来停用相应控件的手势交互。这里，将'gesture'属性设置为'1'关闭了该控件的手势响应。




