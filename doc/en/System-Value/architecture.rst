.. _System-Value-Architecture_EN:

==========================
Architecture
==========================

This chapter first defines the key catalog used by System Value, then follows
one value from the moment a driver notices it changed until the widget that
shows it has been updated, and explains why each step looks the way it does.


.. _System-Value-Keys_EN:

Key Catalog
-----------

A key is a lower-case ASCII path that names one piece of state. The catalog
is defined in ``realgui/sysval/gui_sysval_keys.h``, which is the reference:
each key there carries its type, access, unit and range as a comment block.
This section explains how to read those definitions and how to add keys of
your own.


Reading a Definition
~~~~~~~~~~~~~~~~~~~~~

.. literalinclude:: ../../../realgui/sysval/gui_sysval_keys.h
   :language: c
   :start-at:  * Latest valid heart-rate measurement.
   :end-before: #define GUI_SYSVAL_KEY_HEALTH_HEART_RATE
   :prepend: /**
   :append: */

Use the ``GUI_SYSVAL_KEY_*`` macro, never the string. The macro is what makes
a typo a compile error instead of a request that silently fails at runtime.


Groups in the Catalog
~~~~~~~~~~~~~~~~~~~~~

.. list-table:: Top level namespaces
   :header-rows: 1
   :widths: 20 80

   * - Namespace
     - Contents
   * - ``system/``
     - Wall-clock time and date, time format, time zone, language, do not
       disturb.
   * - ``device/``
     - Name, model and firmware version of the product.
   * - ``power/``
     - Battery level, charging state, remaining time, voltage, external
       power, power saving.
   * - ``display/``
     - Brightness, screen timeout, always on, lift to wake.
   * - ``audio/`` and ``haptics/``
     - Volume, mute, vibration.
   * - ``connectivity/``
     - Bluetooth and Wi-Fi enablement and connection state.
   * - ``health/``
     - Pedometer, heart rate, blood oxygen, blood pressure, temperature,
       stress, sleep and wearing state.
   * - ``workout/``
     - State, mode, elapsed time, steps, distance, calories, heart rate
       summary.
   * - ``weather/``
     - City, condition, temperature and humidity, with an age.
   * - ``media/``
     - Music state, volume, title and artist.
   * - ``notifications/`` and ``alarms/``
     - Counts only. The entries themselves need a list interface, which is
       out of scope for a single value key.
   * - ``stopwatch/`` and ``countdown/``
     - State, elapsed, configured duration and remaining time.
   * - ``reminders/``
     - Sedentary and drink water reminders, with their intervals.


Reading a Value by Type
~~~~~~~~~~~~~~~~~~~~~~~

The catalog guarantees the type, so the member to read is known in advance.
Still check the value pointer and the status first: a supported key with no
current reading answers with an error and no value, and reading the union
without checking would use a stale or empty member.

.. list-table:: Union members
   :header-rows: 1
   :widths: 25 25 50

   * - Type
     - Member
     - Example key
   * - ``GUI_SYSVAL_TYPE_BOOL``
     - ``value->data.b``
     - ``GUI_SYSVAL_KEY_POWER_BATTERY_CHARGING``
   * - ``GUI_SYSVAL_TYPE_INT``
     - ``value->data.i``
     - ``GUI_SYSVAL_KEY_POWER_BATTERY_LEVEL``
   * - ``GUI_SYSVAL_TYPE_STR``
     - ``value->data.s``
     - ``GUI_SYSVAL_KEY_WEATHER_CITY``

Strings hold at most ``GUI_SYSVAL_STR_MAX - 1`` bytes and are always NUL
terminated. Longer text is truncated by the adapter on a UTF-8 character
boundary, so a string may be shorter than the platform's own text but is
never cut mid-character.


Adding a Product Key
~~~~~~~~~~~~~~~~~~~~

The catalog covers state common to many products. Anything specific to one
product goes in a private namespace and must not redefine a standard key:

.. code-block:: c

   /* Product specific: badge LED animation speed. */
   #define GUI_SYSVAL_KEY_BADGE_LED_SPEED "badge/led/speed"

Rules for a new key:

- Describe observable state, not an action. A key named ``reboot`` is a
  command wearing a state key's clothes; use an adapter with a
  ``set_request`` and a state key such as ``system/reboot_pending`` instead.
- Put the unit in the documentation, not in the path. The catalog has one
  historical exception, ``power/battery/remaining_seconds``, which is kept
  for compatibility.
- Use a fixed-point integer rather than a float. ``365`` in tenths of a
  degree is 36.5 degrees, and the unit line says so.
- A state that has a small set of values is a string holding one of the
  documented lower-case tokens, not an integer with an enum in the header.
- Do not add a key for a value that only one screen will ever read. A cached
  local variable is simpler and does not grow the porting surface.


Caveats
~~~~~~~

**No atomic snapshots.** Each request is answered on its own. Reading three
keys does not give a consistent view of the system: the battery level you
read may be one second older than the charging state you read next. Design a
screen so that each value is independently renderable.

**Optional keys.** Declaring a key does not register it. An adapter registers
only what the platform supports, and everything else answers
``GUI_SYSVAL_REQUEST_INVALID``. Treat that as "this product has no such
feature", not as a transient failure, and do not retry it.

**Zero is a value.** ``0%`` battery, ``0`` steps and ``0`` unread messages
are all real. Missing data has its own reporting path, an error status with
no value, so a screen must not treat zero as "unknown".


Two Paths, One Module
---------------------

System Value has two independent paths. They share the key catalog and the
adapter registry, and nothing else.

The read path is request/response: the application asks, the port answers
exactly once. The change path is fire-and-forget in the other direction: the
driver reports, every subscriber that asked for that key hears about it.

A screen commonly uses both. It subscribes once for updates and never reads
again, or it subscribes and then reads once to fill the first frame, because
a subscription carries changes only and would otherwise leave the screen
blank until something happens.


The Queue Is Not the Transport
------------------------------

The obvious implementation is to put the value in a GUI message and let the
GUI thread pull it out. HoneyGUI does not do that, because a message queue is
bounded and a dropped message is a lost value.

Instead, both paths keep their own linked list:

- ``gui_sysval_respond()`` moves the answered request to a **done list**.
- ``gui_sysval_publish()`` copies the value into a **notify list**.

The data never travels through a message slot. A one-slot queue is used only
as a lock, and a single GUI message is sent only to wake an idle GUI thread.
If that wake message is dropped because the queue is full, the GUI thread is
busy anyway and the server hook picks the list up on the next frame. No value
is lost either way.

``sysval_dispatch()`` runs from a server hook every frame, and from the wake
message when the thread is idle. It is the only place a callback is ever
called.


Serial Numbers
--------------

Every publication gets a serial number. A subscription records the serial
that was current when it was created, and is only offered later ones. Four
behaviours follow directly from that one rule.

**A change published before subscribing is not replayed.** The subscription
starts above it. A change that is still sitting in the notify list when the
subscription is created is above it too, and is delivered. The boundary is
"has the GUI thread already seen this", not "when did the call happen", which
is the only definition that matches what the user sees on screen.

**A burst collapses into one delivery.** A second publish for a key that is
still queued replaces the value in place instead of appending. The subscriber
is called once, with the latest value. A burst is therefore cheap and the
display never shows an intermediate state it cannot render.

**A value already on screen is dropped.** Before delivering, the GUI thread
compares the value with the last one it delivered for that key. If they are
equal, nothing is called. This is what keeps a writable key from feeding its
own change notification back into the widget that performed the write: a
volume slider that writes on drag and also subscribes to the volume does not
fight itself.

**Order is only guaranteed per key.** Two keys published in the same frame
may be delivered in either order. The system promises each key's latest
value, not a global sequence, which is what lets it coalesce.


Stale Reads
-----------

Reading once after subscribing has a race that a naive implementation hides.
The port may be slow to answer; meanwhile a change is published and
delivered. When the answer finally arrives it carries the *older* value, and
applying it makes the screen jump backwards.

``gui_sysval_get_request()`` records the serial that was current when the read
started. At dispatch, if a newer value for that key has already been
delivered, the answer is reported as ``GUI_SYSVAL_STATUS_STALE`` instead of
``GUI_SYSVAL_STATUS_OK``. The value is still passed, so a caller that has no
subscription can use it, but a screen that also subscribes knows to keep what
it already has.


Threading
---------

The producer side is free: ``gui_sysval_publish()`` and
``gui_sysval_respond()`` may be called from any thread.

The consumer side is not: requests are created with
``gui_sysval_get_request()`` and ``gui_sysval_set_request()``, and
subscriptions are managed with ``gui_sysval_subscribe()`` and
``gui_sysval_unsubscribe()``, all on the GUI thread. This asymmetry is
intentional. A subscription callback updates widgets, and a widget may only
be touched from the GUI thread, so creating the subscription from anywhere
else would only move the mistake further from the cause.

Guarding the shared lists uses the port's own message queue as a one-slot
lock, the same primitive the rest of HoneyGUI uses for memory. On a single
threaded port with no queue the lock degrades to nothing.


Lifetime
--------

A subscription lives until it is cancelled. Nothing can tell that the
``user_data`` it points at has been freed, so a callback that touches a
widget must be cancelled before that widget is destroyed.

``gui_sysval_unsubscribe()`` is safe to call from inside a callback:
delivery to that subscriber stops immediately and the internal record is
released when dispatch finishes. Calling it twice with the same handle is not
supported; the handle is invalid once the call returns.
