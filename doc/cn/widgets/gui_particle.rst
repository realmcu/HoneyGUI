.. _粒子特效 (Particle):

==================
粒子特效 (Particle)
==================

粒子特效控件用于在屏幕上呈现各种动态视觉效果，如烟花、雪花、气泡、闪电等。系统内置了丰富的预设特效，开发者可以直接选用，快速为应用添加生动的动画表现。

功能概述
--------

粒子特效控件基于高性能粒子引擎构建，提供完整的 2D 粒子系统能力，适用于嵌入式设备上的动态视觉表现。

发射形状
~~~~~~~~

支持多种几何发射源，灵活控制粒子的初始分布：

.. list-table::
   :header-rows: 1
   :widths: 20 80

   * - 形状
     - 说明
   * - Point（点）
     - 从单一坐标点发射，适用于爆发、喷射类效果
   * - Line（线段）
     - 沿线段均匀发射，适用于雨、雪等边缘发射场景
   * - Circle（圆形）
     - 在圆形区域内随机发射
   * - Rect（矩形）
     - 在矩形区域内随机发射，适用于全屏分布效果
   * - Ring（环形）
     - 在内外半径之间的环形区域发射，适用于漩涡、星系等效果

运动轨迹
~~~~~~~~

支持三种粒子运动模型：

.. list-table::
   :header-rows: 1
   :widths: 20 80

   * - 轨迹类型
     - 说明
   * - Linear（线性）
     - 匀速直线运动，可配置阻尼系数
   * - Gravity（重力）
     - 模拟物理重力场，支持重力加速度、风力和阻尼
   * - Orbit（轨道）
     - 围绕指定中心点做轨道运动

颜色系统
~~~~~~~~

.. list-table::
   :header-rows: 1
   :widths: 20 80

   * - 颜色模式
     - 说明
   * - Solid（纯色）
     - 固定单一颜色
   * - Random（随机）
     - 从最多 8 色调色板中随机选取
   * - Gradient（渐变）
     - 在生命周期内从起始色平滑过渡到终止色
   * - Rainbow（彩虹）
     - 在生命周期内自动循环色相

交互行为
~~~~~~~~

粒子系统内置多种触摸交互模式：

.. list-table::
   :header-rows: 1
   :widths: 25 75

   * - 行为模式
     - 说明
   * - Follow Touch（跟随触摸）
     - 发射器实时跟随触摸位置
   * - Trail（拖尾）
     - 沿触摸移动方向的反方向发射粒子，形成拖尾轨迹
   * - Touch Feedback（触摸反馈）
     - 按下爆发、拖拽持续发射、释放最终爆发
   * - Pulse（脉冲）
     - 发射速率按频率周期性变化
   * - Breathe（呼吸）
     - 粒子透明度按频率周期性明灭

渲染特性
~~~~~~~~

- 三种混合模式：Normal（标准 Alpha 混合）、Additive（叠加发光）、Multiply（正片叠底）
- 四种缓动函数：Linear、Ease In、Ease Out、Ease In Out，控制透明度和缩放的变化曲线
- 粒子支持独立的缩放、旋转、透明度动画，可沿速度方向自动对齐
- 边界行为：Kill（销毁）、Reflect（反弹）、Wrap（环绕）

扩展能力
~~~~~~~~

- 自定义回调：支持粒子初始化、逐帧更新、渲染前处理、消亡事件等 6 种回调钩子，实现任意自定义行为
- 多发射器叠加：单个控件支持最多 16 个独立特效同时运行
- 配置序列化：特效配置支持二进制格式导出，可从内存或文件系统加载，便于资源管理和动态切换

用法
----

运行粒子特效示例
~~~~~~~~~~~~~~~~

通过 ``menuconfig`` 启用粒子特效示例，在配置界面中依次完成以下操作并保存：

1. 进入 ``Enable HoneyGUI Framework`` → ``HoneyGUI Feature Configuration`` ，启用 ``Enable Particle System``
2. 返回上级，进入 ``Select HoneyGUI Demo`` ，选择 ``Particle System Demo`` （该选项依赖上一步，需先启用粒子系统才会出现）

.. note::

   ``Select HoneyGUI Demo`` 是单选菜单，选择 ``Particle System Demo`` 会自动取消之前选中的其他 Demo。

启动后会进入特效选择菜单（Launcher），点击列表中的特效名称即可预览对应效果，点击顶部 ``Back`` 按钮返回菜单。

使用预设特效
~~~~~~~~~~~~

每个预设特效都提供了独立的配置函数和演示入口，使用方式如下：

.. code-block:: c

   #include "effect_snow.h"

   /* 方式一：使用默认配置直接初始化 */
   gui_particle_widget_t *widget = effect_snow_demo_init();

   /* 方式二：获取配置后自定义调整 */
   particle_effect_config_t config;
   effect_snow_config(&config);

   /* 根据需要调整参数，例如修改发射速率 */
   config.emission.rate = 30.0f;

   /* 创建粒子控件并加载配置 */
   gui_obj_t *root = gui_obj_get_root();
   gui_particle_widget_t *widget = gui_particle_widget_create(
       root, "my_snow", 0, 0, screen_w, screen_h, 256);
   gui_particle_widget_add_effect(widget, &config);

所有预设特效的头文件和源码位于 ``example/widget/particle/`` 目录，文件命名格式为 ``effect_<name>.c/h`` 。


预设特效
------------

粒子系统提供了以下预设特效，可直接使用。每个特效都经过精心调校，适用于不同的应用场景。

.. list-table::
   :header-rows: 1
   :widths: 18 35 47

   * - 特效名称
     - 描述
     - 效果预览
   * - Galaxy（螺旋星系）
     - 螺旋臂结构的旋转星系，粒子沿螺旋臂分布并向中心汇聚，外圈蓝紫色渐变至内圈白粉色。
     - .. raw:: html

          <div style="text-align: center"><video width="300" controls loop><source src="https://docs.realmcu.com/HoneyGUI/image/widgets/particle/galaxy.mp4" type="video/mp4"></video></div>

   * - Firework（烟花）
     - 定时在随机位置触发爆发，粒子向四周散射并受重力下坠，多色随机绽放。
     - .. raw:: html

          <div style="text-align: center"><video width="300" controls loop><source src="https://docs.realmcu.com/HoneyGUI/image/widgets/particle/firework.mp4" type="video/mp4"></video></div>

   * - Trail（拖尾）
     - 跟随触摸位置持续发射粒子，形成拖拽尾迹效果。按下时发射，释放时停止。
     - .. raw:: html

          <div style="text-align: center"><video width="300" controls loop><source src="https://docs.realmcu.com/HoneyGUI/image/widgets/particle/trail.mp4" type="video/mp4"></video></div>

   * - Touch（触摸反馈）
     - 触摸按下时在触点爆发粒子，拖拽时持续发射，释放时最终爆发。适合用作触摸交互反馈。
     - .. raw:: html

          <div style="text-align: center"><video width="300" controls loop><source src="https://docs.realmcu.com/HoneyGUI/image/widgets/particle/touch.mp4" type="video/mp4"></video></div>

   * - Snow（雪花）
     - 雪花从屏幕顶部缓慢飘落，带有轻微风力偏移和随机旋转，营造冬日氛围。
     - .. raw:: html

          <div style="text-align: center"><video width="300" controls loop><source src="https://docs.realmcu.com/HoneyGUI/image/widgets/particle/snow.mp4" type="video/mp4"></video></div>

   * - Bubble（气泡）
     - 气泡从屏幕底部缓慢上升，碰到边界会反弹，逐渐膨胀。
     - .. raw:: html

          <div style="text-align: center"><video width="300" controls loop><source src="https://docs.realmcu.com/HoneyGUI/image/widgets/particle/bubble.mp4" type="video/mp4"></video></div>

   * - Custom（自定义回调）
     - 展示自定义回调的用法，粒子带有正弦波横向扰动和彩虹色循环。可作为自定义特效的参考模板。
     - .. raw:: html

          <div style="text-align: center"><video width="300" controls loop><source src="https://docs.realmcu.com/HoneyGUI/image/widgets/particle/custom.mp4" type="video/mp4"></video></div>

   * - Tunnel（穿梭隧道）
     - 粒子从屏幕中心高速向外辐射，模拟超空间跃迁效果，由小变大产生透视感。
     - .. raw:: html

          <div style="text-align: center"><video width="300" controls loop><source src="https://docs.realmcu.com/HoneyGUI/image/widgets/particle/tunnel.mp4" type="video/mp4"></video></div>

   * - Rocket（火箭尾焰）
     - 双层粒子系统：明亮的火焰层和灰色的烟雾层，模拟火箭推进器排气。
     - .. raw:: html

          <div style="text-align: center"><video width="300" controls loop><source src="https://docs.realmcu.com/HoneyGUI/image/widgets/particle/rocket.mp4" type="video/mp4"></video></div>

   * - Vortex（能量漩涡）
     - 粒子从外圈螺旋旋转运动，彩虹色渐变，越接近中心旋转越快、尺寸越小。
     - .. raw:: html

          <div style="text-align: center"><video width="300" controls loop><source src="https://docs.realmcu.com/HoneyGUI/image/widgets/particle/vortex.mp4" type="video/mp4"></video></div>

   * - Lightning（闪电）
     - 程序化生成锯齿折线路径，模拟真实闪电放电效果，带有递归分支和闪烁淡出。
     - .. raw:: html

          <div style="text-align: center"><video width="300" controls loop><source src="https://docs.realmcu.com/HoneyGUI/image/widgets/particle/lightning.mp4" type="video/mp4"></video></div>

   * - Fireflies（萤火虫）
     - 全屏随机分布的发光点，缓慢漂移并呈呼吸式明灭，营造夏夜氛围。
     - .. raw:: html

          <div style="text-align: center"><video width="300" controls loop><source src="https://docs.realmcu.com/HoneyGUI/image/widgets/particle/fireflies.mp4" type="video/mp4"></video></div>

   * - Ripple（涟漪）
     - 触摸触发同心圆环向外扩展，模拟水面涟漪效果，支持次级波纹。
     - .. raw:: html

          <div style="text-align: center"><video width="300" controls loop><source src="https://docs.realmcu.com/HoneyGUI/image/widgets/particle/ripple.mp4" type="video/mp4"></video></div>

   * - Rain（雨滴）
     - 雨滴从顶部斜向下落，落地时生成溅射粒子，带有风力偏移。
     - .. raw:: html

          <div style="text-align: center"><video width="300" controls loop><source src="https://docs.realmcu.com/HoneyGUI/image/widgets/particle/rain.mp4" type="video/mp4"></video></div>

   * - MagicCircle（魔法阵）
     - 两张魔法阵图片反向旋转，周围持续喷射橘红色光点向上漂浮，营造魔法效果。
     - .. raw:: html

          <div style="text-align: center"><video width="300" controls loop><source src="https://docs.realmcu.com/HoneyGUI/image/widgets/particle/magic_circle.mp4" type="video/mp4"></video></div>

   * - LightBeam（反射激光）
     - 三束彩色激光在屏幕内反射运动，碰壁时产生闪光，颜色叠加自然混色。
     - .. raw:: html

          <div style="text-align: center"><video width="300" controls loop><source src="https://docs.realmcu.com/HoneyGUI/image/widgets/particle/light_beam.mp4" type="video/mp4"></video></div>

.. note::

   预设特效库持续扩充中。建议定期同步代码仓库并运行 Particle Demo，以获取最新特效更新。

API
---

.. doxygenfile:: gui_particle_widget.h
