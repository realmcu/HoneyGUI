.. _System-Value-Adapter_CN:

==========================
适配器
==========================

适配器是 key 与平台相遇的唯一场所。写一个适配器只需要三个字段和两个
可选回调。


契约
----------------------

.. literalinclude:: ../../../realgui/sysval/gui_sysval_adapter.h
   :language: c
   :start-at: typedef struct gui_sysval_adapter
   :end-before: } gui_sysval_adapter_t;

字段与 key 目录一一对应：

- ``key`` 与 ``type`` 必须与目录条目完全一致。若类型不是三种已知类型之
  一，或该 key 已被注册，注册会被拒绝。
- 只写 key 把 ``get_request`` 留为 ``NULL``，只读 key 把 ``set_request``
  留为 ``NULL``。两者都留 ``NULL`` 会被拒绝：一个两样都不做的适配器，
  只会让这个 key 看起来被支持，而每次请求都失败。
- ``next`` 由 ``gui_sysval_adapter_register()`` 填写。

适配器结构体和 key 字符串必须在程序整个生命周期内保持有效，因此文件级
的 ``static`` 对象是常规选择。


应答请求
----------------------

每个请求必须恰好用 ``gui_sysval_respond()`` 应答一次。有两种形态，适配
器针对每个请求选择其一。

**立即应答。** 值就在手边时，在回调内部应答：

.. literalinclude:: ../../../simulator/win32/port/realgui_port/sysval/gui_port_sysval_power.c
   :language: c
   :start-at: static void port_sysval_battery_level_get
   :end-before: static void port_sysval_battery_charging_get

从应用的角度看这仍然是异步的。应答会被排队，应用的投递回调稍后在 GUI
线程上执行，绝不会早于 ``gui_sysval_get_request()`` 返回。

**延迟应答。** 需要往返一次时，保存请求，在结果到达的地方应答。
下面的 ``get_request`` 只保存请求，工作线程稍后应答：

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
转换成 ``GUI_SYSVAL_STATUS_ERROR``，而不是把一个联合体交给应用、让它用
错误的成员去读。但这个检查是用来发现错误的，不是让错误变得可接受。


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

平台不支持的 key，移植层干脆不注册。这就是可选能力的全部机制：没有编
译开关，没有弱符号，只是一个应答 ``GUI_SYSVAL_REQUEST_INVALID`` 的 key。


为缺失数据选择状态
----------------------

受支持但暂时没有内容可报，是正常情况，不是需要绕过的错误。上报
``GUI_SYSVAL_STATUS_ERROR`` 且不带值，让界面决定画什么。不要发明
``0``、``-1`` 或空字符串之类的哨兵值来表示“没有数据”：零是一个真实的
电量，也是一个真实的步数，应用无法把两者区分开。
