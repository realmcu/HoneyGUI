.. _System-Value-Example_EN:

==========================
Examples
==========================

Two demos ship with HoneyGUI. They are deliberately narrow: each one shows a
single API and nothing else, so the code can be read as a reference rather
than mined for the part relevant to a particular question.

Select one with ``menuconfig``:

.. code-block:: none

   System Value Request Demo
   System Value Subscription Demo

The two options are mutually exclusive, and the build compiles only the
selected source file.


Request Demo
------------

``example/sysval/example_sysval_request.c`` shows the request/response half.
A one second timer reads ten keys and puts each answer in its own text
widget:

.. literalinclude:: ../../../example/sysval/example_sysval_request.c
   :language: c
   :start-at: static void refresh(void *obj)
   :end-before: /*====================

That single call site covers every read behaviour worth knowing:

- keys the platform answers immediately, and keys that take half a second;
- keys that are absent from the platform, which answer with an error and a
  placeholder in the widget;
- keys read from a worker thread, whose answers are handed back on the GUI
  thread.

A second, one-shot timer writes a few values and logs what happened. It shows
the three write outcomes side by side in one place:

.. literalinclude:: ../../../example/sysval/example_sysval_request.c
   :language: c
   :start-at: static void write_demo(void *obj)
   :end-before: gui_obj_stop_timer(GUI_BASE(obj));


Subscription Demo
-----------------

``example/sysval/example_sysval_subscribe.c`` shows the change half. It
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

This is the pattern to copy for a screen that displays platform state. Bind
the subscription to the widget it updates, cancel it when the widget goes
away, and let the platform decide when the value changes.


Running the Demos
-----------------

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
