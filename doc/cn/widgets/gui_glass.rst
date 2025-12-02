.. _玻璃效果 (Glass):

==================
玻璃效果 (Glass)
==================

玻璃控件用于在屏幕上模拟玻璃的折射效果，该控件的使用需要依赖完整的 frame buffer。
在 HoneyGUI 中考虑到运算的时间成本，玻璃控件是一个简化的折射模型，并不遵循真实的光线折射规律。简化后的玻璃模型仅包含当前坐标与折射后坐标的差值关系，示意图如下：

.. figure:: https://foruda.gitee.com/images/1760423436149037567/32e6dd13_8391010.jpeg
   :width: 400px
   :align: center
   
   简化模型 

玻璃数据生成
------------

基于以上简化模型可以得到规则：折射后的像素点仅与玻璃遮盖范围内像素点的偏移量有关。因此只需要生成玻璃上每个点偏移量的集合即可。在 :ref:`示例 <glass_example_cn>` 的设计中，偏移量由玻璃厚度 t （折射率）与观测点位置（x_o, y_o）通过以下关系决定：

.. math::

   offset(x, y) = distortion(x, y) * d(x, y)

其中 distortion 为玻璃上各个点的厚度（折射率），g(x, y)为当前玻璃上的点与观测点（x_o, y_o）的关系。

玻璃效果图像生成器使用说明
~~~~~~~~~~~~~~~~~~~~~~~~~~

软件概述
^^^^^^^^

本软件是一个基于Python的玻璃效果图像生成器，支持创建多种形状的玻璃效果图像，当前支持图形包括圆形、圆角矩形和环形。

.. figure:: https://foruda.gitee.com/images/1764645364845730374/c41935cd_8391010.png
   :width: 400px
   :align: center
   
   软件界面 

功能实现
^^^^^^^^

形状选择
********

- **圆形**：创建圆形玻璃效果
- **圆角矩形**：创建带圆角的矩形玻璃效果
- **环形**：创建环形玻璃效果

参数调节
********

- **半径/尺寸**：控制形状的大小
- **扭曲度**：调整玻璃效果的扭曲强度（0-1）
- **区域**：控制效果区域范围（0-1）

自定义保存路径
**************

- **路径输入**：指定数据保存路径
- **确定输出**：点击后生成最终数据
- **重设**：将设定恢复为默认值

图像操作
********

- **实时预览**：参数调整时实时更新图像
- **图像保存**：支持保存生成的玻璃效果数据，并用于HoneyGUI绘制图形
- **运动效果预览**：通过数据拖动玻璃图形预览动态效果

使用方法
^^^^^^^^

基本操作
********

1. 选择所需的形状类型
2. 调整相应参数滑块
3. 点击"浏览"指定数据保存路径
4. 点击确定，预览显示效果，并支持拖动功能

拖动功能
********

- 所有形状都支持鼠标拖动
- 拖动时形状会跟随鼠标移动
- 自动边界检查防止移出可视区域

注意事项
^^^^^^^^

- 拖动功能需要先生成图像
- 保存数据前请确认保存路径，若未指定路径，则默认保存在工具根目录下

用法
----

.. list-table::
   :header-rows: 1

   * - 描述
     - :term:`API` 
   * - 创建控件
     - :cpp:any:`gui_glass_create_from_mem`  
   * - 设置属性
     - :cpp:any:`gui_glass_set_attribute`
   * - 获得高度
     - :cpp:any:`gui_glass_get_height` 
   * - 获得宽度
     - :cpp:any:`gui_glass_get_width`
   * - 刷新属性
     - :cpp:any:`gui_glass_refresh_size`
   * - 移动位置
     - :cpp:any:`gui_glass_translate`

.. _glass_example_cn:

示例
-------

.. literalinclude:: ../../../example/widget/glass_effect/example_glass_effect.c
    :language: c
    :start-after: /* gui glass widget example start*/
    :end-before: /* gui glass widget example end*/

    

.. raw:: html

   <br>
   <div style="text-align: center"><img src="https://foruda.gitee.com/images/1760426386384113377/a1a16aae_8391010.gif" width= "400" /></div>
   <br>


API
---

.. doxygenfile:: gui_glass.h
