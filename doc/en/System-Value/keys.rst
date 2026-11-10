.. _System-Value-Keys_EN:

==========================
Key Catalog
==========================

A key is a lower-case ASCII path that names one piece of state. The catalog
is defined in ``realgui/sysval/gui_sysval_keys.h``, which is the reference:
each key there carries its type, access, unit and range as a comment block.
This chapter explains how to read those definitions and how to add keys of
your own.


Reading a Definition
--------------------

.. literalinclude:: ../../../realgui/sysval/gui_sysval_keys.h
   :language: c
   :start-at:  * Latest valid heart-rate measurement.
   :end-before: #define GUI_SYSVAL_KEY_HEALTH_HEART_RATE
   :prepend: /**
   :append: */

Use the ``GUI_SYSVAL_KEY_*`` macro, never the string. The macro is what makes
a typo a compile error instead of a request that silently fails at runtime.


Groups in the Catalog
---------------------

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
-----------------------

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
--------------------

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
-------

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
