=============
GUI 应用程序
=============

HoneyGUI 框架的结构图如下所示：

.. figure:: https://foruda.gitee.com/images/1753344638505538859/b08edaa1_10737458.png
   :align: center
   :width: 800px

   HoneyGUI 框架
  

+ 在每个项目中可以预定义多个视图，一般只会显示一个视图，在切换视图时再实时创建新的视图。
+ 每个视图可以设置多种切换方式及切换对象，排版多样。
+ 适配多种第三方库，支持开发丰富的显示效果。
+ GUI_SERVER 根据刷新指令遍历控件，渲染帧缓冲区，执行触发回调等操作。


.. toctree::
    :maxdepth: 2

    C 应用程序 <Native/Native>
    使用可视化工具设计应用程序 <SaaA/index>
    使用 VSCode 插件开发应用程序 <VSCode/index>

