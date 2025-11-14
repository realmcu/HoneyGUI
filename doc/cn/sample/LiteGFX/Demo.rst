===========
LiteGFX
===========

趣戴介绍
-----------

趣戴科技是一家依托自研 LiteGfx 框架，充分发挥各类芯片性能，为客户提供跨平台、一站式 :term:`GUI` 解决方案及丰富绚丽特效产品的软件服务企业。通过自研 2.5D 特效框架，模拟实现 3D 技术并融合了粒子系统物理引擎技术，所有 2.5D 特效都内嵌于 LiteGfx Designer ，客户可轻松使用并进行个性化开发，为客户打造与众不同的视觉形象。趣戴科技将不断丰富公司在 2.5D 技术上的产品储备，助力客户在激烈的市场竞争中脱颖而出。其坚信优秀的视觉设计是提升企业品牌价值和市场竞争力的关键。

.. raw:: html

   <br>
   <div style="text-align: center"><img src="https://docs.realmcu.com/HoneyGUI/image/sample/LiteGFX/wave.gif" width= "400" /></div>
   <br>

   <br>
   <div style="text-align: center"><img src="https://docs.realmcu.com/HoneyGUI/image/sample/LiteGFX/polyhedron.gif" width= "400" /></div>
   <br>

   <br>
   <div style="text-align: center"><img src="https://docs.realmcu.com/HoneyGUI/image/sample/LiteGFX/shadow.gif" width= "400" /></div>
   <br>

   <br>
   <div style="text-align: center"><img src="https://docs.realmcu.com/HoneyGUI/image/sample/LiteGFX/cube.gif" width= "400" /></div>
   <br>

   <br>
   <div style="text-align: center"><img src="https://docs.realmcu.com/HoneyGUI/image/sample/LiteGFX/particle.gif" width= "400" /></div>
   <br>

   <br>
   <div style="text-align: center"><img src="https://docs.realmcu.com/HoneyGUI/image/sample/LiteGFX/fallen.gif" width= "400" /></div>
   <br>

   <br>
   <div style="text-align: center"><img src="https://docs.realmcu.com/HoneyGUI/image/sample/LiteGFX/dial.gif" width= "400" /></div>
   <br>

   <br>
   <div style="text-align: center"><img src="https://docs.realmcu.com/HoneyGUI/image/sample/LiteGFX/energy.gif" width= "400" /></div>
   <br>

   <br>
   <div style="text-align: center"><img src="https://docs.realmcu.com/HoneyGUI/image/sample/LiteGFX/slide.gif" width= "400" /></div>
   <br>

   <br>
   <div style="text-align: center"><img src="https://docs.realmcu.com/HoneyGUI/image/sample/LiteGFX/overturn.gif" width= "400" /></div>
   <br>

   <br>
   <div style="text-align: center"><img src="https://docs.realmcu.com/HoneyGUI/image/sample/LiteGFX/ball.gif" width= "400" /></div>
   <br>

   <br>
   <div style="text-align: center"><img src="https://docs.realmcu.com/HoneyGUI/image/sample/LiteGFX/number.gif" width= "400" /></div>
   <br>



源文件
----------

趣戴组件作为第三方库的形式集成到 HoneyGUI 中，并作为 RealGUI 引擎的一个控件使用，包含核心 lib ，控件适配，平台支持三个部分:

相关代码路径： ``HoneyGUI\realgui\3rd\litegfx``


::
    
    |-HoneyGUI-Adapt
    |      gui_widget_litegfx.c
    |      gui_widget_litegfx.h
    |      tab_app_energybox.c
    |      tab_app_notifications.c
    |      tab_app_prism.c
    |      tab_app_prism_refl.c
    |      tab_app_soccer.c
    |      tab_watchface_butterfly.c
    |      tab_watchface_digitclock.c
    |      tab_watchface_flowerfall.c
    |      tab_watchface_windmill.c
    |      
    |-platform
    |      lx_platform_log.c
    |      lx_platform_log.h
    |      lx_platform_memory.c
    |      lx_platform_memory.h
    |      lx_platform_new.cpp
    |      lx_platform_time.c
    |      lx_platform_time.h
    |      
    |-vglite
        |  liblx_vglite_gcc.a
        |  lx_vglite.lib
        |  
        |-include
            |-interface
                    lx_vglite_api.h


控件适配层
~~~~~~~~~~

此目录下的代码是趣戴为了适配 HoneyGUI 提供的 wrapper 层代码。其中 ``gui_widget_litegfx.c/h`` 为控件，通过调用 ``gui_widget_litegfx_create()`` 函数可以创建一个 LiteGFX 控件。注意，使用此方法本质仍然是通过 RealGUI 调用引擎显示相关效果，此控件支持 FrameBuffer 的分块方式。

项目中提供了多个示例代码，可直接参考使用：

- ``tab_app_energybox.c`` : 能量盒效果示例
- ``tab_app_notifications.c`` : 通知效果示例
- ``tab_app_prism.c`` : 棱镜示例
- ``tab_app_prism_refl.c`` : 棱镜倒影示例
- ``tab_app_soccer.c`` : 足球游戏示例
- ``tab_watchface_butterfly.c`` : 蝴蝶表盘示例
- ``tab_watchface_digitclock.c`` : 数字时钟示例
- ``tab_watchface_flowerfall.c`` : 花朵掉落示例
- ``tab_watchface_windmill.c`` : 风车示例


平台适配层
~~~~~~~~~~

平台适配层依据不同的平台重构了适配平台的一些接口，包括调试日志输出，包括内存管理，包括系统时间相关，包括重载一些 C++ 函数（如 new 等）。

核心 lib
~~~~~~~~~

提供 Windows GCC 版本的 ``liblx_vglite_gcc.a``， 提供嵌入式环境版本 ``lx_vglite.lib``， 请注意编译器版本。
