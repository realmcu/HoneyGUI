.. _水果忍者:

==============
水果忍者
==============

本示例演示了如何开发一个简单的“水果忍者” :term:`APP` ，您可以从中学习和了解开发 :term:`UI` 应用程序的基本方法和流程。在一分钟内通过切割水果获得分数，切到炸弹游戏直接结束。观看下面的演示视频，了解其全部功能。

.. raw:: html

   <br>
   <div style="text-align: center"><img src="https://foruda.gitee.com/images/1753348341025882496/50867451_10737458.gif" width= "450" /></div>
   <br>

.. _环境需求:

环境需求
--------
请参考 :ref:`入门指南` 的安装部分内容。

.. _源文件:

源文件
-------
为了帮助您学习和熟悉开发，您可以在 ``example\application\screen_410_502`` 路径下找到您可能需要的所有源文件。本演示的源文件是 ``app_box2d_fruit_ninja.cpp``，您可以在上述路径中找到它，了解更多详情。

.. _配置:

配置
-----

.. literalinclude:: ../../../../example/application/screen_410_502/app_box2d_fruit_ninja.cpp
   :language: c
   :dedent:
   :start-after: /* fruit ninja screen config start */
   :end-before: /* fruit ninja screen config end */

.. _调用步骤:

调用步骤
---------

步骤 1:  Declare Ui Design Function
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

.. literalinclude:: ../../../../example/application/screen_410_502/app_box2d_fruit_ninja.cpp
   :language: c
   :dedent:
   :start-after: /* fruit ninja design entry start */
   :end-before: /* fruit ninja design entry end */

步骤 2:  Call Function
~~~~~~~~~~~~~~~~~~~~~~~~~

.. literalinclude:: ../../../../example/application/screen_410_502/app_box2d_fruit_ninja.cpp
   :language: c
   :dedent:
   :start-after: /* fruit ninja view instance start */
   :end-before: /* fruit ninja view instance end */

.. _设计思路:

设计思路
---------
+ 在该 APP 中，使用到了 box2d 创建固体模拟在重力环境中的物体运动，在初始化时给定一个 x 轴与 y 轴的初速度等参数。

   .. literalinclude:: ../../../../example/application/screen_410_502/app_box2d_fruit_ninja.cpp
      :language: c
      :dedent:
      :start-after: /* fruit ninja dynamic body start */
      :end-before: /* fruit ninja dynamic body end */

+ 因为相互碰撞不利于游戏的游玩，为了减小物体间相互碰撞的影响，将固体的半径设置为一个较小的值。

   .. literalinclude:: ../../../../example/application/screen_410_502/app_box2d_fruit_ninja.cpp
      :language: c
      :dedent:
      :start-after: /* fruit ninja body shape start */
      :end-before: /* fruit ninja body shape end */

+ 在回调函数中利用固体的中心点映射更新水果（及炸弹）的位置与旋转角度并用图片组件显示。水果位置在显示界面外会对固体的位置与初速度进行复位。

   .. literalinclude:: ../../../../example/application/screen_410_502/app_box2d_fruit_ninja.cpp
      :language: c
      :dedent:
      :start-after: /* fruit ninja get position start */
      :end-before: /* fruit ninja get position end */

+ 切割水果使用了 touch_info 结构体，检测到触控点释放说明完成了一次切割（得到触屏初始点与 x 轴与 y 轴的位移），对切割内容进行判断。

   .. literalinclude:: ../../../../example/application/screen_410_502/app_box2d_fruit_ninja.cpp
      :language: c
      :dedent:
      :start-after: /* fruit ninja cut judgment start */
      :end-before: /* fruit ninja cut judgment end */

+ 若切割线与图片矩形有两个交点，则说明切割有效。

   .. literalinclude:: ../../../../example/application/screen_410_502/app_box2d_fruit_ninja.cpp
      :language: c
      :dedent:
      :start-after: /* fruit ninja cut check start */
      :end-before: /* fruit ninja cut check end */

+ 注意在计算交点时图片的旋转后端点信息需要将旋转角度带入计算才会与显示一致，如此可以提高切割判断准确度。

   .. literalinclude:: ../../../../example/application/screen_410_502/app_box2d_fruit_ninja.cpp
      :language: c
      :dedent:
      :start-after: /* fruit ninja rotate points start */
      :end-before: /* fruit ninja rotate points end */

+ 将水果图片更新为切割后的两张图（对应两个 gui_img_t 指针），并计分，一次切割可以切到多个不同物体。

   .. literalinclude:: ../../../../example/application/screen_410_502/app_box2d_fruit_ninja.cpp
      :language: c
      :dedent:
      :start-after: /* fruit ninja refresh half cut start */
      :end-before: /* fruit ninja refresh half cut end */

+ 注意可以使用 flag 标记水果的切割状况，防止计分错误以及方便更新切割后的图片位置。

+ 当切割后的水果移动到显示界面之外会对固体的位置与初速度进行复位，并将切割效果复原。

   .. literalinclude:: ../../../../example/application/screen_410_502/app_box2d_fruit_ninja.cpp
      :language: c
      :dedent:
      :start-after: /* fruit ninja restore cut start */
      :end-before: /* fruit ninja restore cut end */
