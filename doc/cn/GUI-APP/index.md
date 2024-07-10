# **GUI应用程序框架**

  +  HoneyGUI框架的结构图如下所示：
  
  
<div style="text-align: center"><img width= "600" src="https://foruda.gitee.com/images/1703071724612530370/974a550f_10088396.png" alt="app fw.png"></div><br/>

  +  每个APP由一个线程、一个控件树和事件触发回调（可选）组成。
  +  GUI_SERVER根据刷新指令遍历控件，渲染帧缓冲区，执行触发回调，进行APP调度等操作。
  +  APP可以进行安装、打开、关闭、卸载和切换的操作。


```eval_rst
.. toctree::
    :hidden:

    使用C/C++设计应用程序 <Native/Native>
    使用LVGL设计应用程序 <LVGL/fwk_app_lvgl>
    使用ARM-2D设计应用程序 <ARM-2D/fwk_app_arm2d>
    使用脚本设计应用程序 <SaaA/index>
    切换应用程序 <Switch_APP/index>
```
