#  LiteGFX Application

## LiteGFX

QuDai Technology is a software service company that leverages its self-developed LiteGfx framework to fully harness the performance of various chips, providing customers with cross-platform, one-stop GUI solutions and a plethora of dazzling visual effects products. By utilizing our proprietary 2.5D effects framework, we simulate 3D technology and integrate particle system physics engine technology. All 2.5D effects are embedded within LiteGfx Designer, allowing customers to easily use and personalize them to create unique visual identities. QuDai Technology will continuously enrich its product portfolio in 2.5D technology, helping clients stand out in the fiercely competitive market. We firmly believe that excellent visual design is the key to enhancing a company's brand value and market competitiveness.



<br><div style="text-align: center"><img src="https://foruda.gitee.com/images/1721025203699242435/1a08e055_1860080.gif " width = "300" /></div><br>
<br><div style="text-align: center"><img src="https://foruda.gitee.com/images/1721025223784999234/7c82e0f3_1860080.gif " width = "300" /></div><br>
<br><div style="text-align: center"><img src="https://foruda.gitee.com/images/1721025240766030953/f2010214_1860080.gif " width = "300" /></div><br>
<br><div style="text-align: center"><img src="https://foruda.gitee.com/images/1721025257809181438/fbdb018d_1860080.gif " width = "300" /></div><br>
<br><div style="text-align: center"><img src="https://foruda.gitee.com/images/1721025277229063694/2caf146f_1860080.gif " width = "300" /></div><br>
<br><div style="text-align: center"><img src="https://foruda.gitee.com/images/1721025296329833651/00073d34_1860080.gif " width = "300" /></div><br>
<br><div style="text-align: center"><img src="https://foruda.gitee.com/images/1721025332862441151/670474ac_1860080.gif " width = "300" /></div><br>
<br><div style="text-align: center"><img src="https://foruda.gitee.com/images/1721025351585750284/7924821a_1860080.gif " width = "300" /></div><br>
<br><div style="text-align: center"><img src="https://foruda.gitee.com/images/1721025370264867322/3aa86ee3_1860080.gif " width = "300" /></div><br>
<br><div style="text-align: center"><img src="https://foruda.gitee.com/images/1721025410917976043/d2d1e013_1860080.gif " width = "300" /></div><br>
<br><div style="text-align: center"><img src="https://foruda.gitee.com/images/1721025448772726826/e8126702_1860080.gif " width = "300" /></div><br>
<br><div style="text-align: center"><img src="https://foruda.gitee.com/images/1721025535725973914/e2d8d5b1_1860080.gif " width = "300" /></div><br>



## Source Code
QuDai components are integrated into HoneyGUI as a third-party library and used as a Widget in the RealGUI engine. This integration includes three main parts: core library, control adaptation, and platform support.

Source code path:`HoneyGUI\gui_engine\3rd\litegfx`


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
### Widget adaptation layer

The code in this directory is a wrapper layer provided by QuDai to adapt to HoneyGUI. The files `gui_widget_litegfx.c/h` are for the widgets, while files starting with `tab_` are sample code. 
Users can refer to these sample codes to implement their own upper-layer effects. Note that essentially, using this method still involves calling the RealGUI engine to display related effects. This widget supports the tiled approach for FrameBuffer.

### Platform adaptation layer

This directory restructures some interfaces to accommodate different platforms for convenience. This includes debugging log output, memory management, the restructuring of some C++ functions like new, and system time-related functionalities.

### Core lib

It provides the Windows GCC version of `liblx_vglite_gcc.a` and the embedded environment version of `lx_vglite.lib`. Please pay attention to the compiler version.








