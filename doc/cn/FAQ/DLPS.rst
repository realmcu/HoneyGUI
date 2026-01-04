.. _FAQ_DLPS_CN:

========================
UI 与显示模块的 DLPS 设定
========================

本文主要介绍 HoneyGUI 工程中 **UI 模块** 与 **显示模块** 的 **DLPS** （Deep Low Power State，深度低功耗状态）设定与代码调用关系。

DLPS 概述
---------

**DLPS** 是一种系统休眠模式，特点是：

- **功耗更低**
- **进出速度快**
- **RAM 保持** （休眠期间 RAM 内容不丢失）

在低功耗模式下，系统会将不使用的设备置于低功耗状态，使系统整体电流更低；当需要执行任务时，系统可以快速唤醒并恢复全部功能。

根据不同芯片的设计，诸如 **Clock / CPU / Peripherals / RAM** 等模块可能会被关闭电源或关闭时钟，以降低系统功耗。

DLPS 进入/退出原理
------------------

DLPS 的典型流程如下：

1. 系统周期性检查各个模块是否满足 DLPS 条件。
2. 当 **所有模块** 都满足 DLPS 标准时，系统允许进入 DLPS。
3. 当 OS 或外设需要执行任务（中断/事件）时，系统退出 DLPS。
4. 进入或退出 DLPS 时，系统会执行用户注册的 **回调函数**，由各模块完成休眠前/唤醒后的配置。

对 UI/显示来说，回调函数主要用于：

- 让 UI 线程停止刷新，进入消息队列的阻塞等待（从而让系统满足休眠条件）
- 通过显示驱动接口关闭/开启 LCD
- 唤醒后恢复 UI 线程与显示输出

系统初始化：注册 DLPS 回调
--------------------------

在系统电源管理初始化中，需要注册 3 类回调：

- **check 回调**：判断系统是否允许进入 DLPS
- **enter 回调**：进入 DLPS 前执行
- **exit 回调**：退出 DLPS 后执行

典型配置如下：

- ``power_check_cb_register(app_dlps_check_cb);``
- ``DLPS_IORegUserDlpsEnterCb(app_enter_dlps_config);``
- ``DLPS_IORegUserDlpsExitCb(app_exit_dlps_config);``
- ``DLPS_IORegister();``
- ``power_mode_set(POWER_DLPS_MODE);``

在应用层回调中：

- ``app_enter_dlps_config()`` 中会调用 ``rvd_enter_dlps_config()``
- ``app_exit_dlps_config()`` 中会调用 ``rvd_exit_dlps_config()``
- ``app_dlps_check_cb()`` 中会包含 ``rvd_check_dlps()``，要求所有模块 check 都通过才允许 DLPS

UI 模块的 DLPS 回调
-------------------

UI 模块提供：

- ``rvd_check_dlps()``：UI 是否允许 DLPS 的检查函数
- ``rvd_enter_dlps_config()``：进入 DLPS 前的 UI 配置
- ``rvd_exit_dlps_config()``：退出 DLPS 后的 UI 恢复

其中 ``rvd_check_dlps()`` 依赖一个全局标志位：

- ``rvd_dlps_flag = 1``：UI 已空闲、允许进入 DLPS
- ``rvd_dlps_flag = 0``：UI 活跃/显示点亮，不允许进入 DLPS

参考实现：

- ``rvd_check_dlps()`` 返回 ``rvd_dlps_flag``
- ``rvd_enter_dlps_config()`` 可以为空（视产品需求决定）
- ``rvd_exit_dlps_config()`` 用于重启 UI 线程，并重启相关定时器

UI 线程挂起原理：消息队列进入无限等待
------------------------------------

关键点是让 UI server 线程 **阻塞**，避免持续占用 CPU。

在 ``gui_recv_msg_to_server()`` 中：

- 正常情况下使用 ``gui_mq_recv(..., timeout=0)`` 非阻塞接收并处理消息。
- 当 UI 空闲（超过 ``active_ms``）或进入睡眠条件时：

  1. 调用 ``gui_sleep_cb()`` （抽象 hook）
  2. 然后使用 ``gui_mq_recv(..., timeout=0xFFFFFFFF)`` 进入 **无限等待**

无限等待的效果是：

- UI 线程不再运行
- CPU 负载降低
- 更容易满足所有模块的 DLPS check，从而进入 DLPS

UI 线程唤醒原理：发送 DISPLAY_ON 消息
-------------------------------------

唤醒 UI 的关键做法：

- 不让 UI 立即再次走入睡眠分支
- 给 UI server 发送一条点亮显示的消息

在 ``restart_gui_task()`` 中：

- 将 ``app->active_ms`` 设置为 ``0xFFFFFFFF``，保持 UI 活跃
- 向 UI server 发送 ``GUI_EVENT_DISPLAY_ON``

消息到达后，由 ``gui_server_msg_handler()`` 处理 ``GUI_EVENT_DISPLAY_ON``，并调用：

- ``gui_display_on()``
- ``gui_fb_change()``

抽象层：UI 核心到移植层的两个关键接口
--------------------------------------

UI 核心不直接操作硬件，而是通过移植层 hook：

- ``gui_sleep_cb()`` → 调用 ``os_api->gui_sleep_cb`` （若不为 NULL）
- ``gui_display_on()`` → 调用 ``dc->lcd_power_on`` （若不为 NULL）

因此最终的 LCD 开关行为由 **OS 移植层**与**显示移植层**决定。

移植层实现与调用链
------------------

显示移植层（例如 ``gui_port_dc.c``）通常将接口映射为：

- ``dc.lcd_power_on  = port_gui_lcd_power_on``
- ``dc.lcd_power_off = port_gui_lcd_power_off``

典型实现：

- ``port_gui_lcd_power_on()``

  - 调用 ``rtk_lcd_hal_power_on()``
  - 清 ``rvd_dlps_flag`` （UI 变为活跃，阻止 DLPS）

- ``port_gui_lcd_power_off()``

  - 调用 ``rtk_lcd_hal_power_off()``

OS 移植层（例如 ``gui_port_os.c``）通常配置：

- ``os_api.gui_sleep_cb = port_gui_sleep_cb``

典型实现：

- ``port_gui_sleep_cb()``

  - 调用 ``gui_display_off()``（最终会走到 ``dc->lcd_power_off``）
  - 置 ``rvd_dlps_flag = 1``，使 ``rvd_check_dlps()`` 通过

因此整体行为变为：

- UI 空闲时：关闭显示 + 允许 DLPS
- UI 唤醒时：打开显示 + 阻止 DLPS

LCD HAL 的职责
--------------

以下接口与具体硬件相关，通常由屏幕驱动实现：

- ``rtk_lcd_hal_power_on()``
- ``rtk_lcd_hal_power_off()``

它们应根据硬件设计与屏幕规格实现正确的上电/下电时序，例如：

- 电源轨/背光/复位脚控制
- 屏幕 sleep in/out 的时序要求
- 显示控制器时钟与 pinmux 配置

配置检查清单
------------

- 系统侧注册：

  - ``app_dlps_check_cb``
  - ``app_enter_dlps_config``
  - ``app_exit_dlps_config``

- 开启 ``ENABLE_HONEYGUI``，让 UI 参与 DLPS：

  - check：``rvd_check_dlps``
  - enter：``rvd_enter_dlps_config``
  - exit：``rvd_exit_dlps_config``

- 确认 UI 空闲时能真正进入消息队列的无限等待，否则 UI 线程持续运行会阻止 DLPS。

示例：开机 10 秒后自动息屏
--------------------------

下面示例实现“开机 10 秒后若无 UI 活动则息屏”的策略：

策略说明：

- 创建一个 **单次** 定时器（10,000 ms）
- 定时器到期后调用 ``close_gui_task()``
- ``close_gui_task()`` 将 ``active_ms`` 置为 0，使 UI server 很快进入睡眠分支：

  - 调用 ``gui_sleep_cb()`` → 关闭 LCD，并设置 ``rvd_dlps_flag = 1``
  - 然后进入消息队列无限等待

定时器参数
~~~~~~~~~~

- ``#define RVD_DLPS_DURATION_MS 10000``
- ``static TimerHandle_t rvd_timer;``

创建并启动定时器（系统初始化）
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

- ``os_timer_create(&rvd_timer, "rvd_timer", 1, RVD_DLPS_DURATION_MS, false, rvd_timer_callback)``
- ``os_timer_start(&rvd_timer)``

说明：

- 第 5 个参数为 ``false`` 表示单次定时器（one-shot）。

定时器回调
~~~~~~~~~~

- ``rvd_timer_callback()`` 中调用 ``close_gui_task()``。

退出 DLPS 后重启 UI 与定时器
~~~~~~~~~~~~~~~~~~~~~~~~~~~~

在 ``rvd_exit_dlps_config()`` 中：

- 先调用 ``restart_gui_task()`` 点亮屏幕并恢复 UI
- 再调用 ``os_timer_restart(&rvd_timer, RVD_DLPS_DURATION_MS)`` 重新开始计时

这样可以保证每次唤醒后屏幕立即点亮，并再次开始10 秒无操作则息屏的计时。
