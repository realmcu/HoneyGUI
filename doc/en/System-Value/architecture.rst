.. _System-Value-Architecture_EN:

==========================
Architecture
==========================

This chapter follows one value from the moment a driver notices it changed
until the widget that shows it has been updated, and explains why each step
looks the way it does.


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
