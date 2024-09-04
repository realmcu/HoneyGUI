# 卡片容器控件

卡片效果容器控件，可以嵌套卡片，可以通过滑动来切换卡片。

<br>
<center><img width="500" src= "https://foruda.gitee.com/images/1700123394899348792/914aadf9_10641540.png "></center>
<br>

## 用法

### 创建控件
开发者可以调用 [gui_cardview_create(parent,  name, x, y, w, h)](#gui_cardview_create) 来创建卡片容器控件。

### 回调
当卡片容器的状态改变时，它会触发一个回调，开发者可以在回调函数[gui_cardview_status_cb(this, cb)](#gui_cardview_status_cb)来做想做的事情。

### 设置风格
卡片容器控件有5种风格，开发者可以使用 [gui_cardview_set_style(this, style)](#gui_cardview_set_style)来设置风格。

```eval_rst
.. literalinclude:: ../../../gui_engine/widget/gui_tabview.h
   :language: c
   :start-after: /* T_SLIDE_STYLE enum start*/
   :end-before: /* T_SLIDE_STYLE enum end*/
   
```

### 设置中心对齐
开发者可以调用函数[gui_cardview_alignment(this, align_hight)](#gui_cardview_alignment)来设置卡片自动中心对齐。

### 设置底部间隔
开发者可以调用函数[gui_cardview_set_bottom_space(this, bottom_space)](#gui_cardview_set_bottom_space)来设置卡片的底部间隔。

### y轴方向滑动回调
卡片容器在y轴方向滑动的事件可以添加相应的回调函数[gui_cardview_up(this, callback, parameter)](#gui_cardview_up)、[gui_cardview_down(this, callback, parameter)](#gui_cardview_down)。

## 示例
请参考这一章节的实现:
```eval_rst
`card`_    

.. _card: gui_card.html

```

## API

```eval_rst

.. doxygenfile:: gui_cardview.h

```
