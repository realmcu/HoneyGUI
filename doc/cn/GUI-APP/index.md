# **GUI应用程序框架**

  +  HoneyGUI框架的结构图如下所示：
  
  
<div style="text-align: center"><img width= "600" src="https://foruda.gitee.com/images/1720764399046158372/8fde9e89_1860080.png" alt="app fw.png"></div><br/>

  +  在每个项目中可以同时存在多个应用程序（APP），但同一时间只有一个应用程序可以处于运行状态，其他应用程序将处于挂起状态。
  +  使用不同的显示引擎时，应用程序（APP）的上层代码写法会有所不同。
  +  每个APP可以创建独立的线程，也可以选择不创建。
  +  APP可以进行安装、打开、关闭、卸载和切换的操作。
  +  GUI_SERVER根据刷新指令遍历控件，渲染帧缓冲区，执行触发回调，进行APP调度等操作。


```eval_rst
.. toctree::
    :hidden:

    使用C/C++设计应用程序 <Native/Native>
    使用LVGL设计应用程序 <LVGL/fwk_app_lvgl>
    使用ARM-2D设计应用程序 <ARM-2D/fwk_app_arm2d>
    使用脚本设计应用程序 <SaaA/index>
    切换应用程序 <Switch_APP/index>
```
