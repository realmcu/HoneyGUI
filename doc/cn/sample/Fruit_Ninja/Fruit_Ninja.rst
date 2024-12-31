.. _水果忍者:

==============
水果忍者
==============

本示例演示了如何开发一个简单的"水果忍者"APP，您可以从中学习和了解开发UI应用程序的基本方法和流程。通过切割水果获得分数，直到切到炸弹游戏结束。观看下面的演示视频，了解其全部功能。

.. raw:: html

   <br>
   <div style="text-align: center"><img src="https://foruda.gitee.com/images/1723621216978994553/76b62adc_10737458.gif" width= "450" /></div>
   <br>

.. _环境需求:
环境需求
--------
请参考 :ref:`入门指南` 的安装部分内容。

.. _源文件:
源文件
-----
为了帮助您学习和熟悉开发，您可以在 ``realgui\example\screen_454_454`` 路径下找到您可能需要的所有源文件。本演示的源文件是 ``app_fruit_ninja_box2d.cpp``，您可以在上述路径中找到它，了解更多详情。

.. _配置:
配置
-----
.. code-block:: c

   
   #define SCREEN_WIDTH 454    // Set according to the screen width
   #define SCREEN_HEIGHT 454   // Set according to the screen height
   #define HEIGHT_OFFSET 100   // Set the screen height offset for refreshing fruit from the bottom of the screen

.. _调用步骤:
调用步骤
---------

步骤 1:  Declare the app ui design function
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
.. code-block:: c

   
   /** 
   * @brief Start Fruit Ninja APP by creating a window,
   *        setting the animation function of the window 
   *        and initializing some variables.
   * @param obj The parent widget where the APP's window is nested.
   */
   void fruit_ninja_design(gui_obj_t *obj)

   void app_fruit_ninja_design(gui_obj_t *obj)
   {
      app_fruit_ninja::fruit_ninja_design(obj);
   }

步骤 2:  Call function
~~~~~~~~~~~~~~~~~~~~~~~~~
.. code-block:: c

   extern void app_fruit_ninja_design(gui_obj_t *obj);
   app_fruit_ninja_design(GUI_APP_ROOT_SCREEN);

.. _设计思路:
设计思路
---------
| · 在该app中，使用到了box2d创建固体模拟在重力环境中的物体运动，在初始化时给定一个x轴与y轴的初速度等参数。
.. code-block:: c

   /* Add dynamic bodys */
   b2BodyDef ballBodyDef;
   ballBodyDef.type = b2_dynamicBody;
   ballBodyDef.position.Set(4, SCREEN_HEIGHT + HEIGHT_OFFSET * P2M);
   ballBodyDef.angularVelocity = -314;    //-PI rad/s
   ballBodyDef.linearVelocity.Set(10, -20); // Move up
   body_st = world.CreateBody(&ballBodyDef);

| · 因为相互碰撞不利于游戏的游玩，为了减小物体间相互碰撞的影响，将固体的半径设置为一个较小的值。
.. code-block:: c

   /* Creat body shape and attach the shape to the Body */
   b2CircleShape circleShape;
   circleShape.m_radius = 0.2; // Small radius reducing the impact of collisions
| · 在回调函数中利用固体的中心点映射更新水果（及炸弹）的位置与旋转角度并用图片组件显示。水果位置在显示界面外会对固体的位置与初速度进行复位。
.. code-block:: c

   /* Get the position of the ball then set the image location and rotate it on the GUI */
   b2Vec2 position = body_st->GetPosition();
   if (position_refresh((int)(position.x * M2P - RADIUS_ST), (int)(position.y * M2P - RADIUS_ST),
                        img_strawberry, body_st))
   {
      gui_img_set_attribute(img_strawberry, "img_strawberry", FRUIT_NINJA_STRAWBERRY_BIN,
                              img_strawberry->base.x, img_strawberry->base.y);
      fruit_cut_flag[0] = false;
      gui_img_set_location(img_cut_arry[0], 0, SCREEN_HEIGHT + HEIGHT_OFFSET);
   }

| · 切割水果使用了touch_info结构体，检测到触控点释放说明完成了一次切割（得到触屏初始点与x轴与y轴的位移），对切割内容进行判断。
.. code-block:: c

   /* Cutting judgment */
   GUI_TOUCHPAD_IMPORT_AS_TP // Get touchpoint
   if (tp->released)
   {
      bool bomb_flag = cutting_judgment(win, img_strawberry, img_banana, img_peach, img_watermelon,
                                          img_bomb, tp, img_cut_arry, fruit_cut_flag);
   }

| · 若切割线与图片矩形有两个交点，则说明切割有效。
.. code-block:: c

   line_has_two_intersections_with_rectangle(img_coordinate, img_w, img_h, tp_start, tp_end,
                                                               img_rotate_angle);

| · 注意在计算交点时图片的旋转后端点信息需要将旋转角度带入计算才会与显示一致，如此可以提高切割判断准确度。
.. code-block:: c

   /* Calculate the rectangle's four rotated points */
   Point rotated_rect_min = rotate_point(rect_min, center, angle); // Left-up
   Point rotated_rect_max = rotate_point(rect_max, center, angle); // Right-down
   Point rotated_rect_p2 = rotate_point(rect_p2, center, angle); // Left-down
   Point rotated_rect_p3 = rotate_point(rect_p3, center, angle); // Right-top

| · 将水果图片更新为切割后的两张图（对应两个gui_img_t指针），并计分，一次切割可以切到多个不同物体。
.. code-block:: c

   /* Refresh half-cut fruits position */
   if (fruit_cut_flag[0])
   {
      gui_img_set_location(img_cut_arry[0],  GUI_BASE(img_strawberry)->x + 10,
                              GUI_BASE(img_strawberry)->y + 10);
      gui_img_rotation(img_cut_arry[0], gui_img_get_transform_degrees(img_strawberry),
      gui_img_get_width(img_cut_arry[0]) / 2,
      gui_img_get_height(img_cut_arry[0]) / 2);
   }

| · 注意可以使用flag标记水果的切割状况，防止计分错误以及方便更新切割后的图片位置。
| · 当切割后的水果移动到显示界面之外会对固体的位置与初速度进行复位，并将切割效果复原。
.. code-block:: c

   gui_img_set_attribute(img_strawberry, "img_strawberry", FRUIT_NINJA_STRAWBERRY_BIN,
                                    img_strawberry->base.x, img_strawberry->base.y);
   fruit_cut_flag[0] = false;
   gui_img_set_location(img_cut_arry[0], 0, SCREEN_HEIGHT + HEIGHT_OFFSET);


