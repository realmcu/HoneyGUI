.. _System-Value-Consumer_CN:

==========================
使用方法与示例
==========================

这一章面向应用开发者，结合 HoneyGUI 附带的请求示例和订阅示例，说明如何
读取、写入并跟随一个系统值的变化。两个示例刻意写得窄：每个只演示一种
使用方式，代码可以直接作为参考。

用 ``menuconfig`` 选择其中一个：

.. code-block:: none

   System Value Request Demo
   System Value Subscription Demo

这两个选项互斥，构建时只会编译被选中的源文件。


请求示例
----------------------

``example/sysval/example_sysval_request.c`` 演示请求/应答路径。一个每秒
触发的定时器读取十个 key，并把每个结果放进各自的文本控件：

.. literalinclude:: ../../../example/sysval/example_sysval_request.c
   :language: c
   :start-at: static void refresh(void *obj)
   :end-before: /*====================

如果调用返回了有效的 ID，回调会恰好执行一次，在 GUI 线程上，且在调用
返回之后。如果返回 ``GUI_SYSVAL_REQUEST_INVALID``，回调永远不会执行：
这发生在参数无效、key 未知或不可读，或者内存不足时。

传给回调的值是一个快照，仅在回调执行期间有效：需要什么就复制出来，不
要保留指针。这个调用点同时覆盖了平台立即应答、需要半秒才应答、平台上
不存在，以及从工作线程应答的 key。

第二个一次性定时器写入几个值并记录结果，写入亮度并记录完成状态：

.. literalinclude:: ../../../example/sysval/example_sysval_request.c
   :language: c
   :start-at: value = gui_sysval_value_int(35);
   :end-before: value = gui_sysval_value_bool(true);
   :dedent: 4

回调报告的是**实际生效**的值，不一定是你请求的值。平台可能会截断、取
整，或拒绝范围内的某一部分，因此要显示的是回调里的值，而不是发出去的
那个。

有两种失败方式值得区分：

- 请求根本无法启动，例如写只读 key 或值类型不对，会同步返回
  ``GUI_SYSVAL_REQUEST_INVALID``，且永不回调。
- 请求启动了但随后失败，例如类型正确但超出范围，会以
  ``GUI_SYSVAL_STATUS_INVALID_VALUE`` 之类的状态回调。


订阅示例
----------------------

``example/sysval/example_sysval_subscribe.c`` 演示变化路径。它订阅电量
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

这个示例既不发布也不读取。两行都会保持 ``--``，直到平台上报变化。这正
是订阅者所提供内容的真实写照：一条变化流，而不是一个值缓存。

订阅者能得到以下保证：

- **最新值，而不是每个值。** 下一帧之前的一阵变化会合并成一次调用，带
  最新的值。
- **没有变化就不回调。** 与屏幕上已经显示的值相同的发布会被过滤，因此
  可写 key 无法驱动写下它的那个控件形成回路。
- **只在 GUI 线程。** 在这里写控件代码无需额外加锁。
- **只承载变化。** 当前值不会被重放。如果第一帧就必须正确，请在订阅后
  读一次，并让过期的应答以 ``GUI_SYSVAL_STATUS_STALE`` 上报。

顺序只在单个 key 内保证，跨 key 不保证。不要假设同一帧里电池回调一定
在充电回调之前。


订阅与读取配合使用
----------------------

第一帧就必须正确的界面通常两者都做：

.. code-block:: c

   gui_sysval_subscribe(GUI_SYSVAL_KEY_POWER_BATTERY_LEVEL,
                        battery_changed, battery_text);
   gui_sysval_get_request(GUI_SYSVAL_KEY_POWER_BATTERY_LEVEL,
                          battery_initial_done, battery_text);

这次读取填补了还没有任何发布之前的空档。如果平台先上报了变化，读取的
应答会以 ``GUI_SYSVAL_STATUS_STALE`` 返回；此时初始回调不要去动显示内
容，以免覆盖更新的值：

.. code-block:: c

   if (status == GUI_SYSVAL_STATUS_STALE)
   {
       return;   /* 屏幕上已经是变化后的新值。 */
   }


取消订阅
----------------------

订阅会一直有效，直到被取消。系统无法检测它指向的 ``user_data`` 已被释
放，因此会访问控件的回调，必须在该控件销毁之前取消：

.. code-block:: c

   gui_sysval_unsubscribe(subscription);

调用返回后句柄即失效，请把你保存的那份清掉。在回调内部调用
``gui_sysval_unsubscribe()`` 是安全的，这也是让订阅只生效一次的常规
做法。

如果界面的控件在整个运行期间都存活，例如顶层表盘，则不需要取消任何
订阅。


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
