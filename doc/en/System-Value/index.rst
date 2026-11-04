.. _System-Value_EN:

=================
System Value
=================

An application almost never owns all the state it displays: the battery
percentage belongs to the fuel gauge, the current time to the RTC or the
phone, the unread count to a notification service. If every screen calls the
driver itself, porting becomes a nightmare. Every new board has to satisfy
every invention, the application ends up depending on a driver header, and a
value that changes on its own can only be polled. This chapter describes the
one layer that collects those problems in one place: the application sees
standard keys, and everything platform specific stays in the port.

A screen and the platform talk in two directions only. Upward is what the
application starts: a screen calls ``gui_sysval_get_request()`` to ask for a
value and ``gui_sysval_set_request()`` to ask for a change, both returning an
ID at once, and the real answer is handed by the port to the framework and
from there back to the application on the GUI thread. Downward is what the
platform starts: a driver notices a value changed and reports it with
``gui_sysval_publish()``, and the framework passes it to every screen that
subscribed to that key with ``gui_sysval_subscribe()``. Neither path requires
the application to know which driver serves it.

Every participant in the system is one of three things.

.. list-table::
   :header-rows: 1
   :widths: 16 28 56

   * - Role
     - Who
     - Responsibility
   * - Consumer
     - Application, widget
     - Knows key names. Reads, writes and subscribes, and never sees a
       driver.
   * - Producer
     - Driver, sensor task, service
     - Owns the value. Publishes a new value when it changes.
   * - Adapter
     - Port layer
     - Implements a key on this platform: answers reads and writes.

One module often plays the last two at once. A fuel gauge port registers an
adapter so the application can read the level on demand, and publishes every
time the level actually changes. Keeping both in one place is what makes a
read and a subscription give the same answer.

The type, access, unit and range of each key are fixed per entry by the
catalog in :doc:`keys`. Two rules make one catalog usable across products.
Declaring a key does not register it: an adapter registers only what its
platform supports and every other key answers
``GUI_SYSVAL_REQUEST_INVALID``, so a screen can probe for an optional
capability without a build-time flag. And missing data is not a value: a key
with no current reading answers ``GUI_SYSVAL_STATUS_ERROR`` with no value, so
zero always means a real zero.

The sections below take these in turn: the full key catalog, how a change
travels from a driver thread to a callback, how a port implements a key, how
an application reads and subscribes, and working example code.


.. toctree::
    :maxdepth: 2

    Key Catalog <keys>
    Architecture <architecture>
    Adapters <adapter>
    Consumers <consumer>
    Porting <porting>
    Examples <example>
