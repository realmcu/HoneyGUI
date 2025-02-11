================
GUI Application
================

The GUI framework diagram is shown in the figure below:
  
  
.. figure:: https://foruda.gitee.com/images/1721199084691160190/537fa787_13408154.png
   :align: center
   :width: 800px

   HoneyGUI Framework

+ In each project, multiple applications can exist simultaneously, but only one application can be in a running state at any given time, while the other applications will be in a suspended state.
+ When using different rendering engines, the upper-layer implementation of the application will vary.
+ Each :term:`APP` can create its own dedicated thread, or it may choose not to.
+ The APP can be installed, opened, closed, uninstalled, and switched.
+ The GUI_SERVER performs operations such as traversing widgets according to refresh instructions, rendering the frame buffer, executing trigger callbacks, and scheduling apps.


.. toctree::
    :maxdepth: 2

    C-APP Application<Native/Native>
    Use LVGL Design An Application <LVGL/app_lvgl>
    Use ARM-2D Design An Application <ARM-2D/fwk_app_arm2d>
    Use RVD Tool Design An Application <SaaA/index>
