=================
3D模型 (3D Model)
=================

该控件支持加载由obj和mtl文件组成的3D模型，并支持添加动画特效。

GUI加载3D模型
-------------
1. 3D模型组成部分

   + OBJ文件：存储3D模型的几何数据，包括顶点、法线、纹理坐标、面等。
   + MTL文件：描述3D模型材质的属性，包括颜色、光泽度、透明度和纹理映射等。
   + 图片文件：模型中使用到的贴图。

   .. figure:: https://foruda.gitee.com/images/1735113754178839767/916a3f95_13408154.png
      :width: 800px
      :align: center

      3D模型组成示例

2. 3D模型解析并生成3D信息描述子

   + 调用脚本处理obj文件。

   .. figure:: https://foruda.gitee.com/images/1735540370568112173/cf1c0126_13408154.png
      :width: 800px
      :align: center

      脚本处理
   
   + 生成图片和3D信息描述子的二进制数组。

   .. figure:: https://foruda.gitee.com/images/1735114445910760790/2a41eeab_13408154.png
      :width: 800px
      :align: center

      生成二进制数组

3. GUI加载描述子

   将包含obj解析数据、mtl解析数据和图片数据的desc文件放入工程目录下，并在 :cpp:any:`gui_3d_create` 中加载。


3D控件用法
-------------
创建控件
~~~~~~~~
使用 :cpp:any:`gui_3d_create` 创建3D模型，导入的 ``desc_addr`` 文件即为脚本中提取的解析数据。

形状变换
~~~~~~~~~~
使用 :cpp:any:`gui_3d_set_shape_transform_cb` 对3D模型进行变换，其中 ``cb`` 可以为物体的每个面设置不同的形状变换。该函数中的 ``world``、 ``camera`` 和 ``light`` 分别代表了3D对象的全局变换、相机视角投影和光照信息。

世界变换
^^^^^^^^

初始化函数为 ``gui_3d_world_inititalize(gui_3d_matrix_t *world, float x, float y, float z, float rotX, float rotY, float rotZ, float scale)`` 。

+ ``world``：指向世界变换矩阵的指针，将3D对象从模型坐标系转换到世界坐标系；

+ ``x``：沿X轴进行平移的距离，用以确定对象在世界坐标系中X方向上的位置；

+ ``y``：沿Y轴进行平移的距离，用以确定对象在世界坐标系中Y方向上的位置；

+ ``z``：沿Z轴进行平移的距离，用以确定对象在世界坐标系中Z方向上的位置；

+ ``rotX``：绕X轴旋转的角度（单位：度）；

+ ``rotY``：绕Y轴旋转的角度（单位：度）；

+ ``rotZ``：绕Z轴旋转的角度（单位：度）；

+ ``scale``：统一缩放系数，用于在各个方向上等比例地缩放对象；


作用：

1. 世界变换矩阵通常负责将模型坐标系转换到世界坐标系。例如，如果有一个物体位于模型坐标系的原点，通过世界变换，它可以被放置到场景中的任意位置并进行旋转缩放。
2. 对每个面进行独立的世界变换可以实现局部动画或静态展示。
3. 不同的面可以共享同一个世界矩阵，也可以使用 ``gui_3d_calculator_matrix(gui_3d_matrix_t *matrix, float x, float y, float z, gui_point_4d_t point, gui_vector_4d_t vector, float degrees, float scale)`` 为每个面生成不同的矩阵以实现个性化的局部变换。


相机变换
^^^^^^^^

初始化函数为 ``gui_3d_camera_UVN_initialize(gui_3d_camera_t *camera, gui_point_4d_t cameraPosition, gui_point_4d_t cameraTarget, float near, float far, float fov, float viewPortWidth, float viewPortHeight)`` 。

+ ``camera``：指向相机结构体的指针，用于初始化相机的属性；

+ ``cameraPosition``：相机在世界坐标系中的位置；

+ ``cameraTarget``：相机所指向的目标点，即相机视线的焦点；

+ ``near``：近裁剪平面距离，定义了相机截取视锥体的近端平面到相机的距离，所有靠近相机而小于这个距离的物体将被裁剪掉；

+ ``far``：远裁剪平面距离，定义了视锥体远端平面到相机的距离，所有远离相机而大于这个距离的物体将被裁剪掉；

+ ``fov``：视野范围，通常以垂直角度（单位：度）表示，定义了相机的开阔程度，即相机视锥体的张开角度；

+ ``viewPortWidth``：视口的宽度，定义渲染目标或窗口的横向尺寸；

+ ``viewPortHeight``：视口的高度，定义渲染目标或窗口的纵向尺寸；


作用：

1. 相机变换定义了观察者在场景中的位置和方向，它将世界坐标系转换到摄像机坐标系。
2. 通过操作相机，可以实现不同的视角，例如平移摄像机位置和改变观察方向。


光照信息
^^^^^^^^

初始化函数为 ``gui_3d_light_inititalize(gui_3d_light_t *light, gui_point_4d_t lightPosition, gui_point_4d_t lightTarget, float included_angle, float blend_ratio, gui_3d_RGBAcolor_t color)`` 。

+ ``light``：指向光源结构体的指针，用于初始化光源的属性；

+ ``lightPosition``：光源在世界坐标系中的位置；

+ ``lightTarget``：光源的目标位置，定义光源照射的方向；

+ ``included_angle``：光的锥形角度（单位：度），即图中的 :math:`\alpha` 角，它决定聚光灯的光照范围，即图中聚光灯的外圈范围；

+ ``blend_ratio``：光照融合区比例，定义聚光边缘的柔和度，范围为0~1，决定图中的 :math:`\beta` 角，其值通过以下公式计算得出：

   .. math::
   
      β = α (1 - ratio)

   融合区域即为下图中的聚光灯内圈到外圈的范围，在内圈内光照强度一致，内圈到外圈光照强度逐渐衰减；

+ ``color``：光源的颜色，格式为RGBA8888；

.. figure:: https://foruda.gitee.com/images/1735889400996762341/a4f7e0c8_13408154.png
   :width: 400px
   :align: center

   聚光灯效果示例


作用：

1. 光源类型为聚光灯，其属性包含初始位置、光源朝向、锥形角度、融合区比例和光源颜色。
2. 对每个面或对象局部调整光照可以营造不同的视觉风格。


设置动画
~~~~~~~~~~
:cpp:any:`gui_3d_set_animate` 函数用于为3D对象设置动画属性，其中 ``callback`` 为动画更新的回调函数，当动画每一帧更新时将调用这个函数。


示例
----
3D蝴蝶
~~~~~~~~

.. literalinclude:: ../../../realgui/example/demo/app_ui_realgui_3d.c
   :language: c
   :start-after: /* 3d butterfly demo start*/
   :end-before: /* 3d butterfly demo end*/

.. raw:: html

   <br>
   <div style="text-align: center"><img src="https://foruda.gitee.com/images/1734070660330786955/61e4ff9d_13408154.gif" width= "400" /></div>
   <br>


3D棱镜
~~~~~~~~
.. code-block:: c

   #include "math.h"
   #include "cube3D/desc.txt"

   static float rot_angle = 0.0f;
   void update_cube_animation()
   {
      rot_angle++;
   }

   static void cube_cb(gui_3d_t *this, size_t face/*face offset*/, gui_3d_world_t *world,
                  gui_3d_camera_t *camera, gui_3d_light_t *light)
   {
      gui_dispdev_t *dc = gui_get_dc();
      gui_3d_matrix_t face_matrix;
      gui_3d_matrix_t object_matrix;

      gui_3d_camera_UVN_initialize(camera, gui_point_4d(0, 6, 15), gui_point_4d(0, 0, 0), 1, 32767, 90,
                                    dc->screen_width, dc->screen_height);

      gui_3d_world_inititalize(&object_matrix, 0, 22, 40, 90, 0, 0,
                              10);

      gui_3d_light_inititalize(light, gui_point_4d(0, 22, 45), gui_point_4d(0, 22, 40), 60, 0.6, (gui_3d_RGBAcolor_t){255, 215, 0, 255});

      gui_3d_calculator_matrix(&face_matrix, 0, 0, 0, gui_3d_point(0, 0, 0), gui_3d_vector(0, 0, 1), rot_angle,
                                    1);
      
      *world = gui_3d_matrix_multiply(face_matrix, object_matrix);

   }
   static void app_ui_design(gui_app_t *app)
   {

      gui_3d_t *test_3d = gui_3d_create(&(app->screen), "3d-widget", (void *)_acdesc, 0, 0, 480, 480);

      gui_3d_set_shape_transform_cb(test_3d, 0, cube_cb);

      gui_3d_set_animate(test_3d, 10000, -1, update_cube_animation, NULL);

      return;

   }

.. raw:: html

   <br>
   <div style="text-align: center"><img src="https://docs.realmcu.com/HoneyGUI/image/widgets/cube3d.gif" width= "400" /></div>
   <br>

API
---

.. doxygenfile:: gui_3d.h