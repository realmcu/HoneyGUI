# 二维码或条形码 (Qbcode)

二维码条形码控件可以用来生成或展示二维码和条形码，开发者可以设置二维码控件或者条形码控件的边框、位置和大小属性。生成二维码时，qbcode控件能处理文本和二进制数据；生成条形码时，则只能处理文本数据。二维码遵循QR Code Model 2标准，支持从1到40的各种规格，支持4种纠错等级和4种字符编码模式。默认情况下，二维码的纠错等级设置为最高级。对于条形码，它支持code128格式。

## 用法

### 创建控件

使用 [gui_qbcode_create(void *parent, const char *name, int16_t x, int16_t y, int16_t w, int16_t h, T_QBCODE_DISPLAY_TYPE type, T_QBCODE_ENCODE_TYPE encodeType)](#gui_qbcode_create) 函数创建一个二维码或者条形码控件，具体创建的是二维码控件还是条形码控件，由参数 `encodeType` 决定。`encodeType` 枚举类型如下：

```eval_rst

.. literalinclude:: ../../../realgui/widget/gui_qbcode.h
   :language: c
   :start-after: /** @brief  T_QBCODE_ENCODE_TYPE structure */
   :end-before: /** @brief  T_QBCODE_DISPLAY_TYPE structure */

```

### 设置尺寸和边框

使用 [gui_qbcode_config(gui_qbcode_t *qbcode, uint8_t *data, uint32_t data_len, uint8_t border_size)](#gui_qbcode_config) 函数来设置控件的尺寸和边框。

### 二维码编码注释

二维码的编码数据默认情况下可以超过最大字节数，这时可以将最大版本修改为15，版本的修改可以参照以下文档：https://www.qrcode.com/zh/about/version.html

## 示例

### 二维码

```c
#include "gui_qbcode.h"
#define QRCODE_WIDTH     320
#define QRCODE_HEIGHT    320
#define DISPLAY_TYPE     QRCODE_DISPLAY_IMAGE //QRCODE_DISPLAY_SECTION
#define ENCODED_TYPE     QRCODE_ENCODE_TEXT   // QRCODE_ENCODE_BINARY

char* str = "123455678901234567890123455678901234567890123455678901234567890123455678901234567890123455678901234567890";

gui_qbcode_t * qrcode = gui_qbcode_create(parent,
                                          "qrcode",
                                          454/2 - QRCODE_WIDTH/2,
                                          454/2 - QRCODE_HEIGHT/2,
                                          QRCODE_WIDTH,
                                          QRCODE_HEIGHT,
                                          DISPLAY_TYPE,
                                          ENCODED_TYPE);

gui_qbcode_config(qrcode, str, strlen(str), 3);
```

<br>
<center><img width= "300" src="https://foruda.gitee.com/images/1713239098192542999/e635ea60_10641540.jpeg" /></center>
<br>

### 条形码

```c
#define BARCODE_WIDTH     (143 * 3)
#define BARCODE_HEIGHT    143
#define DISPLAY_TYPE     BARCODE_DISPLAY_IMAGE //BARCODE_DISPLAY_SECTION
#define ENCODED_TYPE     BARCODE_ENCODE_TEXT

char* str = "1234567890";

gui_qbcode_t * barcode = gui_qbcode_create(parent,
                                          "qrcode",
                                          454/2 - BARCODE_WIDTH/2,
                                          454/2 - BARCODE_HEIGHT/2,
                                          BARCODE_WIDTH,
                                          BARCODE_HEIGHT,
                                          DISPLAY_TYPE,
                                          ENCODED_TYPE);

gui_qbcode_config(qrcode, str, strlen(str) + 1, 10);
```
<br>

<center><img width= "300" src="https://foruda.gitee.com/images/1713341892058343798/70403721_10641540.jpeg" /></center>
<br>

<span id="api">

## API

</span>

```eval_rst

.. doxygenfile:: gui_qbcode.h

```
