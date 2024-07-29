# GUI Application

The GUI framework diagram is shown in the figure below:
  
  
<div style="text-align: center"><img width= "600" src="https://foruda.gitee.com/images/1721199084691160190/537fa787_13408154.png" alt="app fw.png"></div><br/>

  +  In each project, multiple applications can exist simultaneously, but only one application can be in a running state at any given time, while the other applications will be in a suspended state.
  +  When using different rendering engines, the upper-layer implementation of the application will vary.
  +  Each [APP](/Glossary.rst#term-APP) can create its own dedicated thread, or it may choose not to.
  +  The APP can be installed, opened, closed, uninstalled, and switched.
  +  The GUI_SERVER performs operations such as traversing controls according to refresh instructions, rendering the frame buffer, executing trigger callbacks, and scheduling apps.


```eval_rst
.. toctree::
    :maxdepth: 2

    C-APP <Native/Native>
    Use LVGL Design An Application <LVGL/fwk_app_lvgl>
    Use Script Design An Application <SaaA/index>
    Application switching <Switch_APP/index>
```
