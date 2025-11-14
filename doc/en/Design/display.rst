==================
Display Subsystem
==================

The workflow of the display system is very complex, and there are different processes for different :term:`UI` frameworks and different widgets.

Display Workflow
-----------------
As the most commonly used UI input source, the image is used here as an example to illustrate the complete workflow from the raw image to the screen, as shown in the figure below.

Due to the different hardware configurations of various types of :term:`IC`, the RTL8772G chip platform is chosen here, and RealUI is used as the UI system to explain the image display workflow.

.. figure:: https://foruda.gitee.com/images/1703054242639518699/13e78a92_9325830.png
   :align: center
   :width: 200px

   Image Display Work Flow

Flash File System
~~~~~~~~~~~~~~~~~~
The original image is converted into a file in a special format and then downloaded into flash. Flash is configured with a pseudo-file system that provides image index information to the widget layer.After the simple migration of the file system is complete, the standard file system can be used.

Please read the :ref:`Image Convert Tool` section for more information about image conversion.

UI Widget
~~~~~~~~~~
The image widget is the most basic UI widget used to display images. There are many widgets in the UI system that draw special images based on image widgets.

Here, the image widget loads the image data and reads the image information. It combines the UI design and the behavior of the widget layer to provide image rendering requirements for the acceleration layer. Such as image movement, image reduction and enlargement, image rotation and so on.

In addition, some of the hardware supports powerful :term:`GPU` that can draw widgets with complex transformation effects, such as cube widget, color wheel widget, and so on.

Acceleration Layer
~~~~~~~~~~~~~~~~~~~
The function of the acceleration layer is to accelerate the UI image drawing process, which is divided into hardware acceleration and software acceleration. In general, hardware acceleration is significantly better than software acceleration, but which one to use depends on the hardware environment in which the UI system is deployed. In addition, different hardware accelerators, also known as graphics processing units (GPU), also have different capabilities. The accelerator receives the drawing task assigned by the UI widget and transfers the completed image to the display buffer.

Buffer
~~~~~~~
In most embedded systems where :term:`RAM` is limited, RealUI uses a chunked rendering mechanism that requires a display buffer. The display buffer stores the image drawing results of the accelerator and the drawing results of other non-accelerating widgets, and the data are transferred to the frame buffer through :term:`DMA`.

Single frame draw mode can be used when the available RAM can accommodate a full frame, in which case a full frame buffer is used instead of a display buffer.

After configuring the display controller, it will transfer the frame buffer data to the screen, at this time, the screen will display the UI interface.
