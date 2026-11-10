.. _System-Value-Porting_EN:

==========================
Porting
==========================

Bringing System Value to a new board is a scoping exercise first and a coding
exercise second. Most boards support a fraction of the catalog, and that is
the expected outcome.


Decide What the Board Owns
--------------------------

Walk the catalog in :doc:`keys` and sort every key into one of three groups.

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
