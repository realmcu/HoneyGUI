==========================================
棱镜模型 (Prism Mirror 3D Model)
==========================================

该控件支持加载由OBJ和MTL文件组成的3D棱柱模型，并支持添加动画特效，可以用在app界面显示。使用之前请阅读： :ref:`棱镜控件使用注意事项` 。

.. raw:: html

   <br>
   <div style="text-align: center;">
       <img src="https://docs.realmcu.com/HoneyGUI/image/widgets/prism_mirror.gif" width="400" alt="Image" />
   </div>
   <br>

模拟器运行棱镜效果
------------------

1. 在 :file:`menu_config.h` 中（ :file:`HoneyGUI\\win32_sim\\menu_config.h` ），开启宏定义CONFIG_REALTEK_BUILD_REAL_PRISM_MIRROR_3D；
2. 编译运行模拟器，可以出现文档开头的视频效果；

.. _GUI加载棱镜模型:

GUI加载棱镜模型
---------------
1. 棱镜模型组成需要的所以文件添加到单独的文件夹内：包含6张PNG图片，3D模型的几何信息文件（ :file:`.obj` ）和对应的材质信息文件（ :file:`.mtl` ）

   + OBJ文件：存储棱镜模型的几何数据，包括顶点、法线、纹理坐标、面等。
   + MTL文件：描述棱镜模型材质的属性，包括颜色、光泽度、透明度和纹理映射等。
   + 图片文件：模型中使用到的贴图。
   + 棱镜信息描述子打包运行exe文件（ :file:`extract_desc.exe` 和 :file:`png2c.py` ）。


2. 棱镜模型解析并生成棱镜信息描述子，调用脚本( :file:`extract_desc.exe` )处理 :file:`.obj` 文件

   + 在棱镜资源包目录中，通过命令行运行以下命令：:kbd:`extract_desc.exe prism.obj`。请根据实际情况将 :file:`prism.obj` 修改为用户自定义的文件名。
   
   + 生成图片和棱镜信息描述子的二进制数组: :file:`desc.txt`

3. GUI加载描述子

   将包含obj解析数据、mtl解析数据和图片数据的 :file:`desc.txt` 文件放入工程目录下，并在 :cpp:any:`gui_prism_mirror3d_create` 中加载。


棱镜控件用法
-------------
创建控件
~~~~~~~~
使用 :cpp:any:`gui_prism_mirror3d_create` 函数来创建棱镜模型。此函数需要提供父对象、名称、描述数据、以及位置和大小参数。通过可选的配置结构（`prism_mirror3d_config_t`）可以指定棱镜的面数、自动旋转和输入灵敏度。参数 `desc_addr` 文件包含从脚本中提取的解析数据

**参数:**

- `parent`: 父对象，新的棱镜模型将附加到此 GUI 组件中。
- `name`: 作名称，用于标识和管理 3D 对象。
- `desc_addr`: 描述符地址，包含可视化的解析数据。
- `x`: 在父组件坐标系中的 X 坐标。
- `y`: 在父组件坐标系中的 Y 坐标。
- `w`: 控件的宽度。
- `h`: 控件的高度。
- `config`: 指向配置结构体的指针，用于配置面数、旋转等特性。

**示例:**

.. code-block:: c

    gui_obj_t *parent = &(app->screen);
    const char *name = "prism_demo";
    uint16_t x = 100, y = 100, w = 300, h = 300;
    prism_mirror3d_config_t config = {6, true, 0.05f};

    gui_prism_mirror3d_t *prism_demo = gui_prism_mirror3d_create(parent, name, desc_addr, x, y, w, h, &config);

添加动态效果
~~~~~~~~~~~~~
使用 :cpp:any:`gui_prism_mirror3d_enter_animate` 为棱镜模型添加动态效果，比如自动旋转和交互式旋转。此函数接收创建的 ``prism_mirror3d`` 作为参数，默认情况下模型会绕 y 轴自动旋转。

**参数:**

- `prism_mirror3d`:已经创建的棱镜模型对象。

**示例:**

.. code-block:: c

    if (prism_demo) {
        gui_prism_mirror3d_enter_animate(prism_demo);
    }

添加app切换效果
~~~~~~~~~~~~~~~
使用 :cpp:any:`gui_prism_mirror3d_click_switch_app_add_event` 为棱镜模型添加点击事件响应效果，实现应用切换。参数 ``callback`` 为对应的回调函数。

**参数:**

- `prism_mirror3d`:已经创建的棱镜模型对象，待添加点击事件。

- `callback`:回调函数，用于处理点击事件后的应用切换逻辑。

**示例:**

.. code-block:: c

    void onSwitchAppCallback(gui_event_t e) {
        // 处理应用切换逻辑
    }

    gui_prism_mirror3d_click_switch_app_add_event(prism, onSwitchAppCallback);

设置大小
~~~~~~~~
使用 :cpp:any:`gui_prism_mirror3d_set_scale` 来设定棱镜模型的大小。调整比例因子以适合场景需求。

**参数:**

- `prism_mirror3d`: 棱镜模型对象。
- `scale`: 缩放因子。

**示例:**

.. code-block:: c

    gui_prism_mirror3d_set_scale(prism, 1.0f);

设置位置
~~~~~~~~
使用 :cpp:any:`gui_prism_mirror3d_set_position` 来设置棱镜模型的位置。此函数需要 x, y, z 坐标来定义模型在 3D 空间中的位置。

**参数:**

- `prism_mirror3d`: 棱镜模型对象。
- `x`: X 坐标。
- `y`: Y 坐标。
- `z`: Z 坐标。

**示例:**

.. code-block:: c

    gui_prism_mirror3d_set_position(prism, 0, 50, 0);


设置方位
~~~~~~~~
使用 :cpp:any:`gui_prism_mirror3d_set_rotation_angles` 来设置棱镜模型的方位。此功能帮助调节模型在 3D 环境中的方向。

**参数:**

- `prism_mirror3d`: 棱镜模型对象。
- `x`: 围绕 X 轴的旋转角度。
- `y`: 围绕 Y 轴的旋转角度。
- `z`: 围绕 Z 轴的旋转角度。

**示例:**

.. code-block:: cpp

    gui_prism_mirror3d_set_rotation_angles(prism, 0, 60, 0);

设置原始状态
~~~~~~~~~~~~~~

使用 :cpp:any:`gui_prism_mirror3d_set_raw_state` 函数来设置 3D 棱镜模型的原始状态。此函数设定棱镜在3D世界中的初始位置、相机位置、旋转角度和缩放比例。通常在控件初始化后立即调用，以便定义初始显示状态。

**参数:**

- `prism_mirror3d`: 要进行配置的棱镜模型对象。
- `world_position`: 长度为 3 的浮点数组，指定棱镜在世界坐标系中的 x, y, z 坐标。
- `camera_position`: 长度为 3 的浮点数组，指定相机相对于棱镜位置的 x, y, z 坐标。
- `rot_x`: 围绕 X 轴的旋转角度（以度为单位）。
- `rot_y`: 围绕 Y 轴的旋转角度（以度为单位）。
- `rot_z`: 围绕 Z 轴的旋转角度（以度为单位）。
- `scale`: 棱镜的缩放比例。

**示例:**

.. code-block:: c

    float raw_world_position[3] = {0, 25, 180};
    float raw_camera_position[3] = {0, 5, 60};
    gui_prism_mirror3d_set_raw_state(prism_demo, raw_world_position, raw_camera_position, 0, 0, 0, 13);

设置目标状态
~~~~~~~~~~~~

使用 :cpp:any:`gui_prism_mirror3d_set_target_state` 函数来定义 3D 棱镜模型在动画或交互中将要达到的目标状态。此功能特别适用于制作平滑的过渡效果，比如从一个位置变换到另一个位置。

**参数:**

- `prism_mirror3d`: 要调整的棱镜模型对象。
- `world_position`: 目标世界坐标系中的 x, y, z 坐标（浮点数组）。
- `camera_position`: 相机的目标位置坐标数组，指定相对于棱镜的 x, y, z。
- `rot_x`: 目标围绕 X 轴的旋转角度。
- `rot_y`: 目标围绕 Y 轴的旋转角度。
- `rot_z`: 目标围绕 Z 轴的旋转角度。
- `scale`: 棱镜的目标缩放比例。

**示例:**

.. code-block:: c

    float target_world_position[3] = {0, 35, 162};
    float target_camera_position[3] = {0, 0, 80};
    gui_prism_mirror3d_set_target_state(prism_demo, target_world_position, target_camera_position, 0, 0, 0, 14);

这两个函数使得棱镜模型能够在初始化时拥有适当的视图和状态，并确保动画或交互期间达到给定的目标状态，为用户提供流畅的视觉体验。

棱镜控件配置
-------------

为了自定义3D棱镜控件的行为，以下配置已直接在代码中进行调整。本节描述了禁用自动旋转和设定触摸灵敏度的有效参数。

自动旋转与灵敏度设置
~~~~~~~~~~~~~~~~~~~~~

棱镜控件通过特定的参数来控制其基本属性。以下是应用的设置概述：

- **面数**：定义棱镜的可见面数量。此设置为 `6`，表示棱镜为六边形结构。

- **自动旋转**：默认情况下，棱镜控件会自动旋转。在此配置中，自动旋转功能是启用的。若需禁用该功能，应在应用代码中通过额外的逻辑来实现，因为此默认设置保持功能激活。

- **触摸灵敏度**：该参数控制棱镜对触摸输入的响应。灵敏度设置为 `0.05f`，指定了中等水平的响应性，允许用户进行流畅的交互。

**配置代码设置：**

.. code-block:: c

    prism_demo->conf.auto_rotation = true;
    prism_demo->conf.face_nums = 6;
    prism_demo->conf.sensitivity = 0.05f;

配置解释
~~~~~~~~

- **面数量** (`face_nums`)：调整棱镜的几何复杂性，目前支持6棱柱效果。

- **自动旋转** (`auto_rotation`)：启用或禁用棱镜自动旋转的功能。值为 `true` 表示功能处于激活状态。

- **灵敏度** (`sensitivity`)：控制棱镜响应触摸手势的程度。将此值默认设置为 `0.05f` 提供了良好的响应性，适合绝大多数用户交互场景的流畅操作。

.. _棱镜控件使用注意事项:

棱镜控件使用注意事项
----------------------

1. 若使用目前默认的三维建模效果，默认demo效果为480x480的方形显示屏设计：

   1. 所有图片资源必须为PNG格式，
   2. 更换demo六棱柱效果图，替换图片命名请改为“image1、image2、......、image6”。
   3. 请从以下路径获取棱镜信息描述子所需的转换文件：

      + 文件路径： :file:`HoneyGUI\\realgui\\example\\demo\\3d` ，所需文件： :file:`extract_desc.exe` 和 :file:`png2c.py`。
      + 文件路径： :file:`HoneyGUI\\realgui\\example\\demo\\3d` ，所需文件： :file:`extract_desc.exe` 和 :file:`png2c.py`。


2. 如果使用其他比例的显示屏，为获得更佳的视觉效果，需要重新使用三维软件建模，并导出相应的OBJ文件（参考 :ref:`棱镜控件修改3D模型` ）制作GUI可以加载的描述文件（具体步骤请参考 :ref:`GUI加载棱镜模型` ）。
3. 三维建模导出的OBJ文件需要配置Y轴为前进轴。
4. 棱镜默认创建后会自动旋转，关闭这个功能，请在 :file:`gui_prism_mirror.h` 将AUTO_ROTATION参数置0；
5. 修改棱镜控件的跟手灵敏度，请在 :file:`gui_prism_mirror.h` 调整SENSITIVITY；

.. _棱镜控件修改3D模型:

棱镜控件修改3D模型
------------------
本例程使用blender软件作为演示。

1. 使用三维建模软件打开 :file:`prism.obj` 文件，切换视图效果，快捷键：英文模式按下键盘左上角处 :kbd:`·`，出现视图选择按下 :kbd:`Bottom`。

.. figure:: https://foruda.gitee.com/images/1736508668989561574/99180bb7_13406851.jpeg
   :align: center
   :width: 800px

   调整模型视图

2. 物体模式下，选中所有物体进行缩放，（以410*502屏幕为例，长宽比例1:1.224，），拉升物体，左下角出现缩放自定义，修改y值为1.224。

.. figure:: https://foruda.gitee.com/images/1736508578410723528/ba011e3a_13406851.jpeg
   :align: center
   :width: 800px

   缩放模型比例

3. 导出文件

.. figure:: https://foruda.gitee.com/images/1736732505811582125/f24394ef_13406851.jpeg
   :align: center
   :width: 800px

   导出模型obj文件

示例
----
棱镜
~~~~~~~~

.. literalinclude:: ../../../realgui/example/demo/app_ui_realgui_3d_prism_mirror.c
   :language: c
   :start-after: /* 3d prism mirror demo start*/
   :end-before: /* 3d prism mirror demo end*/

API
---

.. doxygenfile:: gui_prism_mirror3d.h