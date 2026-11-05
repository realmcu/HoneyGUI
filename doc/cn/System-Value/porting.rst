.. _System-Value-Porting_CN:

==========================
移植与适配器
==========================

把系统值带到一块新板子上，首先是一次范围界定工作，其次才是编码。多数
板子只支持目录中的一部分，这正是预期结果。


确定板子拥有什么
----------------------

过一遍 :ref:`System-Value-Keys_CN` 中的目录，把每个 key 归入以下三类
之一。

.. list-table:: 为新板子界定范围
   :header-rows: 1
   :widths: 18 40 42

   * - 分类
     - 含义
     - 要写什么
   * - 支持
     - 板子有这个状态，能立刻或稍后应答。
     - 一个至少带一个回调的适配器。
   * - 没有
     - 板子没有这项能力，例如没有气压高度计。
     - 什么都不写。不要注册桩实现。
   * - 稍后支持
     - 硬件存在但驱动未就绪。
     - 暂时什么都不写。驱动落地时再补适配器。

不要注册一个永远上报 ``GUI_SYSVAL_STATUS_ERROR`` 的适配器。对应用来说，
“未知的 key”和“坏掉的 key”看起来一模一样，应用作者会花时间去查一个不
存在的缺陷。不注册才是诚实的回答。


适配器契约
----------------------

适配器是 key 与平台相遇的唯一场所。写一个适配器只需要三个字段和两个
可选回调：

.. literalinclude:: ../../../realgui/sysval/gui_sysval_adapter.h
   :language: c
   :start-at: typedef struct gui_sysval_adapter
   :end-before: } gui_sysval_adapter_t;

字段与 key 目录一一对应：

- ``key`` 与 ``type`` 必须与目录条目完全一致。若类型不是三种已知类型之
  一，或该 key 已被注册，注册会被拒绝。
- 只写 key 把 ``get_request`` 留为 ``NULL``，只读 key 把 ``set_request``
  留为 ``NULL``。两者都留 ``NULL`` 会被拒绝。
- ``next`` 由 ``gui_sysval_adapter_register()`` 填写。

适配器结构体和 key 字符串必须在程序整个生命周期内保持有效，因此文件级
的 ``static`` 对象是常规选择。


应答请求
----------------------

每个请求必须恰好用 ``gui_sysval_respond()`` 应答一次。值就在手边时，可
以在回调内部立即应答：

.. literalinclude:: ../../../simulator/win32/port/realgui_port/sysval/gui_port_sysval_power.c
   :language: c
   :start-at: static void port_sysval_battery_level_get
   :end-before: static void port_sysval_battery_charging_get

从应用的角度看这仍然是异步的。应答会被排队，应用的投递回调稍后在 GUI
线程上执行，绝不会早于 ``gui_sysval_get_request()`` 返回。

需要一次往返时，保存请求，在结果到达的地方应答。下面的 ``get_request``
只保存请求，工作线程稍后应答：

.. literalinclude:: ../../../simulator/win32/port/realgui_port/sysval/gui_port_sysval_async.c
   :language: c
   :start-at: static void port_sysval_async_get
   :end-before: static gui_sysval_adapter_t async_value_adapter

请求指针在应答之前一直有效。应答两次或丢弃不理，都是未定义行为。


状态码
----------------------

适配器上报的状态就是应用看到的状态。

.. list-table::
   :header-rows: 1
   :widths: 32 68

   * - 状态
     - 适配器何时上报
   * - ``GUI_SYSVAL_STATUS_OK``
     - 该值当前有效。
   * - ``GUI_SYSVAL_STATUS_INVALID_VALUE``
     - 写入的类型正确，但平台不接受这个值。key 的 ``Range`` 字段说明
       什么可接受，但平台可能只接受其中的一部分。
   * - ``GUI_SYSVAL_STATUS_BUSY``
     - 后端当前无法处理该请求。例如一次需要无线的读取，而无线正忙。
   * - ``GUI_SYSVAL_STATUS_TIMEOUT``
     - 后端放弃等待。
   * - ``GUI_SYSVAL_STATUS_ERROR``
     - 其他任何失败，包括受支持但当前没有值的 key。

适配器绝不会用错误类型的值上报成功。HoneyGUI 会检查这一点，并把不匹配
转换成 ``GUI_SYSVAL_STATUS_ERROR``。这个检查用于发现错误，并不意味着可
以依赖它兜底。


注册
----------------------

在设备初始化阶段注册适配器，早于任何应用代码运行：

.. literalinclude:: ../../../simulator/win32/port/realgui_port/sysval/gui_port_sysval_power.c
   :language: c
   :start-at: static gui_sysval_adapter_t battery_level_adapter
   :end-before: static gui_sysval_adapter_t battery_charging_adapter

.. literalinclude:: ../../../simulator/win32/port/realgui_port/sysval/gui_port_sysval_power.c
   :language: c
   :start-at: static int port_sysval_power_init
   :end-before: GUI_INIT_DEVICE_EXPORT

``GUI_INIT_DEVICE_EXPORT`` 早于 ``GUI_INIT_APP_EXPORT`` 执行，因此应用
可以在自己的 init 函数里订阅某个 key，并确信适配器已经就位。


为缺失数据选择状态
----------------------

受支持但暂时没有内容可报，是正常情况，不是需要绕过的错误。上报
``GUI_SYSVAL_STATUS_ERROR`` 且不带值，让界面决定画什么。不要发明
``0``、``-1`` 或空字符串之类的哨兵值来表示“没有数据”：零是一个真实的
电量，也是一个真实的步数，应用无法把两者区分开。


按 key 选择形态
----------------------

.. list-table:: 一个 key 需要哪些回调
   :header-rows: 1
   :widths: 25 22 53

   * - 状态种类
     - 回调
     - 说明
   * - 只在被读时采样
     - ``get_request``
     - 在回调内从当前硬件读数应答。
   * - 会被采样且自身会变
     - ``get_request`` + 发布
     - 最常见的情况。每次变化都发布；读取从同一个变量应答。
   * - 需要一次往返
     - ``get_request``
     - 保存请求，在工作线程里应答。
   * - 应用可以设置
     - 再加 ``set_request``
     - 应用变更，然后发布该变化，最后应答请求。

第三行是只做一半就会出问题的：一个适配器的读取路径返回一个变量，而生
产方发布的是另一个变量，就会把同一个 key 的两个不同答案交给应用。让它
们共用同一个所有者。


一个典型的移植模块
----------------------

模拟器里的电源移植层是这类模块的最小完整实现。它拥有状态，读取时从该
状态应答：

.. literalinclude:: ../../../simulator/win32/port/realgui_port/sysval/gui_port_sysval_power.c
   :language: c
   :start-at: static int32_t battery_level
   :end-before: static gui_sysval_adapter_t battery_level_adapter

当这个值会自己变化时，在上面的基础上增加一个发布函数，所有写入都经过
它：

.. code-block:: c

   /* 所有写入都经过这里，保证读取和发布始终一致。 */
   static void battery_level_set(int32_t level)
   {
       gui_sysval_value_t value;

       if (level == battery_level)
       {
           return;
       }
       battery_level = level;
       value = gui_sysval_value_int(level);
       gui_sysval_publish(GUI_SYSVAL_KEY_POWER_BATTERY_LEVEL, &value);
   }

   /* 由电量计驱动调用，任意线程。 */
   void port_fuel_gauge_event(int32_t level)
   {
       battery_level_set(level);
   }

注意 ``battery_level_set()`` 在值没有变化时提前返回。派发路径本身会过
滤掉没有变化的值，但提前返回完全省掉了这部分开销——对一个驱动可能每秒
上报一次的值来说，这很重要。

还要注意 ``gui_sysval_publish()`` 可能失败，例如内存耗尽时。通知不会自
动重试，因此在意的移植层应该检查返回值。下一次变化会再次发布，所以单次
失败由下一个事件恢复，而不是靠重试那一次失败。


唤醒 GUI 线程
----------------------

移植层不需要为投递做任何特殊处理。从中断或驱动任务中发布就够了：当一
个有订阅者的 key 发生变化时，GUI 线程会被唤醒；如果唤醒消息被丢弃，服
务端钩子会在下一帧取走这个值。

完全没有订阅者的 key 不会唤醒线程，也不会把值入队。因此应用可以发布一
个频繁变化的 key（例如计步），在相关界面没有显示时完全不付出代价。


测试移植层
----------------------

有两项检查能在应用发现问题之前抓住大部分错误。

**读取与发布一致。** 订阅该 key，读一次，然后比较。如果中间没有发生变
化，两者应当相等。如果不等，说明这个模块对同一个值有两个所有者。

**每个发布出去的值都可读回。** 发布一个值，然后读该 key，必须读回它。
一个从硬件寄存器应答、而生产方发布的是缓存副本的适配器，会在这项检查
上失败。

两项都足够简单，可以保留为设备上的调试命令。
