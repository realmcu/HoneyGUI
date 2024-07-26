#  LiteGFX应用程序

## 趣戴介绍

趣戴科技是一家依托自研LiteGfx框架，充分发挥各类芯片性能，为客户提供跨平台、一站式GUI解决方案及丰富绚丽特效产品的软件服务企业。通过自研2.5D特效框架，模拟实现3D技术并融合了粒子系统物理引擎技术，所有2.5D特效都内嵌于LiteGfx Designer，客户可轻松使用并进行个性化开发，为客户打造与众不同的视觉形象。趣戴科技将不断丰富公司在2.5D技术上的产品储备，助力客户在激烈的市场竞争中脱颖而出。其坚信优秀的视觉设计是提升企业品牌价值和市场竞争力的关键。



<br><div style="text-align: center"><img src="https://docs.realmcu.com/HoneyGUI/image/sample/LiteGFX/wave.gif" width = "300" /></div><br>
<br><div style="text-align: center"><img src="https://docs.realmcu.com/HoneyGUI/image/sample/LiteGFX/polyhedron.gif" width = "300" /></div><br>
<br><div style="text-align: center"><img src="https://docs.realmcu.com/HoneyGUI/image/sample/LiteGFX/shadow.gif" width = "300" /></div><br>
<br><div style="text-align: center"><img src="https://docs.realmcu.com/HoneyGUI/image/sample/LiteGFX/cube.gif" width = "300" /></div><br>
<br><div style="text-align: center"><img src="https://docs.realmcu.com/HoneyGUI/image/sample/LiteGFX/particle.gif" width = "300" /></div><br>
<br><div style="text-align: center"><img src="https://docs.realmcu.com/HoneyGUI/image/sample/LiteGFX/fallen.gif" width = "300" /></div><br>
<br><div style="text-align: center"><img src="https://docs.realmcu.com/HoneyGUI/image/sample/LiteGFX/dial.gif" width = "300" /></div><br>
<br><div style="text-align: center"><img src="https://docs.realmcu.com/HoneyGUI/image/sample/LiteGFX/energy.gif" width = "300" /></div><br>
<br><div style="text-align: center"><img src="https://docs.realmcu.com/HoneyGUI/image/sample/LiteGFX/slide.gif" width = "300" /></div><br>
<br><div style="text-align: center"><img src="https://docs.realmcu.com/HoneyGUI/image/sample/LiteGFX/overturn.gif" width = "300" /></div><br>
<br><div style="text-align: center"><img src="https://docs.realmcu.com/HoneyGUI/image/sample/LiteGFX/ball.gif" width = "300" /></div><br>
<br><div style="text-align: center"><img src="https://docs.realmcu.com/HoneyGUI/image/sample/LiteGFX/number.gif" width = "300" /></div><br>



## 源文件
趣戴组件作为第三方库的形式集成到HoneyGUI中，并作为RealGUI 引擎的一个控件使用，包含核心lib，控件适配，平台支持三个部分:

相关代码路径：`HoneyGUI\gui_engine\3rd\litegfx`


```eval_rst

::

    |-HoneyGUI-Adapt
    |      gui_widget_litegfx.c
    |      gui_widget_litegfx.h
    |      tab_app_energybox.c
    |      tab_app_notifications.c
    |      tab_app_prsim.c
    |      tab_app_prsim_refl.c
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

```
### 控件适配层

此目录下的代码是趣戴为了适配HoneyGUI 提供的wrapper 层代码。其中`gui_widget_litegfx.c/h` 为控件，`tab_` 开头的文件是示例代码，直接使用user可以
参考示例代码实现自己的上层效果。注意，使用此方法本质仍然是通过RealGUI 调用引擎显示相关效果，此控件支持FrameBuffer 的分块方式。

### 平台适配层

平台适配层依据不同的平台重构了适配平台的一些接口,包括调试日志输出,包括内存管理,包括系统时间相关,包括重载一些C++函数(如new等)。

### 核心lib

提供window gcc 版本的liblx_vglite_gcc.a, 提供嵌入式环境版本lx_vglite.lib, 请注意编译器版本。








