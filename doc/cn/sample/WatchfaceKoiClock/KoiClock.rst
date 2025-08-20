表盘界面——锦鲤时钟
=================

锦鲤时钟是一款专为智能手表设计的动态表盘应用，将传统时间显示与生动的 3D 锦鲤动画完美结合，为用户带来独特的视觉体验和互动乐趣。


.. raw:: html

   <br>
   <div style="text-align: center"><img src="https://docs.realmcu.com/HoneyGUI/image/sample/koiclock/koiPank.gif" width= "400" /></div>
   <br>


核心功能
--------

动态锦鲤展示
~~~~~~~~~~~~~
- **3 条独立游动的锦鲤**：每条锦鲤拥有独特的游动轨迹和速度
- **逼真的游动动画**：鱼身自然摆动，模拟真实鱼类游动姿态
- **点击互动**：点击锦鲤可触发加速游动效果

时间显示
~~~~~~~~
- **清晰的时间展示**：采用大字体显示当前时间
- **优雅的时钟设计**：与锦鲤场景完美融合的时钟样式

视觉特效
~~~~~~~~
- **水波纹效果**：触摸屏幕时产生扩散的水波纹
- **光影效果**：锦鲤阴影随游动位置变化

实现
--------------

代码
~~~~
函数 ``static void app_ui_koiclock_design(gui_view_t *view)`` 位于文件 ``example/application/screen_410_502/app_3d_koiClock.c``。

控件树设计
~~~~~~~~~~~~~~~~~~~
.. figure:: https://foruda.gitee.com/images/1755677527679678751/6a1df64f_13406851.png
   :width: 700px
   :align: center
   :name: 控件树设计
   
   控件树设计

