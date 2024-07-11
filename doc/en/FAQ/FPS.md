# How To Increase FPS FAQ

## Pixel format

Using RGBA/RGB images can get great display effects, but if the FPS is low, then you can use RGB565 format image resources, sacrifice a little effect to get a FPS boost.

## Hardware Acceleration

Use hardware acceleration to render images instead of software acceleration whenever possible.Different chip models may have different GPU, please refer to the guidance document in the SDK for details.

## Data transmission speed

The speed of data transfer will also affect the drawing speed of the HoneyGUI, so ensure the bandwidth and frequency of the memory.

## UI design

Reducing complexity in the UI, as well as the number and size of images in a single interface, can increase the frame rate. Make sure that every pixel of the image data that needs to be loaded is useful.

## Image Compression

Almost all image compression reduces the refresh rate of the UI, so avoid using compressed images if the memory size is sufficient.

## Font

### Custom Binary Files

* Use multiples of 8 for font size whenever possible.
* When the file contains several hundred characters, indexMethod should be set to 0 when creating the font file.

### Standard TTF Files

* Using TTF files to display text is significantly slower than using BMP.
* TTF files can be clipped through an open source solution.
