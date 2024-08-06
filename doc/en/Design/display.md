# Display sub-system

The workflow of the display system is very complex, and there are different processes for different UI frameworks and different widgets.

## Display Workflow

As the most commonly used UI input source, the image is used here as an example to illustrate the complete workflow from the raw image to the screen, as shown in the figure below.

Because the hardware configuration of different types of [IC](/Glossary.rst#term-IC) is different, the RTL8772G chip platform is chosen, and RealUI is used as the UI system to explain the image display workflow.

</details></br><center>
<img src="https://foruda.gitee.com/images/1703054151955593791/7444de3c_9325830.png",alt="Image Display Work Flow"/>
</center></br>

### Flash File System

The original image is converted into a file in a special format and then downloaded into flash. Flash is configured with a pseudo-file system that provides image index information to the control layer.

After the simple migration of the file system is complete, the standard file system can be used.

Read the [GUI](/Glossary.rst#term-GUI) online documentation for more information about image conversion tools.

### UI Widget

The image widget is the most basic UI widget used to display images. There are many widgets in the UI system that draw special images based on image widgets.

Here, the image widget loads the image data and reads the image information, which, combined with the UI design and the behavior of the widget layer, provides the image rendering requirements for the acceleration layer. Such as image movement, image reduction and enlargement, image rotation and so on.

In addition, some of the hardware supports powerful [GPU](/Glossary.rst#term-GPU) that can draw widgets with complex transformation effects, such as cube widget, color wheel widget, and so on.

### Acceleration

The function of the acceleration layer is to accelerate the UI image drawing process, which is divided into hardware acceleration and software acceleration.

In general, hardware acceleration is significantly better than software acceleration, but which one to use depends on the hardware environment in which the UI system is deployed. In addition, different hardware accelerators, also known as graphics processing units (GPU), also have different capabilities.

The accelerator receives the drawing task assigned by the UI widget and transfers the completed image to the display buffer.

### Buffer

In most embedded systems where [RAM](/Glossary.rst#term-RAM) is limited, RealUI uses a chunked rendering mechanism that requires a display buffer.

The display buffer stores the image drawing results of the accelerator and the drawing results of other non-accelerating controls, and the data is transferred to the frame buffer through [DMA](/Glossary.rst#term-DMA).

Single frame draw mode can be used when the available RAM can accommodate a full frame, in which case a full frame buffer is used instead of a display buffer.

After configuring display controller, it will transfer the frame buffer data to the screen, at this time, the screen will display the UI interface.
