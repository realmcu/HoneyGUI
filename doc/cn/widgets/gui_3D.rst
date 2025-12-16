.. _3D 模型 (3D Model):

==================
3D 模型 (3D Model)
==================

Lite3D 是由 Realtek 自主研发的轻量级、跨平台的 3D 图形渲染库，支持软件渲染与硬件加速扩展，具有以下特点：

   - 轻量级：代码体积小，便于集成。
   - 跨平台：适配 HoneyGUI、LVGL 等 :term:`GUI` 框架，支持 FreeRTOS 和 Zephyr 环境运行。
   - 灵活渲染：提供软件渲染管线，支持扩展 :term:`GPU` 加速接口。


Lite3D 支持加载多种 3D 模型格式，以满足不同应用场景的需求：

   - 静态模型：支持加载由 :file:`.obj` 和 :file:`.mtl` 文件组成的静态 3D 模型，能够精确处理模型的几何形状和材质信息。
   - 骨骼动画模型：支持加载 :file:`.gltf` 格式，能够渲染包含骨骼动画的 3D 模型，为应用带来更生动、丰富的视觉表现力。

该引擎的工作流程如下：

.. figure:: https://foruda.gitee.com/images/1755252209828076518/c91dad94_13408154.png
   :width: 800px
   :align: center
   
   Lite3D 工作流程



3D 模型资源处理
---------------

OBJ 静态模型
~~~~~~~~~~~~~

OBJ 是一种广泛使用的静态模型文件格式，适用于无需复杂动画的场景，如产品展示、:term:`UI` 装饰元素等。

一个完整的 OBJ 模型通常由以下三个核心组件构成：

1. .obj 文件

   - 存储模型的几何数据，是模型的主体文件。内容包括：

     - 顶点坐标
     - 法线向量
     - 纹理坐标（UV 映射）
     - 面定义
   - 需引用 :file:`.mtl` 文件中的材质信息。

2. .mtl 文件（材质库）

  定义模型表面的光学属性。内容包括：

      - 环境光/漫反射/镜面反射颜色
      - 折射率
      - 透明度
      - 光照模型
      - 纹理贴图引用

3. 纹理图片

   通常为 :term:`PNG` 格式，用于赋予模型表面细节和真实感。常见的贴图类型有：

      - 漫反射贴图
      - 法线贴图
      - 高光贴图
      - 透明贴图

.. figure:: https://foruda.gitee.com/images/1735113754178839767/916a3f95_13408154.png
   :width: 800px
   :align: center
   
   OBJ 模型组成示例

GLTF 骨骼动画模型
~~~~~~~~~~~~~~~~~
GLTF (GL Transmission Format) 是一种专为高效传输和加载 3D 场景和模型而设计的开放标准。它被誉为 "3D 领域的 JPEG"，特别适合用于渲染包含骨骼动画的动态模型。

一个完整的 GLTF 模型通常由以下三个核心组件构成：

   - :file:`.gltf` 文件：JSON 格式的文件，以结构化的方式描述场景的层次关系与组成元素。
   - :file:`.bin` 文件：包含模型的二进制数据，如顶点坐标、法线向量、纹理坐标等。
   - 所有引用的纹理图片：如 PNG 格式的贴图。

其核心数据组件包括：

   - 场景图 (Scene Graph)：由节点 (Node) 组成的层级结构，定义了模型各部件的位置、旋转和缩放。
   - 网格 (Mesh)：模型的几何信息，包括顶点属性（坐标、法线、UV）和索引。
   - 材质 (Material)：基于物理的渲染 (PBR) 属性，如基础颜色、金属度、粗糙度等。
   - 皮肤 (Skin)：定义骨骼动画的关键部分，包含骨骼关节 (Joints) 的层级关系和反向绑定矩阵 (Inverse Bind Matrices)。
   - 动画 (Animation)：存储了节点（骨骼）随时间变化的位移、旋转、缩放等关键帧数据。
   - 纹理 (Texture)：由材质引用的图像数据。


模型预处理
~~~~~~~~~~~~~

在绘制 3D 模型前，需要将其转换为二进制格式。以下是处理流程：

1. **定位转换工具**

   在 HoneyGUI 安装目录下找到以下工具：

      - ``your_HoneyGUI_dir\tool\3D-tool\extract_desc.exe``
      - ``your_HoneyGUI_dir\tool\image-convert-tool\image_converter.py``

2. **准备模型文件**

   将模型文件复制到 ``tool\3D-tool`` 目录下。

   - 对于 OBJ 模型，请确保目录内包含：

      - :file:`.obj` 文件
      - :file:`.mtl` 文件
      - 所有引用的纹理图片

   - 对于 GLTF 模型，请确保目录内包含：

      - :file:`.gltf` 文件
      - :file:`.bin` 文件
      - 所有引用的纹理图片

3. **生成描述文件**

   在 ``tool\3D-tool`` 目录下打开命令行终端并运行：
   
   - 处理 OBJ 模型

      运行命令: :kbd:`extract_desc_v3.exe your_model.obj` 或者使用 Python 版本 :kbd:`python extract_desc_v3.py your_model.obj` 。
      
      工具会询问是否转换纹理：
      
      - 按回车或输入 ``Y`` - 自动将所有 PNG 纹理转换为 bin 格式
      - 输入 ``N`` - 跳过转换（已转换或无需纹理）

      .. figure:: https://foruda.gitee.com/images/1765767298921057824/1cafcec4_13408154.png
         :width: 800px
         :align: center

         脚本处理
      
      执行后会生成 :file:`desc_xxx.txt` 和 :file:`desc_xxx.bin` 文件，其中包含了解析后的 obj、mtl 数据以及内嵌的二进制格式纹理数据。
   
      .. figure:: https://foruda.gitee.com/images/1765768566599235890/a974e84c_13408154.png
         :width: 800px
         :align: center

         生成二进制文件

   - 处理 GLTF 模型

      运行命令: :kbd:`extract_desc.exe xxx.gltf` 或者 :kbd:`python extract_desc_v3.py your_model.obj`，该命令也会自动处理所有引用的纹理图片。

      执行后会生成 :file:`gltf_desc_xxx.txt` 和 :file:`gltf_desc_xxx.bin` 文件，其中包含了解析后的 gltf、bin 数据以及内嵌的纹理数据。


3D 模型生成
-------------

创建模型
~~~~~~~~

对于 OBJ 模型和 GLTF 模型都可使用函数 ``l3_create_model(void *desc_addr, L3_DRAW_TYPE draw_type, int16_t x, int16_t y, int16_t view_w, int16_t view_h)`` 创建。

**参数说明：**

- ``desc_addr``：指向脚本中提取的解析数据地址。
- ``draw_type``：模型绘制方式：

   - ``L3_DRAW_FRONT_ONLY``：仅绘制模型的正面，适用于需要隐藏背面的场景，如蝴蝶模型。
   - ``L3_DRAW_FRONT_AND_BACK``：绘制模型的正面和背面，适用于需要双面可见的场景，如棱镜模型。
   - ``L3_DRAW_FRONT_AND_SORT``：绘制模型的正面并排序，适用于前后景遮挡的场景，如人脸模型。

- ``x``， ``y``：模型视口在屏幕上的左上角坐标。
- ``view_w``， ``view_h``：模型视口的宽度和高度。

变换控制
~~~~~~~~~~~~

全局变换
^^^^^^^^
全局变换是对模型的所有顶点应用一个统一的矩阵变换，常用于实现模型的整体旋转、平移和缩放。

.. code-block:: c

   void l3_set_global_transform(l3_model_base_t *_this, l3_global_transform_cb cb);


用户需要实现一个回调函数，类型为 ``l3_global_transform_cb``，并在该函数内部定义变换逻辑，例如 ``world`` 世界坐标变换和 ``camera`` 相机视角投影。

**典型应用场景：**

+ 模型整体旋转/平移
+ 场景视角切换

相机变换
********
初始化函数为 ``l3_camera_UVN_initialize(l3_camera_t *camera, l3_4d_point_t cameraPosition, l3_4d_point_t cameraTarget, float near, float far, float fov, float viewPortWidth, float viewPortHeight)`` 。

+ ``camera``：指向相机结构体的指针，用于初始化相机的属性。

+ ``cameraPosition``：相机在世界坐标系中的位置。

+ ``cameraTarget``：相机所指向的目标点，即相机视线的焦点。

+ ``near``：近裁剪平面距离，定义了相机截取视锥体的近端平面到相机的距离，所有靠近相机而小于这个距离的物体将被裁剪掉。

+ ``far``：远裁剪平面距离，定义了视锥体远端平面到相机的距离，所有远离相机而大于这个距离的物体将被裁剪掉。

+ ``fov``：视野范围，通常以垂直角度（单位：度）表示，定义了相机的开阔程度，即相机视锥体的张开角度。

+ ``viewPortWidth``：视口的宽度，定义渲染目标或窗口的横向尺寸。

+ ``viewPortHeight``：视口的高度，定义渲染目标或窗口的纵向尺寸。


**作用：**

1. 相机变换定义了观察者在场景中的位置和方向，它将世界坐标系转换到摄像机坐标系。
2. 通过操作相机，可以实现不同的视角，例如平移摄像机位置和改变观察方向。

世界变换
********
初始化函数为 ``l3_world_initialize(l3_4x4_matrix_t *world, float x, float y, float z, float rotX, float rotY, float rotZ, float scale)`` 。

+ ``world``：指向世界变换矩阵的指针，将 3D 对象从模型坐标系转换到世界坐标系。

+ ``x``：沿 X 轴进行平移的距离，用以确定对象在世界坐标系中 X 方向上的位置。

+ ``y``：沿 Y 轴进行平移的距离，用以确定对象在世界坐标系中 Y 方向上的位置。

+ ``z``：沿 Z 轴进行平移的距离，用以确定对象在世界坐标系中 Z 方向上的位置。

+ ``rotX``：绕 X 轴旋转的角度（单位：度）。

+ ``rotY``：绕 Y 轴旋转的角度（单位：度）。

+ ``rotZ``：绕 Z 轴旋转的角度（单位：度）。

+ ``scale``：统一缩放系数，用于在各个方向上等比例地缩放对象。


**作用：**

1. 世界变换定义了模型在世界坐标系中的位置和方向，负责将模型坐标系转换到世界坐标系。
2. 通过世界变换矩阵可以实现模型整体的平移、旋转和缩放。


面变换
^^^^^^^^
支持对模型进行局部变换。

.. code-block:: c

   void l3_set_face_transform(l3_model_base_t *_this, l3_face_transform_cb cb);

其中 ``l3_face_transform_cb`` 类型的回调函数可以为物体的每个面设置不同的形状变换， ``face_index`` 为指定变换的面。

**功能特点：**

+ 支持按面片索引独立控制。
+ 使用 ``l3_calculator_4x4_matrix(l3_4x4_matrix_t *matrix, float tx, float ty, float tz, l3_4d_point_t point, l3_4d_vector_t vector, float degrees, float scale)`` 为每个面生成不同的矩阵以实现个性化的局部动画。


3D 模型绘制
------------

创建控件
~~~~~~~~~~
HoneyGUI 中已集成了 Lite3D 库，并封装成 ``gui_lite3d`` 控件，使用 :cpp:any:`gui_lite3d_create` 函数可以创建 3D 模型控件。

设置点击事件
~~~~~~~~~~~~~
:cpp:any:`gui_lite3d_on_click` 函数可以为 3D 模型控件设置点击事件，当用户点击模型时，会触发回调函数。

设置动画
~~~~~~~~~~
:cpp:any:`gui_obj_create_timer` 函数可以为 3D 模型控件设置动画属性，其中 ``callback`` 为动画更新的回调函数。


示例
----

.. _3D 蝴蝶:

3D 蝴蝶
~~~~~~~~

该模型由 8 个矩形面构成，每个面都有相应的纹理贴图，绘制方式为 ``L3_DRAW_FRONT_ONLY``。通过调用 ``l3_set_face_transform(l3_model_base_t *_this, l3_face_transform_cb cb)`` 函数，可以为不同的面设置局部变换，从而实现动画效果。

.. literalinclude:: ../../../example/application/screen_410_502/app_3d_butterfly.c
   :language: c
   :start-after: /* 3d butterfly demo start*/
   :end-before: /* 3d butterfly demo end*/

.. raw:: html

   <br>
   <div style="text-align: center"><img src="https://docs.realmcu.com/HoneyGUI/image/Lite3D/3d_butterfly.gif" width= "400" /></div>
   <br>

.. _3D 人脸:

3D 人脸
~~~~~~~~

该模型由 1454 个三角形面构成，使用默认材质基础色进行填充，绘制方式为 ``L3_DRAW_FRONT_AND_SORT``。

.. literalinclude:: ../../../example/application/screen_410_502/app_3d_face.c
   :language: c
   :start-after: /* 3d face demo start*/
   :end-before: /* 3d face demo end*/

.. raw:: html

   <br>
   <div style="text-align: center"><img src="https://docs.realmcu.com/HoneyGUI/image/Lite3D/3d_face.gif" width= "400" /></div>
   <br>


.. _3D 小狗:

3D 小狗
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


.. _3D 应用列表:

3D 应用列表
~~~~~~~~~~~~

该界面由 6 个 3D 应用图标组成，通过调用 ``l3_set_face_image(l3_model_base_t *_this, uint8_t face_index, void *image_addr)`` 函数可以为 3D 模型的指定面替换纹理贴图。

.. literalinclude:: ../../../example/application/screen_410_502/app_3d_applist.c
   :language: c
   :start-after: /* 3d applist demo start*/
   :end-before: /* 3d applist demo end*/

.. raw:: html

   <br>
   <div style="text-align: center"><img src="https://docs.realmcu.com/HoneyGUI/image/Lite3D/3d_applist.gif" width= "400" /></div>
   <br>


.. _3D 机器人:

3D 机器人
~~~~~~~~~~~~

该模型为 GLTF 格式，带骨骼动画，Lite3D 默认循环播放该动画。

.. literalinclude:: ../../../example/widget/3d/app_ui_realgui_3d_robot_gltf.c
   :language: c
   :start-after: /* 3d robot demo start*/
   :end-before: /* 3d robot demo end*/

.. raw:: html

   <br>
   <div style="text-align: center"><img src="https://docs.realmcu.com/HoneyGUI/image/Lite3D/3d_robot.gif" width= "400" /></div>
   <br>


帧率测试
--------

以下表格展示了不同芯片平台上，各个示例的帧率表现。编译环境采用 ``ARMCLANG V6.22``，编译选项为 ``-O2 LTO``。

.. csv-table:: 帧率测试结果
   :header: 芯片型号,处理器主频,分辨率,:ref:`3D 蝴蝶`,:ref:`3D 人脸`,:ref:`3D 小狗`,:ref:`3D 应用列表`,:ref:`3D 机器人`
   :align: center
   :name: 帧率测试结果

   RTL8773E,100MHz,410 x 502,38 FPS,13 FPS,22 FPS,28 FPS,7 FPS
   RTL8773G,200MHz,410 x 502,61 FPS,27 FPS,50 FPS,61 FPS,13 FPS


API
---

.. doxygenfile:: gui_lite3d.h
