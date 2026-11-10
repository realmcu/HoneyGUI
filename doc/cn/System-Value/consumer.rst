.. _System-Value-Consumer_CN:

==========================
使用方
==========================

这一章是应用开发者需要的部分。涵盖读一次、写一次，以及跟随一个值的
变化。


读一次
----------------------

.. literalinclude:: ../../../example/sysval/example_sysval_request.c
   :language: c
   :start-at: static void battery_done
   :end-before: static void charging_done

如果调用返回了有效的 ID，回调会恰好执行一次，在 GUI 线程上，且在调用
返回之后。如果返回 ``GUI_SYSVAL_REQUEST_INVALID``，回调永远不会执行：
这发生在参数无效、key 未知或不可读，或者内存不足时。

传给回调的值是一个快照，仅在回调执行期间有效：需要什么就复制出来，不
要保留指针。


写入
----------------------

下面这段取自请求示例，写入亮度并用 ``write_done`` 记录结果：

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


跟随变化
----------------------

取订阅示例中的回调和订阅调用：

.. literalinclude:: ../../../example/sysval/example_sysval_subscribe.c
   :language: c
   :start-at: static void battery_changed
   :end-before: static void charging_changed

.. literalinclude:: ../../../example/sysval/example_sysval_subscribe.c
   :language: c
   :start-at: if (gui_sysval_subscribe(GUI_SYSVAL_KEY_POWER_BATTERY_LEVEL,
   :end-before: gui_log("sysval subscribe failed
   :dedent: 4

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
