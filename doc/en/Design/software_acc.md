# Software Accelerate

## Overall Flow Chart
The flowchart depicts the image resource processing flow accelerated by software. When processing images, different processing methods are selected based on the compression status and type of image:

- **Cover**: Write the source image data directly to the corresponding position in the frame buffer. Do not perform any processing, just overwrite it.
- **Bypass**: Write the source image data directly to the corresponding position in the frame buffer. Bypass mode is incapable of handling the transparency of images. It applies a global opacity value to the entire image, thereby affecting the overall transparency. When it comes to creating transparency effects, bypass mode is more space-efficient compared to source_over mode.
- **Filter black**: The filtering technique effectively sifts out pixel data with a value of zero from the originating image data, which essentially means that black pixels are precluded from being inscribed into the frame buffer. This mechanism induces much swifter refresh dynamics. Pixels of any color other than black undergo the standard processing method and are duly recorded into the frame buffer.
- **Source_over**: A blending method that combines image color data and frame buffer pixel color data to calculate the final color based on the opacity_value value `Sa`, and writes it to the corresponding location in the frame buffer. The formula is `((255 - Sa) * D + Sa * S) / 255)`, where `Sa` is the opacity_value of the original image, `D` is the frame buffer pixel data, and `S` is the source image pixel data.

<div style="text-align: center"><img src ="https://foruda.gitee.com/images/1726135723241666402/b6cb0cbc_13671125.png"/></div><br/>

- The `img_type` can be obtained from the `head` of the image, where the structure of the image head is as follows.

```
typedef struct gui_rgb_data_head
{
    unsigned char scan : 1;
    unsigned char align : 1;
    unsigned char resize: 2; //0-no resize;1-50%(x&y);2-70%;3-80%
    unsigned char compress: 1;
    unsigned char rsvd : 3;
    char type;
    short w;
    short h;
    char version;
    char rsvd2;
} gui_rgb_data_head_t;
```
- The value of `img_type` is depicted in the enum below. If the value is `IMDC_COMPRESS`, it indicates that the image is compressed and enters the `rle` processing flow; otherwise, it enters the `no rle` processing flow.

```
typedef enum
{
    RGB565      = 0, //bit[4:0] for Blue, bit[10:5] for Green, bit[15:11] for Red
    ARGB8565    = 1, //bit[4:0] for Blue, bit[10:5] for Green, bit[15:11] for Red, bit[23:16] for Alpha
    RGB888      = 3, //bit[7:0] for Blue, bit[15:8] for Green, bit[23:16] for Red
    ARGB8888    = 4, //bit[7:0] for Blue, bit[15:8] for Green, bit[23:16] for Red, bit[21:24] for Alpha
    BINARY      = 5,
    ALPHAMASK   = 9,
    BMP         = 11,
    JPEG        = 12,
    PNG         = 13,
    GIF         = 14,
    RTKARGB8565 = 15,
} GUI_FormatType;
```

- Execute the corresponding `blit` process based on different `blend_mode`.
```
typedef enum
{
    IMG_BYPASS_MODE = 0,
    IMG_FILTER_BLACK,
    IMG_SRC_OVER_MODE, //S * Sa + (1 - Sa) * D
    IMG_COVER_MODE,
    IMG_RECT,
} BLEND_MODE_TYPE;
```

- When the image is compressed, it is necessary to obtain the compression header from the address of the compressed data. The `algorithm_type` parameter of this header contains the actual image type. The types of compressed images are described in the `imdc_src_type` struct, which includes three types: `IMDC_SRC_RGB565`, `IMDC_SRC_RGB888`, and `IMDC_SRC_ARGB8888`.
```
typedef struct imdc_file_header
{
    struct
    {
        uint8_t algorithm: 2;
        uint8_t feature_1: 2;
        uint8_t feature_2: 2;
        uint8_t pixel_bytes: 2;
    } algorithm_type;
    uint8_t reserved[3];
    uint32_t raw_pic_width;
    uint32_t raw_pic_height;
} imdc_file_header_t;
```
```
typedef enum
{
    IMDC_SRC_RGB565 = 0x04, // 4,
    IMDC_SRC_RGB888 = 0x44, // 68,
    IMDC_SRC_ARGB8888 = 0x84, // 132,

} imdc_src_type;
```

## Overview No RLE Cover Mode
The following flow describes the `cover mode` process for `No RLE` compressed image. Select a processing method based on the image matrix and the pixel byte of the display device, and write it to the frame buffer.

<div style="text-align: center"><img width= "800" img src ="https://foruda.gitee.com/images/1726135750546602965/e70749d0_13671125.png"/></div><br/>

- If the matrix is an identity matrix, a [blit](/Glossary.rst#term-blit) process without matrix operations is performed; otherwise, a blit process with matrix operations is carried out.
- The `dc_bytes_per_pixel` is pixel bytes of  display device, calculated as `dc->bit_depth >> 3`, where `bit_depth` is the bit depth of the display device. Taking a display device with a bit depth of 24 as an example, its pixel bytes are 3.

### No RLE Cover
The following flowchart describes the process of writing `uncompressed images` to a frame buffer in `cover mode`. Taking the target device image type as RGB565 as an example.

<div style="text-align: center"><img width="400" src ="https://foruda.gitee.com/images/1726135919132573906/4e4cb2fd_13671125.png"/></div><br/>

### No RLE Cover Matrix
The following flowchart describes the process of writing `uncompressed images` to a frame buffer using `cover mode with matrix operations`. Taking the target device image type as RGB565 as an example.

<div style="text-align: center"><img width="550" src ="https://foruda.gitee.com/images/1726135926171202621/00c62b0c_13671125.png"/></div><br/>

## Overview No RLE Bypass Mode

The following flow describes the `bypass mode` process for `No RLE` compressed image. Select a processing method based on the image matrix and the pixel byte of the display device, and write it to the frame buffer.

<div style="text-align: center"><img width= "800" img src ="https://foruda.gitee.com/images/1726135755236374806/661c982d_13671125.png"/></div><br/>

- If the matrix is an identity matrix, a blit process without matrix operations is performed; otherwise, a blit process with matrix operations is carried out.
- The `dc_bytes_per_pixel` is pixel bytes of  display device, calculated as `dc->bit_depth >> 3`, where `bit_depth` is the bit depth of the display device. Taking a display device with a bit depth of 24 as an example, its pixel bytes are 3.

### No RLE Bypass Mode
The following flowchart describes the process of writing `uncompressed images` to a frame buffer in `bypass mode`. Taking the target device image type as RGB565 as an example.

<div style="text-align: center"><img width= "900" img src ="https://foruda.gitee.com/images/1726135914549506931/43192327_13671125.png"/></div><br/>

1. Perform different processing steps based on the `img_type`.
2. Based on the `opacity_value`, execute the corresponding operation to write image pixels into the framebuffer.
    - If the `opacity_value` is `0`, the image is not displayed and the process is break.
    - If the `opacity_value` is `255`, convert the source image pixels to RGB565 format and write them to the frame buffer.
    - If the `opacity_value` is between `0 and 255`, perform an alpha blending operation to blend the source image pixels with the corresponding frame buffer pixels. The blending formula is `((255 - Sa) * D + Sa * S) / 255)`. Write the blended result to the frame buffer.

### No RLE Bypass Matrix
The following flowchart describes the process of writing `uncompressed images` to a frame buffer using `blend mode with matrix operations`. Taking the target device image type as RGB565 as an example.

<div style="text-align: center"><img width= "900" img src ="https://foruda.gitee.com/images/1726135932315321260/e1dc02cc_13671125.png"/></div><br/>

1. Perform different processing steps based on the `img_type`.
2. Perform matrix calculation to map the target area write-in points to image pixels, and obtain the pixel value of the image pixels.
3. Based on the `opacity_value`, execute the corresponding operation to write image pixels into the framebuffer.
    - If the `opacity_value` is `0`, the image is not displayed and the process is break.
    - If the `opacity_value` is `255`, convert the source image pixels to RGB565 format and write them to the frame buffer.
    - If the `opacity_value` is between `0 and 255`, perform an alpha blending operation to blend the source image pixels with the corresponding frame buffer pixels. The blending formula is `((255 - Sa) * D + Sa * S) / 255)`. Write the blended result to the frame buffer.

## Overview No RLE Filter
The following flow describes the `filter mode` process for `No RLE` compressed image. Select a processing method based on the image matrix and the pixel byte of the display device, and write it to the frame buffer.

<div style="text-align: center"><img width= "800" img src ="https://foruda.gitee.com/images/1726135759587667374/d42c3cba_13671125.png"/></div><br/>

### No RLE Filter
The following flowchart describes the process of writing `uncompressed images` to a frame buffer using `filter mode`. Taking the target device image type as RGB565 as an example.

<div style="text-align: center"><img width= "900" img src ="https://foruda.gitee.com/images/1726135936809407977/178a3356_13671125.png"/></div><br/>

1. Perform different processing steps based on the `img_type`.
2. If the pixel value is 0, skip the processing; otherwise, perform the subsequent writing operation.
3. Based on the `opacity_value`, execute the corresponding operation to write image pixels into the framebuffer.
    - If the `opacity_value` is `0`, the image is not displayed and the process is break.
    - If the `opacity_value` is `255`, convert the source image pixels to RGB565 format and write them to the frame buffer.
    - If the `opacity_value` is between `0 and 255`, perform an alpha blending operation to blend the source image pixels with the corresponding frame buffer pixels. The blending formula is `((255 - Sa) * D + Sa * S) / 255)`. Write the blended result to the frame buffer.

### No RLE Filter Matrix
The following flowchart describes the process of writing `uncompressed images` to a frame buffer using `filter mode with matrix operations`. Taking the target device image type as RGB565 as an example.

<div style="text-align: center"><img width= "900" img src ="https://foruda.gitee.com/images/1726135941645383326/65173b6c_13671125.png"/></div><br/>

1. Perform different processing steps based on the `img_type`.
2. Perform matrix calculation to map the target area write-in points to image pixels, and obtain the pixel value of the image pixels.
3. If the pixel value is 0, skip the processing; otherwise, perform the subsequent writing operation.
4. Based on the `opacity_value`, execute the corresponding operation to write image pixels into the framebuffer.
    - If the `opacity_value` is `0`, the image is not displayed and the process is break.
    - If the `opacity_value` is `255`, convert the source image pixels to RGB565 format and write them to the frame buffer.
    - If the `opacity_value` is between `0 and 255`, perform an alpha blending operation to blend the source image pixels with the corresponding frame buffer pixels. The blending formula is `((255 - Sa) * D + Sa * S) / 255)`. Write the blended result to the frame buffer.

## Overview No RLE Source_over
The following flow describes the `source_over mode` process for `No RLE` compressed image. Select a processing method based on the image matrix and the pixel byte of the display device, and write it to the frame buffer.

<div style="text-align: center"><img width= "1000" img src ="https://foruda.gitee.com/images/1726135811742209771/c8bad88f_13671125.png"/></div><br/>

### No RLE Alpha No Matrix
The following flowchart describes the process of writing `uncompressed images` to a frame buffer using `source_over mode`. Taking the target device image type as RGB565 and the source image type as RGB565 as an example.

<div style="text-align: center"><img width="500" src ="https://foruda.gitee.com/images/1726135946825496906/bafaabe5_13671125.png"/></div><br/>

Based on the `opacity_value`, execute the corresponding operation to write image pixels into the framebuffer.
  - If the `opacity_value` is `0`, the image is not displayed and the process is break.
  - If the `opacity_value` is `255`, convert the source image pixels to RGB565 format and write them to the frame buffer.
  - If the `opacity_value` is between `0 and 255`, perform `do_blending_acc_2_rgb565_opacity` to blend the source image pixels with the corresponding frame buffer pixels. Write the blended result to the frame buffer.

### No RLE Alpha Matrix
The following flowchart describes the process of writing `uncompressed images` to a frame buffer using `source_over mode with matrix operations`. Taking the target device image type as RGB565 and the source image type as RGB565 as an example.

<div style="text-align: center"><img width="500" src ="https://foruda.gitee.com/images/1726135953438894385/7422d479_13671125.png"/></div><br/>

1. Perform matrix calculation to map the target area write-in points to image pixels, and obtain the pixel value of the image pixels.
2. Based on the `opacity_value`, execute the corresponding operation to write image pixels into the framebuffer.
    - If the `opacity_value` is `0`, the image is not displayed and the process is break.
    - If the `opacity_value` is `255`, convert the source image pixels to RGB565 format and write them to the frame buffer.
    - If the `opacity_value` is between `0 and 255`, perform `do_blending_acc_2_rgb565_opacity` to blend the source image pixels with the corresponding frame buffer pixels. Write the blended result to the frame buffer.

## Overview RLE Cover Mode
The following flow describes the `cover mode` process for `RLE` compressed image. Select a processing method based on the image matrix and the pixel byte of the display device, and write it to the frame buffer.

<div style="text-align: center"><img width= "800" img src ="https://foruda.gitee.com/images/1726135823311485058/2b8f94bf_13671125.png"/></div><br/>

### RLE Cover No Matrix
The following flowchart describes the process of writing `compressed images` to a frame buffer in `cover mode`. Taking the target device image type as RGB565 as an example.

<div style="text-align: center"><img width="700" src ="https://foruda.gitee.com/images/1726136024470398285/83f66a60_13671125.png"/></div><br/>

1. Perform different processing steps based on the `img_type` from the head of compression data.
2. Decompress the compressed image data.
3. Write the pixel result to the frame buffer.

### RLE Cover Matrix
The following flowchart describes the process of writing `compressed images` to a frame buffer in `cover mode with matrix operations`. Taking the target device image type as RGB565 as an example.

<div style="text-align: center"><img width="700" src ="https://foruda.gitee.com/images/1726136030945132846/70e37d28_13671125.png"/></div><br/>

1. Perform different processing steps based on the `img_type` from the head of compression data.
2. Decompress the compressed image data.
3. Perform matrix calculation to map the target area write-in points to image pixels, and obtain the pixel value of the image pixels.
4. Write the pixel result to the frame buffer.

## Overview RLE Bypass Mode
The following flow describes the `bypass mode` process for `RLE` compressed image. Select a processing method based on the image matrix and the pixel byte of the display device, and write it to the frame buffer.

<div style="text-align: center"><img width= "800" img src ="https://foruda.gitee.com/images/1726135816836257523/3224601d_13671125.png"/></div><br/>

### RLE Bypass No Matrix
The following flowchart describes the process of writing `compressed images` to a frame buffer in `bypass mode`. Taking the target device image type as RGB565 as an example.

<div style="text-align: center"><img width= "900" img src ="https://foruda.gitee.com/images/1726136019654689664/085ffd2a_13671125.png"/></div><br/>

1. Perform different processing steps based on the `img_type` from the head of compression data.
2. Decompress the compressed image data.
3. Based on the `opacity_value`, execute the corresponding operation to write image pixels into the framebuffer.
    - If the `opacity_value` is `0`, the image is not displayed and the process is break.
    - If the `opacity_value` is `255`, convert the source image pixels to RGB565 format and write them to the frame buffer.
    - If the `opacity_value` is between `0 and 255`, perform an alpha blending operation to blend the source image pixels with the corresponding frame buffer pixels. The blending formula is `((255 - Sa) * D + Sa * S) / 255)`. Write the blended result to the frame buffer.

### RLE Bypass Matrix
The following flowchart describes the process of writing `compressed images` to a frame buffer in `bypass mode with matrix operations`. Taking the target device image type as RGB565 as an example.

<div style="text-align: center"><img width= "900" img src ="https://foruda.gitee.com/images/1726136035401068218/cd9d0da1_13671125.png"/></div><br/>

1. Perform different processing steps based on the `img_type` from the head of compression data.
2. Decompress the compressed image data.
3. Perform matrix calculation to map the target area write-in points to image pixels, and obtain the pixel value of the image pixels.
4. Based on the `opacity_value`, execute the corresponding operation to write image pixels into the framebuffer.
    - If the `opacity_value` is `0`, the image is not displayed and the process is break.
    - If the `opacity_value` is `255`, convert the source image pixels to RGB565 format and write them to the frame buffer.
    - If the `opacity_value` is between `0 and 255`, perform an alpha blending operation to blend the source image pixels with the corresponding frame buffer pixels. The blending formula is `((255 - Sa) * D + Sa * S) / 255)`. Write the blended result to the frame buffer.

## Overview RLE Filter
The following flow describes the `filter mode` process for `RLE` compressed image. Select a processing method based on the image matrix and the pixel byte of the display device, and write it to the frame buffer.

<div style="text-align: center"><img width= "800" img src ="https://foruda.gitee.com/images/1726135828203631320/cee92853_13671125.png"/></div><br/>

### RLE Filter
The following flowchart describes the process of writing `compressed images` to a frame buffer in `filter mode`. Taking the target device image type as RGB565 as an example.

<div style="text-align: center"><img width= "900" img src ="https://foruda.gitee.com/images/1726136039315952991/89ee16cb_13671125.png"/></div><br/>

1. Perform different processing steps based on the `img_type` from the head of compression data.
2. Decompress the compressed image data.
3. If the pixel value is 0, skip the processing; otherwise, perform the subsequent writing operation.
4. Based on the `opacity_value`, execute the corresponding operation to write image pixels into the framebuffer.
    - If the `opacity_value` is `0`, the image is not displayed and the process is break.
    - If the `opacity_value` is `255`, convert the source image pixels to RGB565 format and write them to the frame buffer.
    - If the `opacity_value` is between `0 and 255`, perform an alpha blending operation to blend the source image pixels with the corresponding frame buffer pixels. The blending formula is `((255 - Sa) * D + Sa * S) / 255)`. Write the blended result to the frame buffer.

### RLE Filter Matrix
The following flowchart describes the process of writing `compressed images` to a frame buffer in `filter mode with matrix operations`. Taking the target device image type as RGB565 as an example.

<div style="text-align: center"><img width= "900" img src ="https://foruda.gitee.com/images/1726136044246530066/3cb24841_13671125.png"/></div><br/>

1. Perform different processing steps based on the `img_type` from the head of compression data.
2. Decompress the compressed image data.
3. Perform matrix calculation to map the target area write-in points to image pixels, and obtain the pixel value of the image pixels.
4. If the pixel value is 0, skip the processing; otherwise, perform the subsequent writing operation.
5. Based on the `opacity_value`, execute the corresponding operation to write image pixels into the framebuffer.
    - If the `opacity_value` is `0`, the image is not displayed and the process is break.
    - If the `opacity_value` is `255`, convert the source image pixels to RGB565 format and write them to the frame buffer.
    - If the `opacity_value` is between `0 and 255`, perform an alpha blending operation to blend the source image pixels with the corresponding frame buffer pixels. The blending formula is `((255 - Sa) * D + Sa * S) / 255)`. Write the blended result to the frame buffer.

## Overview RLE Source_over
The following flow describes the `source_over mode` process for `RLE` compressed image. Select a processing method based on the image matrix and the pixel byte of the display device, and write it to the frame buffer.

<div style="text-align: center"><img width= "800" img src ="https://foruda.gitee.com/images/1726135833249440419/4bf65309_13671125.png"/></div><br/>

### RLE Source_over No Matrix
The following flowchart describes the process of writing `compressed images` to a frame buffer in `source_over mode`. Taking the target device image type as RGB565 as an example.

<div style="text-align: center"><img width= "900" img src ="https://foruda.gitee.com/images/1726136048376898937/05600e92_13671125.png"/></div><br/>

1. Perform different processing steps based on the `img_type` from the head of compression data.
2. Decompress the compressed image data.
3. Based on the `opacity_value`, execute the corresponding operation to write image pixels into the framebuffer.
    - If the `opacity_value` is `0`, the image is not displayed and the process is break.
    - If the `opacity_value` is `255`: When the source image is in RGB565 format, directly write it to the frame buffer. Otherwise, perform the corresponding `do blend` operation and write the blend result to the frame buffer.
    - If the `opacity_value` is between `0 and 255`, perform the appropriate `do_blending` operation to blend the source image pixels with the corresponding frame buffer pixels. Write the blended result to the frame buffer.

### RLE Source_over Matrix
The following flowchart describes the process of writing `compressed images` to a frame buffer in `source_over mode with matrix operations`. Taking the target device image type as RGB565 as an example.

<div style="text-align: center"><img width= "900" img src ="https://foruda.gitee.com/images/1726136052909602952/42461659_13671125.png"/></div><br/>

1. Perform different processing steps based on the `img_type` from the head of compression data.
2. Decompress the compressed image data.
3. Perform matrix calculation to map the target area write-in points to image pixels, and obtain the pixel value of the image pixels.
4. Based on the `opacity_value`, execute the corresponding operation to write image pixels into the framebuffer.
    - If the `opacity_value` is `0`, the image is not displayed and the process is break.
    - If the `opacity value` level is `255`: When the source image is in RGB565 format, directly write it to the frame buffer. Otherwise, perform the corresponding `do blend` operation and write the blend result to the frame buffer.
    - If the `opacity_value` is between `0 and 255`, perform the appropriate `do_blending` operation to blend the source image pixels with the corresponding frame buffer pixels. Write the blended result to the frame buffer.


```{note}
   In compressed source_over matrix mode output rle_rgb888 and rle_rgba8888 equivalent to output as rle_rgb565.
```

## Support Input type and Output type

|Input type|Output type|
|---|---|
|RGB565|RGB565|
|RGB888|RGB888|
|ARGB8888|ARGB8888|
|ARGB8565|ARGB8565|
|RLE_RGB565|RLE_RGB565|
|RLE_RGB888|RLE_RGB888|
|RLE_ARGB8888|RLE_ARGB8888|
|RLE_ARGB8565|RLE_ARGB8565|
