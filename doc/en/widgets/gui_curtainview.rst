.. _Curtainview:

============
Curtainview
============

The curtainview widget can accommodate multiple curtain widgets. On the created curtain widgets, you can place application-related information such as application menus or other pertinent content. This widget can be extended as needed and customized according to requirements.
As illustrated in figure, the curtainview is composed of five curtains, arranged in the following five directions: top, bottom, right, left, and center.

.. figure:: https://foruda.gitee.com/images/1727088694333469506/7c243250_13408154.png
   :align: center
   :width: 400px

   Curtain Distribution


Usage
-----
Create Curtainview Widget
~~~~~~~~~~~~~~~~~~~~~~~~~~
The function :cpp:any:`gui_curtainview_create` is utilized to create a container that is capable of housing the curtain widget.

Callback
~~~~~~~~~
Once the curtain extension is finished, you can configure certain callback events through the use of :cpp:any:`gui_curtainview_set_done_cb`.

Example
--------
Please refer to the section: :ref:`Curtain`


API
-----

.. doxygenfile:: gui_curtainview.h

