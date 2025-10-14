.. _玻璃效果 (Glass):

===============
玻璃效果(Glass)
===============

玻璃控件用于在屏幕上模拟玻璃的折射效果，该控件的使用需要依赖完整的frame buffer。
在HoneyGUI中考虑到运算的时间成本，玻璃控件是一个简化的折射模型，并不遵循真实的光线折射规律。简化后的玻璃模型仅包含当前坐标与折射后坐标的差值关系，示意图如下：

.. figure:: https://foruda.gitee.com/images/1760423436149037567/32e6dd13_8391010.jpeg
   :width: 400px
   :align: center
   
   简化模型 

用法
----

.. list-table::
   :header-rows: 1

   * - 描述
     - API 
   * - 创建控件
     - :cpp:any:`gui_glass_effect_create_from_mem`  
   * - 设置属性
     - :cpp:any:`gui_glass_effect_set_attribute`
   * - 获得高度
     - :cpp:any:`gui_glass_effect_get_height` 
   * - 获得宽度
     - :cpp:any:`gui_glass_effect_get_width`
   * - 刷新属性
     - :cpp:any:`gui_glass_effect_refresh_size`
   * - 移动位置
     - :cpp:any:`gui_glass_effect_translate`

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
