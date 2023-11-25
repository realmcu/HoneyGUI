# **GUI APP FrameWork**

  +  The GUI framework diagram is shown in the figure below:
  
  
![APP](https://foruda.gitee.com/images/1669962011556176197/5cf9c62c_10088396.png "app.png")


  +  Each APP consists of a thread, a control tree, and event trigger callbacks (optional).
  +  The GUI_SERVER traverses the widgets according to the refresh instruction, renders the frame buffer, executes the trigger callback, schedules the app and other operations.
  +  APP can be installed, opened, closed, uninstalled and switched.


```eval_rst
.. toctree::
    :hidden:

    Use C/C++ Design an Application <GUI-APP Demo>
    Use LVGL Design an Application <fwk_app_lvgl>
    Use Script Design an Application <SaaA/index>
    How to Prepare UI Resource <GUI-APP UI resource>
    Demo an Application <GUI-APP Demo>
```