.. _System-Value-Porting_EN:

==========================
Porting and Adapters
==========================

Bringing System Value to a new board is a scoping exercise first and a coding
exercise second. Most boards support a fraction of the catalog, and that is
the expected outcome.


Decide What the Board Owns
--------------------------

Walk the catalog in :ref:`System-Value-Keys_EN` and sort every key into one
of three groups.

.. list-table:: Scoping a new board
   :header-rows: 1
   :widths: 18 40 42

   * - Group
     - Meaning
     - What to write
   * - Supported
     - The board has this state and can answer for it now or soon.
     - An adapter with at least one callback.
   * - Absent
     - The board has no such capability, for example no altimeter.
     - Nothing. Do not register a stub.
   * - Later
     - The hardware exists but the driver is not ready.
     - Nothing yet. Add the adapter when the driver lands.

Do not register an adapter that always reports
``GUI_SYSVAL_STATUS_ERROR``. A key that is unknown and a key that is broken
look the same to the application, and an application author will spend time
looking for the bug. Not registering is the honest answer.


The Adapter Contract
--------------------

An adapter is the only place a key meets a platform. Writing one is three
fields and two optional callbacks:

.. literalinclude:: ../../../realgui/sysval/gui_sysval_adapter.h
   :language: c
   :start-at: typedef struct gui_sysval_adapter
   :end-before: } gui_sysval_adapter_t;

The fields map directly onto the catalog:

- ``key`` and ``type`` must match the catalog entry exactly. Registration is
  refused if the type is not one of the three known types, or if the key is
  already registered.
- Leave ``get_request`` at ``NULL`` for a write-only key, and ``set_request``
  at ``NULL`` for a read-only key. Leaving both at ``NULL`` is refused.
- ``next`` is set by ``gui_sysval_adapter_register()``.

The adapter struct and the key string must stay valid for the life of the
program, so a file-scope ``static`` object is the normal choice.


Answering a Request
-------------------

Every request must be answered exactly once with ``gui_sysval_respond()``.
When the value is at hand, answer inside the callback:

.. literalinclude:: ../../../simulator/win32/port/realgui_port/sysval/gui_port_sysval_power.c
   :language: c
   :start-at: static void port_sysval_battery_level_get
   :end-before: static void port_sysval_battery_charging_get

This is still asynchronous from the application's point of view. The
response is queued and the application's callback runs later on the GUI
thread, never before ``gui_sysval_get_request()`` returns.

When the answer needs a round trip, keep the request and answer from wherever
the answer arrives. The following ``get_request`` only saves the request and
lets a worker answer it later:

.. literalinclude:: ../../../simulator/win32/port/realgui_port/sysval/gui_port_sysval_async.c
   :language: c
   :start-at: static void port_sysval_async_get
   :end-before: static gui_sysval_adapter_t async_value_adapter

The request pointer stays valid until it is answered. Answering it twice, or
dropping it, is undefined behaviour.


Status Codes
------------

The status the adapter reports is what the application sees.

.. list-table::
   :header-rows: 1
   :widths: 32 68

   * - Status
     - When the adapter reports it
   * - ``GUI_SYSVAL_STATUS_OK``
     - The value is valid right now.
   * - ``GUI_SYSVAL_STATUS_INVALID_VALUE``
     - A write of the right type, but the platform will not accept this
       value. The ``Range`` field of the key says what is acceptable, but the
       platform may accept only part of it.
   * - ``GUI_SYSVAL_STATUS_BUSY``
     - The backend cannot take the request now. For example a read that
       would need a radio that is already in use.
   * - ``GUI_SYSVAL_STATUS_TIMEOUT``
     - The backend gave up waiting.
   * - ``GUI_SYSVAL_STATUS_ERROR``
     - Any other failure, including a supported key with no current value.

An adapter never reports success with a value of the wrong type. HoneyGUI
checks this and converts a mismatch into ``GUI_SYSVAL_STATUS_ERROR``. The
check exists to catch the mistake, not to make it acceptable.


Registering
-----------

Register adapters during device init, before any application code runs:

.. literalinclude:: ../../../simulator/win32/port/realgui_port/sysval/gui_port_sysval_power.c
   :language: c
   :start-at: static gui_sysval_adapter_t battery_level_adapter
   :end-before: static gui_sysval_adapter_t battery_charging_adapter

.. literalinclude:: ../../../simulator/win32/port/realgui_port/sysval/gui_port_sysval_power.c
   :language: c
   :start-at: static int port_sysval_power_init
   :end-before: GUI_INIT_DEVICE_EXPORT

``GUI_INIT_DEVICE_EXPORT`` runs before ``GUI_INIT_APP_EXPORT``, so an
application can subscribe to a key in its own init function and be sure the
adapter is already there.


Choosing a Status for Missing Data
----------------------------------

A supported key with nothing to report is a normal situation, not an error to
work around. Report ``GUI_SYSVAL_STATUS_ERROR`` with no value and let the
screen decide what to draw. Do not invent a sentinel such as ``0``, ``-1`` or
an empty string to mean "no data": zero is a real battery level and a real
step count, and the application cannot tell the two apart.


Pick the Shape per Key
----------------------

.. list-table:: Which callbacks a key needs
   :header-rows: 1
   :widths: 25 22 53

   * - Kind of state
     - Callbacks
     - Notes
   * - Sampled when asked only
     - ``get_request``
     - Answer inside the callback from the current hardware reading.
   * - Sampled and changing on its own
     - ``get_request`` + publish
     - The common case. Every change publishes; the read answers from the
       same variable.
   * - Needs a round trip
     - ``get_request``
     - Keep the request and answer from the worker.
   * - Settable by the application
     - add ``set_request``
     - Apply the change, then publish it, then answer the request.

The third row is the one that causes trouble when it is done halfway: an
adapter whose read path returns one variable and whose producer publishes a
different one will hand the application two different answers for the same
key. Keep them in a single owner.


A Typical Port Module
---------------------

The simulator power port is the smallest complete implementation of this
shape. It owns the state and answers reads from it:

.. literalinclude:: ../../../simulator/win32/port/realgui_port/sysval/gui_port_sysval_power.c
   :language: c
   :start-at: static int32_t battery_level
   :end-before: static gui_sysval_adapter_t battery_level_adapter

When the value changes on its own, add a publish function on top of that and
route every write through it:

.. code-block:: c

   /* Every write goes through here, so a read and a publish always agree. */
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

   /* Called from the fuel gauge driver, any thread. */
   void port_fuel_gauge_event(int32_t level)
   {
       battery_level_set(level);
   }

Note that ``battery_level_set()`` returns early when the value did not
change. The dispatch path already filters an unchanged value, but returning
early avoids the work entirely, which matters for a value that a driver may
report once per second.

Note also that ``gui_sysval_publish()`` can fail, when memory is exhausted.
The notification is not retried, so a port that cares should look at the
return value. The next change publishes again, so a single failure is
recovered by the next event, not by retrying the failed one.


Waking the GUI Thread
---------------------

A port does not have to do anything special to get a publish delivered.
Publishing from an interrupt or a driver task is enough: the GUI thread is
woken when a key that has subscribers changes, and the server hook picks the
value up on the next frame if the wake message is dropped.

A key with no subscribers at all does not wake the thread and does not queue
a value. That is why an application can publish a rapidly changing key, such
as a step counter, without paying for it while the relevant screen is not on
display.


Testing a Port
--------------

Two checks catch most mistakes before an application sees them.

**The read and the publish agree.** Subscribe to the key, read it once, and
compare. They should be equal when no change happened in between. If they
differ, the module has two owners for one value.

**Every published value is readable.** Publish a value, then read the key.
The read must return it. An adapter that answers from a hardware register
while the producer publishes a cached copy fails this check.

Both are simple enough to keep as a debug command on the device.
