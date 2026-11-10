.. _System-Value-Adapter_EN:

==========================
Adapters
==========================

An adapter is the only place a key meets a platform. Writing one is three
fields and two optional callbacks.


The Contract
------------

.. literalinclude:: ../../../realgui/sysval/gui_sysval_adapter.h
   :language: c
   :start-at: typedef struct gui_sysval_adapter
   :end-before: } gui_sysval_adapter_t;

The fields map directly onto the catalog:

- ``key`` and ``type`` must match the catalog entry exactly. Registration is
  refused if the type is not one of the three known types, or if the key is
  already registered.
- Leave ``get_request`` at ``NULL`` for a write-only key, and ``set_request``
  at ``NULL`` for a read-only key. Leaving both at ``NULL`` is refused: an
  adapter that can do neither would only make the key look supported while
  every request fails.
- ``next`` is set by ``gui_sysval_adapter_register()``.

The adapter struct and the key string must stay valid for the life of the
program, so a file-scope ``static`` object is the normal choice.


Answering a Request
-------------------

Every request must be answered exactly once with ``gui_sysval_respond()``.
There are two shapes, and an adapter picks one per request.

**Answer immediately.** When the value is at hand, answer inside the callback:

.. literalinclude:: ../../../simulator/win32/port/realgui_port/sysval/gui_port_sysval_power.c
   :language: c
   :start-at: static void port_sysval_battery_level_get
   :end-before: static void port_sysval_battery_charging_get

This is still asynchronous from the application's point of view. The
response is queued and the application's callback runs later on the GUI
thread, never before ``gui_sysval_get_request()`` returns.

**Answer later.** When the answer needs a round trip, keep the request and
answer from wherever the answer arrives:

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
checks this and converts a mismatch into ``GUI_SYSVAL_STATUS_ERROR`` rather
than hand the application a union it would read through the wrong member, but
the check exists to catch the mistake, not to make it acceptable.


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

A port that does not support a key simply does not register it. That is the
whole mechanism behind optional capability: no build flags, no weak symbols,
just a key that answers ``GUI_SYSVAL_REQUEST_INVALID``.


Choosing a Status for Missing Data
----------------------------------

A supported key with nothing to report is a normal situation, not an error to
work around. Report ``GUI_SYSVAL_STATUS_ERROR`` with no value and let the
screen decide what to draw. Do not invent a sentinel such as ``0``,
``-1`` or an empty string to mean "no data": zero is a real battery level and
a real step count, and the application cannot tell the two apart.
