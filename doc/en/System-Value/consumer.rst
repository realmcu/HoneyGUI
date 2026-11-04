.. _System-Value-Consumer_EN:

==========================
Consumers
==========================

This is the part an application author needs. It covers reading a value
once, writing one, and following one as it changes.


Reading Once
------------

.. literalinclude:: ../../../example/sysval/example_sysval_request.c
   :language: c
   :start-at: static void battery_done
   :end-before: static void charging_done

If the call returns a valid ID, the callback runs exactly once, on the GUI
thread, after the call has returned. If it returns
``GUI_SYSVAL_REQUEST_INVALID``, the callback never runs, which happens when
the arguments are invalid, the key is unknown or not readable, or memory is
unavailable.

The value passed to the callback is a snapshot. It is valid only for the
duration of the callback: copy what you need, do not keep the pointer.


Writing
-------

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


Following Changes
-----------------

.. literalinclude:: ../../../example/sysval/example_sysval_subscribe.c
   :language: c
   :start-at: static void battery_changed
   :end-before: static void charging_changed

.. literalinclude:: ../../../example/sysval/example_sysval_subscribe.c
   :language: c
   :start-at: if (gui_sysval_subscribe(GUI_SYSVAL_KEY_POWER_BATTERY_LEVEL,
   :end-before: gui_log("sysval subscribe failed
   :dedent: 4

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
