# 软件加速

## 总体流程图
该流程图描绘了软件加速处理图像资源的流程。 在处理图像时，根据图像的压缩状态和类型选择不同的处理方法：

- **覆盖 (cover)**: 将原始图像数据直接写入到帧缓冲区中的相应位置。不进行任何处理，只是直接覆盖。
- **旁路 (bypass)**: 将原始图像数据直接写入到帧缓冲区中的相应位置。旁路模式无法处理图像的透明度。它对整个图像应用全局不透明度值，从而影响整体透明度。在创建透明效果时，旁路模式相比源覆盖模式更占用空间。
- **滤黑 (Filter black)**: 滤波技术有效地从原始图像数据中筛选出像素值为零的像素数据，这意味着黑色像素不会被写入到帧缓冲区中。这种机制能够加快刷新速度。除黑色以外的任何颜色像素都会经过标准处理方法，并被记录到帧缓冲区中。
- **源覆盖 (Source_over)**: 一种混合方法，将图像的颜色数据和帧缓冲区像素的颜色数据结合起来，基于不透明度值 `Sa` 计算最终的颜色，并将其写入到帧缓冲区的相应位置。计算公式为 `((255 - Sa) * D + Sa * S) / 255)`，其中 `Sa` 是原始图像的不透明度值，`D` 是帧缓冲区的像素数据，`S` 是源图像的像素数据。

<div style="text-align: center"><img src ="https://foruda.gitee.com/images/1714378645744054188/753790c4_13671125.png"/></div><br/>

- `img_type` 可以从图像的头部 `head` 中获得，图像头部的结构如下：

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
- `img_type` 的枚举值如下。如果值为 `IMDC_COMPRESS`，则表示图像已压缩，并进入 `rle` 处理流程；否则，进入 `no rle` 处理流程。

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

- 根据不同的混合模式 `blend_mode`执行相应的 `blit` 过程。
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

- 当图像被压缩时，需要从压缩数据的地址中获取压缩头。该头部中的 `algorithm_type` 参数包含了实际的图像类型。压缩图像的类型在 `imdc_src_type` 结构体中描述，包括三种类型：`IMDC_SRC_RGB565`、 `IMDC_SRC_RGB888` 和 `IMDC_SRC_ARGB8888`。
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

## 无RLE覆盖模式概述
以下流程描述了 `No RLE` 压缩图像的 `cover mode` 处理过程。根据图像矩阵和显示设备的像素字节数选择处理方法，并将其写入帧缓冲区。

<div style="text-align: center"><img width= "800" img src ="https://foruda.gitee.com/images/1714375761122113396/e0e01095_13671125.png"/></div><br/>

- 如果矩阵是单位矩阵，则执行没有矩阵操作的[blit](/Glossary.rst#term-blit)过程；否则，执行具有矩阵操作的blit过程。
- `dc_bytes_per_pixel` 表示显示设备的像素字节数，计算方式为 `dc->bit_depth >> 3`，其中 `bit_depth` 为显示设备的位深度。以位深度为24的显示设备为例，其像素字节数为3。

### 无RLE覆盖模式（不带矩阵变换）
下面的流程图描述了将 `uncompressed images` 写入帧缓冲区的 `cover mode`处理过程，以RGB565为目标设备图像类型为例。

<div style="text-align: center"><img width="400" src ="https://foruda.gitee.com/images/1714375777765665290/25d64da9_13671125.png"/></div><br/>

### 无RLE覆盖模式（带矩阵变换）
下面的流程图描述了使用 `cover mode with matrix operations` 将 `uncompressed images` 写入帧缓冲区的过程，以RGB565为目标设备图像类型为例。

<div style="text-align: center"><img width="550" src ="https://foruda.gitee.com/images/1714455839216373200/304e06b6_13671125.png"/></div><br/>

## 无RLE旁路模式概述

以下流程描述了 `No RLE` 压缩图像的 `bypass mode` 处理过程。根据图像矩阵和显示设备的像素字节数选择处理方法，并将其写入帧缓冲区。

<div style="text-align: center"><img width= "800" img src ="https://foruda.gitee.com/images/1710327745470292085/b2ffb40d_13671125.png"/></div><br/>

- 如果矩阵是单位矩阵，则执行没有矩阵操作的blit过程；否则，执行具有矩阵操作的blit过程。
- `dc_bytes_per_pixel` 表示显示设备的像素字节数，计算方式为 `dc->bit_depth >> 3`，其中 `bit_depth` 为显示设备的位深度。以位深度为24的显示设备为例，其像素字节数为3。
  
### 无RLE旁路模式（不带矩阵变换）
下面的流程图描述了将 `uncompressed images` 写入帧缓冲区的 `bypass mode`处理过程，以RGB565为目标设备图像类型为例。

<div style="text-align: center"><img src ="https://foruda.gitee.com/images/1710318765186128026/16894745_13671125.png"/></div><br/>

1. 根据 `img_type` 执行不同的处理步骤。
2. 基于 `opacity_value` 执行相应的操作将图像像素写入帧缓冲区。
    - 如果 `opacity_value` 为 `0`，表示图像不显示，直接跳出处理流程。
    - 如果 `opacity_value` 为 `255`，将源图像像素转换为RGB565格式，并写入帧缓冲区。
    - 如果 `opacity_value` 介于 `0` 和 `255` 之间，执行Alpha混合操作将源图像像素与对应的帧缓冲区像素进行混合。混合公式为 `((255 - Sa) * D + Sa * S) / 255)`，将混合结果写入帧缓冲区。

### 无RLE旁路模式（带矩阵变换）
下面的流程图描述了使用 `blend mode with matrix operations` 将 `uncompressed images` 写入帧缓冲区的过程，以RGB565为目标设备图像类型为例。

<div style="text-align: center"><img src ="https://foruda.gitee.com/images/1710318778978528410/26fe6091_13671125.png"/></div><br/>

1. 根据 `img_type` 执行不同的处理步骤。
2. 执行矩阵计算，将目标区域的写入点映射到图像像素，并获取图像像素的像素值。
3. 基于 `opacity_value` 执行相应的操作将图像像素写入帧缓冲区。
    - 如果 `opacity_value` 为 `0`，表示图像不显示，直接跳出处理流程。
    - 如果 `opacity_value` 为 `255`，将源图像像素转换为RGB565格式，并写入帧缓冲区。
    - 如果 `opacity_value` 介于 `0` 和 `255` 之间，执行Alpha混合操作将源图像像素与对应的帧缓冲区像素进行混合。混合公式为 `((255 - Sa) * D + Sa * S) / 255)`，将混合结果写入帧缓冲区。

## 无RLE滤黑模式概述
以下流程描述了 `No RLE` 压缩图像的 `filter mode` 处理过程。根据图像矩阵和显示设备的像素字节数选择处理方法，并将其写入帧缓冲区。

<div style="text-align: center"><img width= "800" img src ="https://foruda.gitee.com/images/1710327755412119622/87231a49_13671125.png"/></div><br/>

### 无RLE滤黑模式（不带矩阵变换）
下面的流程图描述了将 `uncompressed images` 写入帧缓冲区的 `filter mode`处理过程，以RGB565为目标设备图像类型为例。

<div style="text-align: center"><img src ="https://foruda.gitee.com/images/1710318784445569859/c4bf1075_13671125.png"/></div><br/>

1. 根据 `img_type` 执行不同的处理步骤。
2. 如果像素值为0，则跳过处理；否则，执行后续写入操作。
3. 基于 `opacity_value` 执行相应的操作将图像像素写入帧缓冲区。
    - 如果 `opacity_value` 为 `0`，表示图像不显示，直接跳出处理流程。
    - 如果 `opacity_value` 为 `255`，将源图像像素转换为RGB565格式，并写入帧缓冲区。
    - 如果 `opacity_value` 介于 `0` 和 `255` 之间，执行Alpha混合操作将源图像像素与对应的帧缓冲区像素进行混合。混合公式为 `((255 - Sa) * D + Sa * S) / 255)`，将混合结果写入帧缓冲区。

### 无RLE旁路模式（带矩阵变换）
下面的流程图描述了使用 `filter mode with matrix operations` 将 `uncompressed images` 写入帧缓冲区的过程，以RGB565为目标设备图像类型为例。

<div style="text-align: center"><img src ="https://foruda.gitee.com/images/1710318788829157429/3ca92cb9_13671125.png"/></div><br/>

1. 根据 `img_type` 执行不同的处理步骤。
2. 执行矩阵计算，将目标区域的写入点映射到图像像素，并获取图像像素的像素值。
3. 如果像素值为0，则跳过处理；否则，执行后续写入操作。
4. 基于 `opacity_value` 执行相应的操作将图像像素写入帧缓冲区。
    - 如果 `opacity_value` 为 `0`，表示图像不显示，直接跳出处理流程。
    - 如果 `opacity_value` 为 `255`，将源图像像素转换为RGB565格式，并写入帧缓冲区。
    - 如果 `opacity_value` 介于 `0` 和 `255` 之间，执行Alpha混合操作将源图像像素与对应的帧缓冲区像素进行混合。混合公式为 `((255 - Sa) * D + Sa * S) / 255)`，将混合结果写入帧缓冲区。

## 无RLE源覆盖模式概述
以下流程描述了 `No RLE` 压缩图像的 `source_over mode` 处理过程。根据图像矩阵和显示设备的像素字节数选择处理方法，并将其写入帧缓冲区。

<div style="text-align: center"><img src ="https://foruda.gitee.com/images/1710327760371625897/982f575e_13671125.png"/></div><br/>

### 无RLE源覆盖模式（不带矩阵变换）
下面的流程图描述了将 `uncompressed images` 写入帧缓冲区的 `source_over mode`处理过程，以RGB565为目标设备图像类型为例。

<div style="text-align: center"><img width="500" src ="https://foruda.gitee.com/images/1710318794490000646/1da4e0a2_13671125.png"/></div><br/>

基于 `opacity_value` 执行相应的操作将图像像素写入帧缓冲区。
  - 如果 `opacity_value` 为 `0`，表示图像不显示，直接跳出处理流程。
  - 如果 `opacity_value` 为 `255`，将源图像像素转换为RGB565格式，并写入帧缓冲区。
  - 如果 `opacity_value` 介于 `0` 和 `255` 之间，执行`do_blending_acc_2_rgb565_opacity` 对源图像像素与相应的帧缓冲区像素进行混合，将混合结果写入帧缓冲区。

### 无RLE源覆盖模式（带矩阵变换）
下面的流程图描述了使用 `source_over mode with matrix operations` 将 `uncompressed images` 写入帧缓冲区的过程，以RGB565为目标设备图像类型为例。

<div style="text-align: center"><img width="500" src ="https://foruda.gitee.com/images/1710318799964556869/22c5cdfb_13671125.png"/></div><br/>

1. 执行矩阵计算，将目标区域的写入点映射到图像像素，并获取图像像素的像素值。
2. 基于 `opacity_value` 执行相应的操作将图像像素写入帧缓冲区。
    - 如果 `opacity_value` 为 `0`，表示图像不显示，直接跳出处理流程。
    - 如果 `opacity_value` 为 `255`，将源图像像素转换为RGB565格式，并写入帧缓冲区。
    - 如果 `opacity_value` 介于 `0` 和 `255` 之间，执行`do_blending_acc_2_rgb565_opacity` 对源图像像素与相应的帧缓冲区像素进行混合，将混合结果写入帧缓冲区。

## RLE覆盖模式概述
以下流程描述了 `RLE` 压缩图像的 `cover mode` 处理过程。根据图像矩阵和显示设备的像素字节数选择处理方法，并将其写入帧缓冲区。

<div style="text-align: center"><img width= "800" img src ="https://foruda.gitee.com/images/1714376646404378867/18b85d40_13671125.png"/></div><br/>

### RLE覆盖模式（不带矩阵变换）
下面的流程图描述了将 `compressed images` 写入帧缓冲区的 `cover mode`处理过程，以RGB565为目标设备图像类型为例。

<div style="text-align: center"><img width="700" src ="https://foruda.gitee.com/images/1714455786263653108/4f957293_13671125.png"/></div><br/>

1. 根据压缩数据头部的 `img_type` 执行不同的处理步骤。
2. 对压缩图像数据进行解压。
3. 将像素结果写入帧缓冲区。

### RLE覆盖模式（带矩阵变换）
下面的流程图描述了使用 `cover mode with matrix operations` 将 `compressed images` 写入帧缓冲区的过程，以RGB565为目标设备图像类型为例。

<div style="text-align: center"><img width="700" src ="https://foruda.gitee.com/images/1714455799554861994/cbe7fe2e_13671125.png"/></div><br/>

1. 根据压缩数据头部的 `img_type` 执行不同的处理步骤。
2. 对压缩图像数据进行解压。
3. 进行矩阵计算，将目标区域的写入点映射到图像像素，并获得图像像素的像素值。
4. 将像素结果写入帧缓冲区。

## RLE旁路模式概述
以下流程描述了 `RLE` 压缩图像的 `bypass mode` 处理过程。根据图像矩阵和显示设备的像素字节数选择处理方法，并将其写入帧缓冲区。

<div style="text-align: center"><img width= "800" img src ="https://foruda.gitee.com/images/1710327763804701147/539b16fd_13671125.png"/></div><br/>

### RLE旁路模式（不带矩阵变换）
下面的流程图描述了将 `bypass images` 写入帧缓冲区的 `cover mode`处理过程，以RGB565为目标设备图像类型为例。

<div style="text-align: center"><img src ="https://foruda.gitee.com/images/1710318858967603728/74fc9285_13671125.png"/></div><br/>

1. 根据压缩数据头部的 `img_type` 执行不同的处理步骤。
2. 对压缩图像数据进行解压。
3. 基于 `opacity_value` 执行相应的操作将图像像素写入帧缓冲区。
    - 如果 `opacity_value` 为 `0`，表示图像不显示，直接跳出处理流程。
    - 如果 `opacity_value` 为 `255`，将源图像像素转换为RGB565格式，并写入帧缓冲区。
    - 如果 `opacity_value` 介于 `0` 和 `255` 之间，执行Alpha混合操作将源图像像素与对应的帧缓冲区像素进行混合。混合公式为 `((255 - Sa) * D + Sa * S) / 255)`，将混合结果写入帧缓冲区。

### RLE旁路模式（带矩阵变换）
下面的流程图描述了使用 `bypass mode with matrix operations` 将 `compressed images` 写入帧缓冲区的过程，以RGB565为目标设备图像类型为例。

<div style="text-align: center"><img src ="https://foruda.gitee.com/images/1710318868326988985/8e089811_13671125.png"/></div><br/>

1. 根据压缩数据头部的 `img_type` 执行不同的处理步骤。
2. 对压缩图像数据进行解压。
3. 进行矩阵计算，将目标区域的写入点映射到图像像素，并获得图像像素的像素值。
4. 基于 `opacity_value` 执行相应的操作将图像像素写入帧缓冲区。
    - 如果 `opacity_value` 为 `0`，表示图像不显示，直接跳出处理流程。
    - 如果 `opacity_value` 为 `255`，将源图像像素转换为RGB565格式，并写入帧缓冲区。
    - 如果 `opacity_value` 介于 `0` 和 `255` 之间，执行Alpha混合操作将源图像像素与对应的帧缓冲区像素进行混合。混合公式为 `((255 - Sa) * D + Sa * S) / 255)`，将混合结果写入帧缓冲区。

## RLE滤黑模式概述
以下流程描述了 `RLE` 压缩图像的 `filter mode` 处理过程。根据图像矩阵和显示设备的像素字节数选择处理方法，并将其写入帧缓冲区。

<div style="text-align: center"><img width= "800" img src ="https://foruda.gitee.com/images/1710327772848839544/88fe64aa_13671125.png"/></div><br/>

### RLE滤黑模式（不带矩阵变换）
下面的流程图描述了将 `compressed images` 写入帧缓冲区的 `filter mode`处理过程，以RGB565为目标设备图像类型为例。

<div style="text-align: center"><img src ="https://foruda.gitee.com/images/1710318872306891932/9145c589_13671125.png"/></div><br/>

1. 根据压缩数据头部的 `img_type` 执行不同的处理步骤。
2. 对压缩图像数据进行解压。
3. 如果像素值为0，则跳过处理；否则，执行后续写入操作。
4. 基于 `opacity_value` 执行相应的操作将图像像素写入帧缓冲区。
    - 如果 `opacity_value` 为 `0`，表示图像不显示，直接跳出处理流程。
    - 如果 `opacity_value` 为 `255`，将源图像像素转换为RGB565格式，并写入帧缓冲区。
    - 如果 `opacity_value` 介于 `0` 和 `255` 之间，执行Alpha混合操作将源图像像素与对应的帧缓冲区像素进行混合。混合公式为 `((255 - Sa) * D + Sa * S) / 255)`，将混合结果写入帧缓冲区。

### RLE滤黑模式（带矩阵变换）
下面的流程图描述了使用 `filter mode with matrix operations` 将 `compressed images` 写入帧缓冲区的过程，以RGB565为目标设备图像类型为例。

<div style="text-align: center"><img src ="https://foruda.gitee.com/images/1710318876464804105/64471b6b_13671125.png"/></div><br/>

1. 根据压缩数据头部的 `img_type` 执行不同的处理步骤。
2. 对压缩图像数据进行解压。
3. 进行矩阵计算，将目标区域的写入点映射到图像像素，并获得图像像素的像素值。
4. 如果像素值为0，则跳过处理；否则，执行后续写入操作。
5. 基于 `opacity_value` 执行相应的操作将图像像素写入帧缓冲区。
    - 如果 `opacity_value` 为 `0`，表示图像不显示，直接跳出处理流程。
    - 如果 `opacity_value` 为 `255`，将源图像像素转换为RGB565格式，并写入帧缓冲区。
    - 如果 `opacity_value` 介于 `0` 和 `255` 之间，执行Alpha混合操作将源图像像素与对应的帧缓冲区像素进行混合。混合公式为 `((255 - Sa) * D + Sa * S) / 255)`，将混合结果写入帧缓冲区。

## RLE源覆盖模式概述
以下流程描述了 `RLE` 压缩图像的 `source_over mode` 处理过程。根据图像矩阵和显示设备的像素字节数选择处理方法，并将其写入帧缓冲区。

<div style="text-align: center"><img width= "800" img src ="https://foruda.gitee.com/images/1710327781855932915/3ce7e711_13671125.png"/></div><br/>

### RLE源覆盖模式（不带矩阵变换）
下面的流程图描述了将 `compressed images` 写入帧缓冲区的 `source_over mode`处理过程，以RGB565为目标设备图像类型为例。

<div style="text-align: center"><img src ="https://foruda.gitee.com/images/1710318880764265303/dee05307_13671125.png"/></div><br/>

1. 根据压缩数据头部的 `img_type` 执行不同的处理步骤。
2. 对压缩图像数据进行解压。
3. 基于 `opacity_value` 执行相应的操作将图像像素写入帧缓冲区。
    - 如果 `opacity_value` 为 `0`，表示图像不显示，直接跳出处理流程。
    - 如果 `opacity_value` 为 `255`， 当源图像为RGB565格式时，直接将其写入帧缓冲区。否则，执行相应的混合操作 `do blend` ，并将混合结果写入帧缓冲区。
    - 如果 `opacity_value` 介于 `0` 和 `255` 之间，执行适当的混合操作 `do_blending` 来混合源图像像素与相应的帧缓冲区像素，将混合结果写入帧缓冲区。

### RLE源覆盖模式（带矩阵变换）
下面的流程图描述了使用 `source_over mode with matrix operations` 将 `compressed images` 写入帧缓冲区的过程，以RGB565为目标设备图像类型为例。

<div style="text-align: center"><img src ="https://foruda.gitee.com/images/1710318886248980211/a668df5f_13671125.png"/></div><br/>

1. 根据压缩数据头部的 `img_type` 执行不同的处理步骤。
2. 对压缩图像数据进行解压。
3. 进行矩阵计算，将目标区域的写入点映射到图像像素，并获得图像像素的像素值。
4. 基于 `opacity_value` 执行相应的操作将图像像素写入帧缓冲区。
    - 如果 `opacity_value` 为 `0`，表示图像不显示，直接跳出处理流程。
    - 如果 `opacity_value` 为 `255`， 当源图像为RGB565格式时，直接将其写入帧缓冲区。否则，执行相应的混合操作 `do blend` ，并将混合结果写入帧缓冲区。
    - 如果 `opacity_value` 介于 `0` 和 `255` 之间，执行适当的混合操作 `do_blending` 来混合源图像像素与相应的帧缓冲区像素，将混合结果写入帧缓冲区。

## 流程图下载
如果您需要修改流程图的可视化文件，请从以下路径下载：
[acc](https://gitee.com/langhao-luo/gui_images/blob/master/visio/design/acc.vsdx)
[rle](https://gitee.com/langhao-luo/gui_images/blob/master/visio/design/rle.vsdx)
[no rle](https://gitee.com/langhao-luo/gui_images/blob/master/visio/design/no_rle.vsdx)


```{eval-rst}
.. 注意::
   在压缩的source_over矩阵模式下，rle_rgb888和rle_rgba8888相当于输出rle_rgb565。
```

## 支持的输入类型和输出类型

|输入类型|输出类型|
|---|---|
|RGB565|RGB565|
|RGB888|RGB888|
|-|ARGB8565|
|ARGB8888|ARGB8888|

