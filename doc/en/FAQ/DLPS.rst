.. _FAQ_DLPS_EN:

=================================
DLPS Settings for UI and Display
=================================

This document introduces how to configure **DLPS** (Deep Low Power State) for the **UI module** and the **display module**.

DLPS Overview
-------------

**DLPS** is a system sleep mode designed for:

- **Lower power consumption**
- **Fast entry/exit time**
- **RAM retention** (RAM contents are kept)

In low power mode, the system will put unused devices into a low power state. In this state, devices consume less power and can be woken up quickly to restore full functionality. Depending on the SoC design, many components (e.g., **Clock**, **CPU**, **Peripherals**, and **RAM**) may be powered down to reduce system power.

DLPS Entry/Exit Principle
------------------------

The DLPS mechanism works as follows:

1. The system checks whether each module is in a state that allows DLPS.
2. If **all modules** meet the DLPS criteria, the system enters DLPS.
3. When the OS or peripherals need to run (interrupt/event), the system exits DLPS.
4. When entering/exiting DLPS, the system executes **user-registered callbacks** so each module can prepare for sleep or restore its operation.

For the UI/display, the callbacks are used to:

- Make the UI task stop refreshing and block in a message-queue wait
- Power off/on the LCD through the display driver hooks
- Resume the UI task and re-enable display output after wakeup

Power Manager Registration (System Init)
---------------------------------------

During system initialization, you must register three callbacks:

- **Check callback**: decides whether the system can enter DLPS
- **Enter callback**: executed right before entering DLPS
- **Exit callback**: executed right after exiting DLPS

Typical configuration:

- ``power_check_cb_register(app_dlps_check_cb);``
- ``DLPS_IORegUserDlpsEnterCb(app_enter_dlps_config);``
- ``DLPS_IORegUserDlpsExitCb(app_exit_dlps_config);``
- ``DLPS_IORegister();``
- ``power_mode_set(POWER_DLPS_MODE);``

In the application callbacks:

- ``app_enter_dlps_config()`` calls module enter hooks, including ``rvd_enter_dlps_config()`` .
- ``app_exit_dlps_config()`` calls module exit hooks, including ``rvd_exit_dlps_config()`` .
- ``app_dlps_check_cb()`` requires **all modules** to pass, including ``rvd_check_dlps()`` .

UI Module DLPS Callbacks
-----------------------

The UI module exports the following callbacks:

- ``rvd_check_dlps()``: UI readiness check for DLPS
- ``rvd_enter_dlps_config()``: UI hook before entering DLPS
- ``rvd_exit_dlps_config()``: UI hook after exiting DLPS

The readiness check depends on a global flag:

- ``rvd_dlps_flag = 1``: UI is idle, DLPS allowed
- ``rvd_dlps_flag = 0``: UI is active / display is on, DLPS not allowed

The provided implementation:

- ``bool rvd_check_dlps(void)`` simply returns ``rvd_dlps_flag``.
- ``void rvd_enter_dlps_config(void)`` is empty (placeholder; no extra operation needed).
- ``void rvd_exit_dlps_config(void)`` resumes the UI task and restarts the UI timer.

How the UI Task Enters an Idle State (So DLPS Can Happen)
--------------------------------------------------------

The core idea is to make the UI server task **block** and stop consuming CPU.

In ``gui_recv_msg_to_server()``, the UI server processes messages in a loop while messages are available:

- ``gui_mq_recv(..., timeout=0)`` is used for non-blocking receive.

When UI is inactive (``active_ms`` timeout reached) or a sleep condition is set, the UI uses a sleep path:

1. Call ``gui_sleep_cb()`` (abstract hook)
2. Block on message queue forever using ``gui_mq_recv(..., timeout=0xFFFFFFFF)``

This infinite wait is important:

- The UI task does not run
- CPU load drops
- The system is more likely to pass the global DLPS check and enter DLPS

How the UI Task Wakes Up
------------------------

To wake the UI, the system must both:

- Ensure the UI does not immediately go back to the sleep path
- Send an event that turns the display on

In ``restart_gui_task()``:

- ``app->active_ms`` is set to ``0xFFFFFFFF`` to keep the UI in active mode.
- A message is sent to the UI server: ``GUI_EVENT_DISPLAY_ON``.

Message Handling: Display On
----------------------------

When the UI server receives the message, ``gui_server_msg_handler()`` handles ``GUI_EVENT_DISPLAY_ON`` by calling:

- ``gui_display_on()``
- ``gui_fb_change()``

This re-enables display output and refreshes framebuffer state.

Two Key Abstraction Points
--------------------------

The UI core does not directly control hardware. Instead it calls porting hooks:

- ``gui_sleep_cb()`` calls ``os_api->gui_sleep_cb`` (if not NULL)
- ``gui_display_on()`` calls ``dc->lcd_power_on`` (if not NULL)

So the final behavior depends on the OS port layer and display port layer.

Port Layer Implementation and Call Chain
----------------------------------------

Display port layer (e.g., ``gui_port_dc.c``):

- ``dc.lcd_power_on  = port_gui_lcd_power_on``
- ``dc.lcd_power_off = port_gui_lcd_power_off``

Typical implementation:

- ``port_gui_lcd_power_on()``

  - Calls ``rtk_lcd_hal_power_on()``
  - Clears ``rvd_dlps_flag`` (UI becomes active / DLPS blocked)

- ``port_gui_lcd_power_off()``

  - Calls ``rtk_lcd_hal_power_off()``

OS port layer (e.g., ``gui_port_os.c``):

- ``os_api.gui_sleep_cb = port_gui_sleep_cb``

Typical implementation:

- ``port_gui_sleep_cb()``

  - Calls ``gui_display_off()`` (which will reach ``dc->lcd_power_off``)
  - Sets ``rvd_dlps_flag = 1`` so ``rvd_check_dlps()`` passes

With this design:

- When the UI becomes idle, it powers **off** the display and allows DLPS.
- When the UI wakes up, it powers **on** the display and blocks DLPS.

LCD HAL Responsibilities
------------------------

The functions below are hardware-dependent and implemented in the concrete display driver:

- ``rtk_lcd_hal_power_on()``
- ``rtk_lcd_hal_power_off()``

They should implement the correct power sequence based on:

- Board power design (rails, backlight, reset pins)
- Panel requirements (sleep in/out timing)
- Controller clock/pinmux requirements

Configuration Checklist
-----------------------

- System register:

  - ``app_dlps_check_cb``
  - ``app_enter_dlps_config``
  - ``app_exit_dlps_config``

- Ensure the UI hooks participate in:

  - DLPS check: ``rvd_check_dlps``
  - DLPS enter: ``rvd_enter_dlps_config``
  - DLPS exit: ``rvd_exit_dlps_config``

- Ensure UI idling actually blocks on the message queue with an infinite timeout, otherwise the UI task may keep running and prevent DLPS.

Example: Turn Off the Display 10 Seconds After Boot
---------------------------------------------------

This example shows a common policy: **turn off the display 10 seconds after boot** if there is no UI activity.

Design summary:

- Create a **one-shot** timer (10,000 ms).
- When the timer expires, call ``close_gui_task()``.
- ``close_gui_task()`` sets ``active_ms = 0`` so the UI server quickly enters the sleep path:

  - calls ``gui_sleep_cb()`` → powers off the LCD and sets ``rvd_dlps_flag = 1``
  - then blocks on the message queue with infinite timeout

Timer definitions
~~~~~~~~~~~~~~~~~

- ``#define RVD_DLPS_DURATION_MS 10000``
- ``static TimerHandle_t rvd_timer;``

Timer creation and start (during system init)
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

Create the timer at initialization and start it immediately after boot:

- ``os_timer_create(&rvd_timer, "rvd_timer", 1, RVD_DLPS_DURATION_MS, false, rvd_timer_callback)``
- ``os_timer_start(&rvd_timer)``

Notes:

- The 5th parameter is set to ``false`` to indicate a **one-shot** timer.
- If you want periodic sleep behavior, set it to ``true`` and decide when to stop it.

Timer callback
~~~~~~~~~~~~~~

When the timer fires, suspend the UI task:

- ``rvd_timer_callback()`` calls ``close_gui_task()``

This causes the UI server to reach the sleep branch in ``gui_recv_msg_to_server()`` and eventually power off the display.

Restart after waking up (DLPS exit)
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

On DLPS exit, restart the UI task and re-arm the 10-second screen-off timer:

- ``rvd_exit_dlps_config()`` calls ``restart_gui_task()``
- Then calls ``os_timer_restart(&rvd_timer, RVD_DLPS_DURATION_MS)``

This ensures that after wakeup the display turns on immediately, and the screen-off policy starts counting again.
