# FAQ
## Development environment FAQ
### Simulator in VSCode
If you encounter problems the first time you run the simulator in VSCode, please check the following configurations in your development environment:
#### Installing the appropriate version of the toolchain
Simulator in VSCode using MinGW toolchain (refer to section 3.1 in Get Started), and MinGW version 8.1.0 is recommended, which can be accessed from [MinGW v8.1.0 Download](https://sourceforge.net/projects/mingw-w64/files/Toolchains%20targetting%20Win64/Personal%20Builds/mingw-builds/8.1.0/threads-posix/sjlj/x86_64-8.1.0-release-posix-sjlj-rt_v6-rev0.7z). There is no guarantee that all later versions of the MinGW will function properly.
> Known issues: VSCode currently does not support gdb version higher than v9.2.0 in MinGW. (gdb v8.1 is used in MinGW v8.1.0, which is recommended.)

#### Adding toolchain to system environment variables
Make sure `C:/mingw64/bin` is already added to system environment variable `Path`.

## Porting FAQ
###  User data
User Data bin image generation need to consider user data address in flash_map.h. Normally the address in generate script is consistent with user data address in flash_map.h,
and if user data bin need to add image header due to mppgtool requirement, the generate script address must increase by image header size.
###  JS malloc heap
JS(javascript) is include in GUI module, the heap space JS used maybe malloc failed by resource limition, so this heap space could relocate on psram if SoC support psram feture, the specific
infomation could see api `void *context_alloc(size_t size, void *cb_data_p)`.
###  Feed watch dog
GUI task not support feed watch dog, so app should do this in hook function registered by APP and used by GUI. The registered function is `void gui_task_ext_execution_sethook(void (*hook)(void))`.
###  Magic image create
If SoC not support GPU, while creating magic image, create api should change to normal image create api, the specific info could see `gui_magic_img_t *xml_gui_img_creat_from_mem(void *parent,  const char *name, void *addr,int16_t x, int16_t y)`.
###  Not support FPU
If SoC not support FPU, some header and code should not include by macro, example as RTL8763EP.
###  File system
SoC need to read image and font resource from flash by file system which should set a start address that is consistent with address in User Data generation script, GUI has supplied the related file which is 'romfs.c' where the start address is `ROMFS_ADDR`.
###  Realloc function
GUI need this function, but it would bring hardfault if using C library api. APP could use malloc api instead of realloc.
###  Flash setting
Flash speed mode should set to 4 bit mode; flash clock should set to a higher value based on chip capabilities.
###  CPU frequence
CPU frequence should set to a higher value based on chip capabilities.
###  SCONS Version
Specific scons version is required, please use the `pip install scons==4.4.0` command to download.
## Specification FAQ
###  Graphics
| Platform  | 8762D  |8772F   |87X2D |PC   |8763E|
|---|---|---|---|---|---|
| RGB565  |  Y | Y  | Y  | Y  | Y  |
|  RGB888 | N  | Y  |  Y |  Y | N  |
|  ARGB8888 |  N | Y  |  Y | Y  | N  |
|  SVG | N  | Y   |  N |  Y  | N  |
| TTF | N  | Y   |  N |  Y  | N  |
| DOT font  |  Y | Y  | Y  | Y  |Y  |
|  Vector Graphics | N  | Y   |  N |  Y  |N |
|  Linear gradient | N  | Y   |  N |  Y  |N |
|  Radial gradient | N  | N   |  N |  Y  |N |

###  Memory usage
#### 8772F demo

```eval_rst
.. raw:: html

    <iframe src="https://drive.google.com/file/d/1qpLBVJRt2gnRR_cpuXc9L06VjqwvpzS0/preview" width="640" height="480" allow="autoplay"></iframe>
```
The memory consumption statistics of this demo are as follows
| Module | Cost  |
|---|---|
| Widget  |  14.56KB |
| Framebuffer  |  screenWidth*pixelBytes*Lines |
| Thread stack  |  10KB |

#### Widget memory usage
| Widget | Memory(Byte)  |
|---|---|
| RGB565  |  Y |
|win	|104|
|img	|248|
|switch	|384|
|text	|128|
|page 	|128|
|tabview	|120|
|tab	|104|
|curtainview	|152|
|curtain	|104|
|button	|408|
|seekbar	|416|

## How To Increase FPS FAQ

### Pixel format

Using RGBA/RGB images can get great display effects, but if the FPS is low, then you can use RGB565 format image resources, sacrifice a little effect to get a FPS boost.

### Hardware Acceleration

Use hardware acceleration to render images instead of software acceleration whenever possible.Different chip models may have different GPU, please refer to the guidance document in the SDK for details.

### Data transmission speed

The speed of data transfer will also affect the drawing speed of the HoneyGUI, so ensure the bandwidth and frequency of the memory.

### UI design

Reducing complexity in the UI, as well as the number and size of images in a single interface, can increase the frame rate. Make sure that every pixel of the image data that needs to be loaded is useful.

### Image Compression

Almost all image compression reduces the refresh rate of the UI, so avoid using compressed images if the memory size is sufficient.

### Font

#### Custom Binary Files

* Use multiples of 8 for font size whenever possible.
* When the file contains several hundred characters, indexMethod should be set to 0 when creating the font file.

#### Standard TTF Files

* Using TTF files to display text is significantly slower than using BMP.
* TTF files can be clipped through an open source solution.
