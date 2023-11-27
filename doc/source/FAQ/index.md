# FAQ
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
|  RGBA8888 |  N | Y  |  Y | Y  | N  |
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
