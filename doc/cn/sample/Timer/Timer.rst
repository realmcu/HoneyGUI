定时器
======

- 此应用程序具有两个界面：定时器和秒表，可以通过点击底部的两个按钮轻松切换。
- 定时器界面：点击即可开始计时，屏幕上会显示秒数递增。
- 秒表界面：使用三个可调滚轴选择您的开始时间，包括小时、分钟和秒。开始后，会有一个动画效果，选择的时间居中并开始倒计时。
.. raw:: html

   <br>
   <div style="text-align: center"><img src="https://foruda.gitee.com/images/1733714624102383656/da340c0e_10088396.gif" width= "400" /></div>
   <br> 

实现
--------------

代码
~~~~
函数  ``app_clock_ui_design`` 位于文件 ``realgui/example/screen_454_454/gui_menu/app_clock.c``。

控件树设计
~~~~~~~~~~~~~~~~~~~
.. figure:: https://foruda.gitee.com/images/1733726655223672334/395e0dfa_10088396.png
   :width: 400px
   :align: center
   :name: 定时器控件树设计
   
   定时器控件树设计

