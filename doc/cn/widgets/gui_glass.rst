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

用法
----

.. list-table::
   :header-rows: 1

   * - 描述
     - API 
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
