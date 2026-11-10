.. _System-Value-Example_CN:

==========================
示例
==========================

HoneyGUI 附带两个示例。它们刻意写得窄：每个只演示一个 API，别的什么都
不做，因此代码可以直接当作参考来读，而不必从里面翻找与自己问题相关的
那一段。

用 ``menuconfig`` 选择其中一个：

.. code-block:: none

   System Value Request Demo
   System Value Subscription Demo

这两个选项互斥，构建时只会编译被选中的那个源文件。


请求示例
----------------------

``example/sysval/example_sysval_request.c`` 演示请求/应答这一半。一个每秒
触发的定时器读取十个 key，并把每个结果放进各自的文本控件：

.. literalinclude:: ../../../example/sysval/example_sysval_request.c
   :language: c
   :start-at: static void refresh(void *obj)
   :end-before: /*====================

这一处调用点就覆盖了所有值得了解的读取行为：

- 平台立即应答的 key，以及需要半秒的 key；
- 平台上不存在的 key，会以错误应答并在控件里显示占位符；
- 从工作线程读取的 key，其应答会在 GUI 线程上交回。

第二个一次性定时器写入几个值并记录结果。它把三种写入结果并排放在一处：

.. literalinclude:: ../../../example/sysval/example_sysval_request.c
   :language: c
   :start-at: static void write_demo(void *obj)
   :end-before: gui_obj_stop_timer(GUI_BASE(obj));


订阅示例
----------------------

``example/sysval/example_sysval_subscribe.c`` 演示变化这一半。它订阅电量
和充电状态，把收到的内容画出来：

.. literalinclude:: ../../../example/sysval/example_sysval_subscribe.c
   :language: c
   :start-at: static void battery_changed
   :end-before: static void charging_changed

.. literalinclude:: ../../../example/sysval/example_sysval_subscribe.c
   :language: c
   :start-at: if (gui_sysval_subscribe(GUI_SYSVAL_KEY_POWER_BATTERY_LEVEL,
   :end-before: gui_log("sysval subscribe failed
   :dedent: 4

这个示例既不发布也不读取。两行都会保持 ``--``，直到平台上报变化——这
正是订阅者所提供内容的真实写照：一条变化流，而不是一个值缓存。

这是显示平台状态的界面应当照抄的模式：把订阅绑定到它所更新的控件，在
控件销毁时取消订阅，让平台决定值何时变化。


运行示例
----------------------

.. code-block:: shell

   cd simulator/win32
   menuconfig ../../Kconfig.gui      # 选择两个示例之一
   scons
   cd ../..
   ./simulator/win32/gui             # Windows 上是 gui.exe

模拟器移植层用合理的固定值应答标准 key，因此示例无需任何硬件即可运行。
这些移植层位于 ``simulator/win32/port/realgui_port/sysval/``，同时也是
最易读的适配器示例。
