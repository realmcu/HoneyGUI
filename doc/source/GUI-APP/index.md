# **GUI APP FrameWork**

  +  The GUI framework diagram is shown in the figure below:
  
  
![APP](https://foruda.gitee.com/images/1669962011556176197/5cf9c62c_10088396.png "app.png")


  +  Each APP consists of a thread, a control tree, and event trigger callbacks (optional).
  +  The GUI_SERVER traverses the widgets according to the refresh instruction, renders the frame buffer, executes the trigger callback, schedules the app and other operations.
  +  APP can be installed, opened, closed, uninstalled and switched.


```eval_rst
.. toctree::
    :hidden:

    Use C/C++ Design An Application <Native/Native>
    Use LVGL Design An Application <LVGL/fwk_app_lvgl>
    Use Script Design An Application <SaaA/index>
```
