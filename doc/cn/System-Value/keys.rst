.. _System-Value-Keys_CN:

==========================
key 目录
==========================

key 是命名一项状态的小写 ASCII 路径。这份目录定义在
``realgui/sysval/gui_sysval_keys.h`` 中，那里是权威参考：每个 key 都带有
类型、访问权限、单位和范围的注释块。本章说明如何阅读这些定义，以及如何
添加自己的 key。


如何阅读一个定义
----------------------

.. literalinclude:: ../../../realgui/sysval/gui_sysval_keys.h
   :language: c
   :start-at:  * Latest valid heart-rate measurement.
   :end-before: #define GUI_SYSVAL_KEY_HEALTH_HEART_RATE
   :prepend: /**
   :append: */

请使用 ``GUI_SYSVAL_KEY_*`` 宏，不要直接写字符串。正是这个宏让笔误变成
编译错误，而不是一次运行时静默失败。


目录分组
----------------------

.. list-table:: 顶层命名空间
   :header-rows: 1
   :widths: 20 80

   * - 命名空间
     - 内容
   * - ``system/``
     - 本地时间与日期、时间格式、时区、语言、免打扰。
   * - ``device/``
     - 产品名称、型号和固件版本。
   * - ``power/``
     - 电量、充电状态、剩余时间、电压、外接电源、省电模式。
   * - ``display/``
     - 亮度、息屏超时、常亮、抬腕亮屏。
   * - ``audio/`` 与 ``haptics/``
     - 音量、静音、振动。
   * - ``connectivity/``
     - 蓝牙与 Wi-Fi 的使能状态和连接状态。
   * - ``health/``
     - 计步、心率、血氧、血压、体温、压力、睡眠与佩戴状态。
   * - ``workout/``
     - 状态、模式、已用时、步数、距离、卡路里、心率汇总。
   * - ``weather/``
     - 城市、天气状况、温度与湿度，附带数据时效。
   * - ``media/``
     - 音乐状态、音量、标题与艺术家。
   * - ``notifications/`` 与 ``alarms/``
     - 仅有数量。条目本身需要一个列表接口，超出了单个值 key 的范围。
   * - ``stopwatch/`` 与 ``countdown/``
     - 状态、已用时间、设定时长与剩余时间。
   * - ``reminders/``
     - 久坐与喝水提醒，及其间隔。


按类型读取值
----------------------

目录保证了类型，因此该读哪个成员是预先确定的。但仍然要先检查值指针和状
态：受支持但当前没有读数的 key 会以错误且不带值的方式应答，不检查就访
问联合体会用到过期或空的成员。

.. list-table:: 联合体成员
   :header-rows: 1
   :widths: 25 25 50

   * - 类型
     - 成员
     - 示例 key
   * - ``GUI_SYSVAL_TYPE_BOOL``
     - ``value->data.b``
     - ``GUI_SYSVAL_KEY_POWER_BATTERY_CHARGING``
   * - ``GUI_SYSVAL_TYPE_INT``
     - ``value->data.i``
     - ``GUI_SYSVAL_KEY_POWER_BATTERY_LEVEL``
   * - ``GUI_SYSVAL_TYPE_STR``
     - ``value->data.s``
     - ``GUI_SYSVAL_KEY_WEATHER_CITY``

字符串最多容纳 ``GUI_SYSVAL_STR_MAX - 1`` 字节，且总是以 NUL 结尾。过长
的文本由适配器在 UTF-8 字符边界上截断，因此字符串可能比平台自身的文本
短，但绝不会在字符中间被切断。


新增产品专用 key
----------------------

这份目录覆盖的是许多产品共有的状态。任何只属于某个产品的状态，都应放在
私有命名空间下，且不得重新定义标准 key：

.. code-block:: c

   /* 产品专用：工牌 LED 动画速度。 */
   #define GUI_SYSVAL_KEY_BADGE_LED_SPEED "badge/led/speed"

新增 key 的规则：

- 描述可观察的状态，而不是动作。名为 ``reboot`` 的 key 是穿着状态外衣
  的命令；应该用带 ``set_request`` 的适配器，配一个
  ``system/reboot_pending`` 这样的状态 key。
- 把单位写进文档，不要写进路径。目录里有一个历史遗留的例外
  ``power/battery/remaining_seconds``，为兼容性保留。
- 用定点整数而不是浮点数。以 0.1 度为单位的 ``365`` 就是 36.5 度，单位
  那一行会说明这一点。
- 取值集合很小的状态，用字符串承载文档中列出的某个小写标记，而不是用整
  数配头文件里的枚举。
- 不要为只有某一个界面会读的值新增 key。一个本地缓存变量更简单，也不会
  扩大移植面。


注意事项
----------------------

**没有原子快照。** 每个请求独立应答。读三个 key 并不给出一致的系统视图：
你读到的电量可能比你接着读到的充电状态早一秒。设计界面时，要让每个值都
能被独立渲染。

**可选 key。** 声明一个 key 不等于注册它。适配器只注册平台支持的部分，
其余全部应答 ``GUI_SYSVAL_REQUEST_INVALID``。要把它理解成“这个产品没有
这项功能”，而不是一次临时故障，也不要重试。

**零是一个值。** ``0%`` 电量、``0`` 步和 ``0`` 条未读消息都是真实的。缺
失数据有它自己的上报路径——一个不带值的错误状态——因此界面不能把零当作
“未知”。
