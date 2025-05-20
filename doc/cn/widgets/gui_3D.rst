=================
3D模型 (3D Model)
=================

该控件支持加载由 :file:`.obj` 和 :file:`.mtl` 文件组成的3D模型，能够处理模型的几何形状和材质信息，并支持为模型添加丰富的动画特效以增强视觉表现力。

该控件支持两种基础几何面类型的3D模型绘制：矩形面和三角形面。矩形面适用于简单的几何体，如立方体、平面等，而三角形面则适用于更复杂的模型，如人脸、动物等。用户可以根据需要选择合适的面类型来创建3D模型。


3D模型组成要素
---------------

完整的3D模型包含三个核心组件：

1. **.obj文件**

   - 存储几何数据，包括：

     - 顶点坐标
     - 法线向量
     - 纹理坐标（UV映射）
     - 面定义
   - 需引用 :file:`.mtl` 文件中的材质信息。

2. **.mtl文件（材质库）**

   - 定义表面属性，包括：

     - 环境光/漫反射/镜面反射颜色
     - 折射率
     - 透明度
     - 光照模型
     - 纹理贴图引用

3. **纹理图片**

   - 通常为PNG格式，用于：

     - 漫反射贴图
     - 法线贴图
     - 高光贴图
     - 透明贴图

.. figure:: https://foruda.gitee.com/images/1735113754178839767/916a3f95_13408154.png
   :width: 800px
   :align: center
   
   3D模型组成示例

3D模型处理流程
---------------

在HoneyGUI中使用3D模型前，需要将其转换为二进制格式。以下是处理流程：

1. **定位转换工具**
   
   - 在HoneyGUI安装目录下找到以下工具：

     - ``your_HoneyGUI_dir/tool/3D-tool/png2c.py``
     - ``your_HoneyGUI_dir/tool/3D-tool/extract_desc.exe``

2. **准备模型目录**
   
   - 将上述工具复制到模型目录。

   - 该模型目录确保包含：

     - :file:`.obj` 文件
     - :file:`.mtl` 文件
     - 所有引用的纹理图片

3. **生成描述文件**
   
   - 使用提取器处理模型: ``extract_desc.exe xxx.obj``，该可执行文件会自动调用 :file:`png2c.py` 将所有的PNG纹理转换为二进制数组。
   
   .. figure:: https://foruda.gitee.com/images/1735540370568112173/cf1c0126_13408154.png
      :width: 800px
      :align: center

      脚本处理
   
   - 生成的 :file:`desc.txt` 文件包含以下内容：

     - obj解析数据
     - mtl解析数据
     - 内嵌纹理数据
   
   .. figure:: https://foruda.gitee.com/images/1735114445910760790/2a41eeab_13408154.png
      :width: 800px
      :align: center

      生成二进制数组

.. note::
   三角形面组成的模型暂不支持纹理贴图。


在HoneyGUI中的集成
-------------------

1. **添加描述文件到项目**

   - 将生成的 :file:`desc.txt` 文件放入项目目录。

2. **创建3D控件**

   - 使用 :cpp:any:`gui_3d_create` 函数创建：

   .. code-block:: c

      gui_3d_t *test_3d = gui_3d_create(gui_obj_get_root(), "3d-widget", (void *)_acdesc, 0, 0, 480, 480);


用法
-------------

创建控件
~~~~~~~~
使用 :cpp:any:`gui_3d_create` 创建3D模型，导入的 ``desc_addr`` 文件即为脚本中提取的解析数据。

变换控制
~~~~~~~~

全局变换
^^^^^^^^
使用 :cpp:any:`gui_3d_set_global_transform_cb` 对3D模型进行整体变换，其中 ``gui_3d_global_transform_cb`` 类型的回调函数可以为物体的所有面设置相同的形状变换。例如 ``world`` 世界坐标变换和 ``camera`` 相机视角投影，矩形面还支持设置 ``light`` 光照信息。具体配置可参考 :ref:`坐标变换和光照系统` 章节。

典型应用场景：

+ 模型整体旋转/平移
+ 场景视角切换

面变换
^^^^^^^^
使用 :cpp:any:`gui_3d_set_face_transform_cb` 对3D模型进行局部变换，其中 ``gui_3d_global_transform_cb`` 类型的回调函数可以为物体的每个面设置不同的形状变换， ``face_index`` 为指定变换的面。

功能特点：

+ 支持局部动画
+ 支持按面片索引独立控制

.. _坐标变换和光照系统:

坐标变换和光照系统
~~~~~~~~~~~~~~~~~~

世界变换
^^^^^^^^

初始化函数为 ``gui_3d_world_inititalize(gui_3d_matrix_t *world, float x, float y, float z, float rotX, float rotY, float rotZ, float scale)`` 。

+ ``world``：指向世界变换矩阵的指针，将3D对象从模型坐标系转换到世界坐标系。

+ ``x``：沿X轴进行平移的距离，用以确定对象在世界坐标系中X方向上的位置。

+ ``y``：沿Y轴进行平移的距离，用以确定对象在世界坐标系中Y方向上的位置。

+ ``z``：沿Z轴进行平移的距离，用以确定对象在世界坐标系中Z方向上的位置。

+ ``rotX``：绕X轴旋转的角度（单位：度）。

+ ``rotY``：绕Y轴旋转的角度（单位：度）。

+ ``rotZ``：绕Z轴旋转的角度（单位：度）。

+ ``scale``：统一缩放系数，用于在各个方向上等比例地缩放对象。


作用：

1. 世界变换矩阵通常负责将模型坐标系转换到世界坐标系。例如，如果有一个物体位于模型坐标系的原点，通过世界变换，它可以被放置到场景中的任意位置并进行旋转缩放。
2. 对每个面进行独立的世界变换可以实现局部动画或静态展示。
3. 不同的面可以共享同一个世界矩阵，也可以使用 ``gui_3d_calculator_matrix(gui_3d_matrix_t *matrix, float x, float y, float z, gui_point_4d_t point, gui_vector_4d_t vector, float degrees, float scale)`` 为每个面生成不同的矩阵以实现个性化的局部变换。


相机变换
^^^^^^^^

初始化函数为 ``gui_3d_camera_UVN_initialize(gui_3d_camera_t *camera, gui_point_4d_t cameraPosition, gui_point_4d_t cameraTarget, float near, float far, float fov, float viewPortWidth, float viewPortHeight)`` 。

+ ``camera``：指向相机结构体的指针，用于初始化相机的属性。

+ ``cameraPosition``：相机在世界坐标系中的位置。

+ ``cameraTarget``：相机所指向的目标点，即相机视线的焦点。

+ ``near``：近裁剪平面距离，定义了相机截取视锥体的近端平面到相机的距离，所有靠近相机而小于这个距离的物体将被裁剪掉。

+ ``far``：远裁剪平面距离，定义了视锥体远端平面到相机的距离，所有远离相机而大于这个距离的物体将被裁剪掉。

+ ``fov``：视野范围，通常以垂直角度（单位：度）表示，定义了相机的开阔程度，即相机视锥体的张开角度。

+ ``viewPortWidth``：视口的宽度，定义渲染目标或窗口的横向尺寸。

+ ``viewPortHeight``：视口的高度，定义渲染目标或窗口的纵向尺寸。


作用：

1. 相机变换定义了观察者在场景中的位置和方向，它将世界坐标系转换到摄像机坐标系。
2. 通过操作相机，可以实现不同的视角，例如平移摄像机位置和改变观察方向。


光照信息
^^^^^^^^

初始化函数为 ``gui_3d_light_inititalize(gui_3d_light_t *light, gui_point_4d_t lightPosition, gui_point_4d_t lightTarget, float included_angle, float blend_ratio, gui_3d_RGBAcolor_t color)`` 。

+ ``light``：指向光源结构体的指针，用于初始化光源的属性。

+ ``lightPosition``：光源在世界坐标系中的位置。

+ ``lightTarget``：光源的目标位置，定义光源照射的方向。

+ ``included_angle``：光的锥形角度（单位：度），即图中的 :math:`\alpha` 角，它决定聚光灯的光照范围，即图中聚光灯的外圈范围。

+ ``blend_ratio``：光照融合区比例，定义聚光边缘的柔和度，范围为0~1，决定图中的 :math:`\beta` 角，其值通过以下公式计算得出：

   .. math::
   
      β = α (1 - ratio)

   融合区域即为下图中的聚光灯内圈到外圈的范围，在内圈内光照强度一致，内圈到外圈光照强度逐渐衰减。

+ ``color``：光源的颜色，格式为RGBA8888。

.. figure:: https://foruda.gitee.com/images/1735889400996762341/a4f7e0c8_13408154.png
   :width: 400px
   :align: center

   聚光灯效果示例


作用：

1. 光源类型为聚光灯，其属性包含初始位置、光源朝向、锥形角度、融合区比例和光源颜色。
2. 对每个面或对象局部调整光照可以营造不同的视觉风格。


设置动画
~~~~~~~~~~
:cpp:any:`gui_obj_create_timer` 函数可以为3D对象设置动画属性，其中 ``callback`` 为动画更新的回调函数。


示例
----

.. _3D蝴蝶:

3D蝴蝶
~~~~~~~~

在 :file:`menu_config.h` 中开启宏定义 ``CONFIG_REALTEK_BUILD_REAL_BUTTERFLY_3D`` 来运行此示例。该模型由 8 个矩形面构成，每个面都有相应的纹理贴图。通过调用 :cpp:any:`gui_3d_set_face_transform_cb()` 函数，可以为不同的面设置局部变换，从而实现动画效果。

.. literalinclude:: ../../../example/widget/3d/app_ui_realgui_3d_butterfly.c
   :language: c
   :start-after: /* 3d butterfly demo start*/
   :end-before: /* 3d butterfly demo end*/

.. raw:: html

   <br>
   <div style="text-align: center"><img src="https://docs.realmcu.com/HoneyGUI/image/widgets/butterfly.gif" width= "400" /></div>
   <br>

.. _3D人脸:

3D人脸
~~~~~~~~

在 :file:`menu_config.h` 中开启宏定义 ``CONFIG_REALTEK_BUILD_REAL_FACE_3D`` 来运行此示例。该模型由 1454 个三角形面构成，使用默认材质基础色进行填充。

.. literalinclude:: ../../../example/widget/3d/app_ui_realgui_3d_face.c
   :language: c
   :start-after: /* 3d face demo start*/
   :end-before: /* 3d face demo end*/

.. raw:: html

   <br>
   <div style="text-align: center"><img src="https://docs.realmcu.com/HoneyGUI/image/widgets/face.gif" width= "400" /></div>
   <br>


.. _3D小狗:

3D小狗
~~~~~~~~

在 :file:`menu_config.h` 中开启宏定义 ``CONFIG_REALTEK_BUILD_REAL_DOG_3D`` 来运行此示例。该模型由 774 个三角形面组成，支持自定义材质基础色的填充。

.. literalinclude:: ../../../example/widget/3d/app_ui_realgui_3d_dog.c
   :language: c
   :start-after: /* 3d dog demo start*/
   :end-before: /* 3d dog demo end*/

.. raw:: html

   <br>
   <div style="text-align: center"><img src="https://docs.realmcu.com/HoneyGUI/image/widgets/dog.gif" width= "400" /></div>
   <br>


.. _3D圆盘:

3D圆盘
~~~~~~~~

在 :file:`menu_config.h` 中开启宏定义 ``CONFIG_REALTEK_BUILD_REAL_DISC_3D`` 来运行此示例。该模型由 133 个矩形面构成，其中圆盘部分使用纹理贴图，而矩形部分采用颜色填充。

.. literalinclude:: ../../../example/widget/3d/app_ui_realgui_3d_disc.c
   :language: c
   :start-after: /* 3d disc demo start*/
   :end-before: /* 3d disc demo end*/

.. raw:: html

   <br>
   <div style="text-align: center"><img src="https://docs.realmcu.com/HoneyGUI/image/widgets/disc.gif" width= "400" /></div>
   <br>


帧率测试
--------

以下表格展示了不同芯片平台上，各个示例的帧率表现。编译环境采用 ``ARMCLANG V6.22``，编译选项为 ``-O3 LTO``。

.. csv-table:: 帧率测试结果
   :header: 芯片型号,处理器主频,分辨率,:ref:`3D蝴蝶`,:ref:`3D人脸`,:ref:`3D小狗`,:ref:`3D圆盘`
   :align: center
   :name: 帧率测试结果

   RTL8773E,100MHz,410 x 502,40 FPS,13 FPS,20 FPS,10 FPS


API
---

.. doxygenfile:: gui_3d.h