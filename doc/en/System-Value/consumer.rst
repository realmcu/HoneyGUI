.. _System-Value-Consumer_EN:

==========================
Usage and Examples
==========================

This chapter is for application authors. It uses the request and subscription
demos shipped with HoneyGUI to show how to read, write, and follow a system
value. Both demos are deliberately narrow: each shows one usage pattern, so
the code can be read as a reference.

Select one with ``menuconfig``:

.. code-block:: none

   System Value Request Demo
   System Value Subscription Demo

The two options are mutually exclusive, and the build compiles only the
selected source file.


Request Example
---------------

``example/sysval/example_sysval_request.c`` shows the request/response path.
A one second timer reads ten keys and puts each answer in its own text
widget:

.. literalinclude:: ../../../example/sysval/example_sysval_request.c
   :language: c
   :start-at: static void refresh(void *obj)
   :end-before: /*====================

If the call returns a valid ID, the callback runs exactly once, on the GUI
thread, after the call has returned. If it returns
``GUI_SYSVAL_REQUEST_INVALID``, the callback never runs, which happens when
the arguments are invalid, the key is unknown or not readable, or memory is
unavailable.

The value passed to the callback is a snapshot. It is valid only for the
duration of the callback: copy what you need, do not keep the pointer. This
call site covers keys answered immediately, keys that take half a second,
keys that are absent from the platform, and keys answered from a worker
thread.

A second, one-shot timer writes a few values and logs what happened. This
snippet writes brightness and records the completion status:

.. literalinclude:: ../../../example/sysval/example_sysval_request.c
   :language: c
   :start-at: value = gui_sysval_value_int(35);
   :end-before: value = gui_sysval_value_bool(true);
   :dedent: 4

The callback reports what actually took effect, which need not be what was
requested. A platform may clamp, round, or refuse part of a range, so the
callback's value is the one to display, not the one that was sent.

Two failure styles are worth telling apart:

- A request that cannot start at all, for example a write to a read-only key
  or a value of the wrong type, is rejected synchronously with
  ``GUI_SYSVAL_REQUEST_INVALID`` and never calls back.
- A request that starts and then fails, for example a value of the right type
  but out of range, calls back with a status such as
  ``GUI_SYSVAL_STATUS_INVALID_VALUE``.


Subscription Example
--------------------

``example/sysval/example_sysval_subscribe.c`` shows the change path. It
subscribes to the battery level and the charging state, and draws whatever
arrives:

.. literalinclude:: ../../../example/sysval/example_sysval_subscribe.c
   :language: c
   :start-at: static void battery_changed
   :end-before: static void charging_changed

.. literalinclude:: ../../../example/sysval/example_sysval_subscribe.c
   :language: c
   :start-at: if (gui_sysval_subscribe(GUI_SYSVAL_KEY_POWER_BATTERY_LEVEL,
   :end-before: gui_log("sysval subscribe failed
   :dedent: 4

The demo does not publish and does not read. Both lines stay as ``--`` until
the platform reports a change, which is the honest picture of what a
subscriber provides: a change stream, not a value cache.

What the subscriber is promised:

- **The latest value, not every value.** A burst of changes before the next
  frame collapses into one call with the newest value.
- **No callback when nothing changed.** Republishing the value that is
  already on screen is filtered out, so a writable key cannot drive the
  widget that wrote it in a loop.
- **GUI thread only.** Widget code is safe here without extra locking.
- **Changes only.** The current value is not replayed. If the first frame has
  to be correct, read once after subscribing and let a stale answer be
  reported as ``GUI_SYSVAL_STATUS_STALE``.

Order is guaranteed per key, not across keys. Do not assume that a battery
callback precedes a charging callback in the same frame.


Subscribing and Reading Together
--------------------------------

A screen that must be correct on the first frame usually does both:

.. code-block:: c

   gui_sysval_subscribe(GUI_SYSVAL_KEY_POWER_BATTERY_LEVEL,
                        battery_changed, battery_text);
   gui_sysval_get_request(GUI_SYSVAL_KEY_POWER_BATTERY_LEVEL,
                          battery_initial_done, battery_text);

The read fills the gap before anything is published. If the platform reports
a change first, the read answer comes back as
``GUI_SYSVAL_STATUS_STALE``; the initial callback should then leave the
display alone instead of overwriting the newer value:

.. code-block:: c

   if (status == GUI_SYSVAL_STATUS_STALE)
   {
       return;   /* A change is already on screen. */
   }


Cancelling
----------

A subscription stays active until it is cancelled. Nothing can detect that
the ``user_data`` it points at has been freed, so a callback that touches a
widget must be cancelled before the widget is destroyed:

.. code-block:: c

   gui_sysval_unsubscribe(subscription);

The handle is invalid once this returns, so clear your copy of it. Calling
``gui_sysval_unsubscribe()`` from inside the callback is safe, and is the
normal way to make a subscription one-shot.

A screen whose widgets live for the whole run, such as a top level watch
face, does not have to cancel anything.


Running the Examples
--------------------

.. code-block:: shell

   cd simulator/win32
   menuconfig ../../Kconfig.gui      # pick one of the two demos
   scons
   cd ../..
   ./simulator/win32/gui             # gui.exe on Windows

The simulator port answers the standard keys with plausible fixed values, so
the demos run without any hardware. The ports live in
``simulator/win32/port/realgui_port/sysval/`` and are also the smallest
readable example of an adapter.
