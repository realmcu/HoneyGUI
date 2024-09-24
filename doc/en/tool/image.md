# Image Convert Tool

## Image Format Conversion
Convert pictures in various formats into RGB raw pictures
* Open the converter. Please refer to this section for the download link of the image conversion tool: [Tool](./index.md) 
* The operation steps and detailed instructions are as follows:

<br/>
<div style="text-align: center"><img width= "700" src="https://foruda.gitee.com/images/1725345166232084759/b1982ac0_13671125.png"></div>
<br/>

1. Open the image folder.
2. Open the settings.
3. Select the output folder.
4. Check the color information header.
5. Choose the image format to be configured.
6. Set the conversion parameters.
7. Convert.

### Configuration
- Color head : BeeGUI wants this head to display.
- Big-endian : Whether the input image is big-endian.
- Compress : Enable image compression.
- MixAlphaChannel Flag : Whether to mix alpha channel to rgb when converting rgba to rgb or rgb565.
- scanMode : Select whether the scan direction is horizontal or vertical, BeeGUI only wants horizontal.
- Color Space : Select colorSpace (RGB565, RGBA, BINARY...), BeeGUI can display all of them.

### Color Space

- RGB565: Colorful but with low rendering cost and storage. 2 bytes per pixel.

|  Red |  Green |  Blue |
|----------|------------|-----------|
|  5bit|  6bit  |  5bit |

- ARGB8565: 24-bit ARGB mode.

|  Opacity |  Red |  Green |  Blue |
|----------|------------|-----------|--------------|
|  8bit|  5bit  |  6bit |    5bit  |

- RTKRGAB: 16-bit RGAB mode.

|  Red |  Green |  Opacity |  Blue |
|----------|------------|-----------|--------------|
|  5bit|  5bit  |  1bit |    5bit  |

- RGB : 24-bit RGB mode. 3 bytes per pixel.

|  Red |  Green |  Blue |
|----------|------------|-----------|
|  8bit|  8bit  |  8bit |

- ARGB : True color with opacity. Enhance the display quality with transparency effects. 4 bytes per pixel.

|  Opacity |  Red |  Green |  Blue |
|----------|------------|-----------|--------------|
|  8bit|  8bit  |  8bit |    8bit  |

- BINARY : Use one bit for a pixel.

- RTKARGB8565 : RTK 24-bit ARGB8565 mode.

|  Opacity | Opacity |... |  Red |  Green |  Blue | Red |  Green |  Blue | ... |
|---|---|---|---|---|---|---|---|---|---|
|  8bit | 8bit | ... |  5bit  |  6bit |    5bit  | 5bit  |  6bit | 5bit |...|


## Output Files
The following files will be generated.
<br/>
<div style="text-align: center"><img width= "400" src="https://foruda.gitee.com/images/1718776957325643948/803a9ab3_13408154.png"></div>
<br/>

By using the image conversion tool, we can convert the three `JPG files a, b, and c` into three `binary files a, b, and c`.

Place the `binary file` into the `root` folder of the packaging directory. For the packaging process, please refer to the [Pack Tool](./packing.md) section.

<div style="text-align: center"><img width= "230" src="https://foruda.gitee.com/images/1718786296198222012/e213af1a_13408154.png"></div>
<br/>



























