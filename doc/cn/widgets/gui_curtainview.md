# 幕布容器
<br>

幕布容器控件`(curtainvew)`，可容纳多个幕布控件，在创建的幕布控件上可以放置应用程序相关的信息，例如应用程序菜单或应用程序等信息。幕布容器控件`(curtainvew)`可以根据需要进行扩展，并可以根据需要进行自定义。如下图所示，幕布容器控件`(curtainvew)`由五个幕布组成，分别在上/下/右/左/中五个方向进行排列。

<br>
<div style="text-align: center"><img src="https://foruda.gitee.com/images/1700114998989746788/e5140991_10641540.png" /></div>
<br>

## 用法

### 创建幕布容器控件

使用 [gui_curtainview_create(parent, filename, x, y, w, h)](#gui_curtainview_create) 函数创建幕布容器控件，这是一个容器控件，可以容纳幕布控件。

### 回调函数

当幕布容器控件中的幕布全部关闭时，可以使用 [gui_curtainview_set_done_cb (this, cb)](#gui_curtainview_create) 此函数来实现某些回调事件。

## 示例

请参考以下示例代码:

```eval_rst
`curtain`_

.. _curtain: gui_curtain.html

```

<span id = "gui_curtainview_create">

## API

</span>

```eval_rst

.. doxygenfile:: gui_curtainview.h

```
