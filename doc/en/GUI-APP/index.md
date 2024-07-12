# **GUI APP FrameWork**

  +  The GUI framework diagram is shown in the figure below:
  
  
<div style="text-align: center"><img width= "600" src="https://foruda.gitee.com/images/1720764399046158372/8fde9e89_1860080.png" alt="app fw.png"></div><br/>

  +  In each project, multiple applications (APPs) can exist simultaneously, but only one application can be in a running state at any given time, while the other applications will be in a suspended state.
  +  When using different rendering engines, the upper-layer implementation of the application (APP) will vary.
  +  Each app can create its own dedicated thread, or it may choose not to.
  +  The app can be installed, opened, closed, uninstalled, and switched.
  +  The GUI_SERVER performs operations such as traversing controls according to refresh instructions, rendering the frame buffer, executing trigger callbacks, and scheduling apps.


```eval_rst
.. toctree::
    :maxdepth: 2

    Use C/C++ Design An Application <Native/Native>
    Use LVGL Design An Application <LVGL/fwk_app_lvgl>
    Use Script Design An Application <SaaA/index>
    Switch Application <Switch_APP/index>
```
