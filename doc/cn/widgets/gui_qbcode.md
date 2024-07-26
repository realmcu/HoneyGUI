# Qrcode

The qbcode widget is the widget used to create or display qrcode and barcode. You can set a border, location and size attribute for qbcode widget. Qbcode widget support text and binary data  for qrcode encode, and support support text for barcode encode. The qrcode widget the QR Code Model 2 specification, supporting all versions (sizes) from 1 to 40, all 4 error correction levels, and 4 character encoding modes. The widget support fout error correction level in a QR Code symbol, and the high level is set by default. Barcode support code128 format.

## Usage

### create qrcode widget

[gui_qbcode_t *gui_qbcode_create(void                  *parent,
                                 const char            *name,
                                 int16_t                x,
                                 int16_t                y,
                                 int16_t                w,
                                 int16_t                h,
                                 T_QBCODE_DISPLAY_TYPE  type,
                                 T_QBCODE_ENCODE_TYPE   encodeType)](#api)
 
### config qrcode size and border

[void gui_qbcode_config(gui_qbcode_t *qbcode, uint8_t *data, uint32_t data_len, uint8_t border_size)](#api)

### qrcode encode notes

The encoded data for qrcode can exceeded the max bytes by default, which can modify the max version is 15; You can modify the version according to the document: [https://www.qrcode.com/zh/about/version.html](https://www.qrcode.com/zh/about/version.html);

## Example

<details> <summary>sample code</summary>

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

</details>

<br>
<center><img width= "300" src="https://foruda.gitee.com/images/1713239098192542999/e635ea60_10641540.jpeg" /></center>
<br>

<details> <summary>sample code</summary>

```c
#define BARCODE_WIDTH     (143 * 3)
#define BARCODE_HEIGHT    143
#define DISPLAY_TYPE     BARCODE_DISPLAY_IMAGE //BARCODE_DISPLAY_SECTION
#define ENCODED_TYPE     BARCODE_ENCODE_TEXT

char* str = "1234567890";

gui_qbcode_t * qrcode = gui_qbcode_create(parent,
                                          "qrcode",
                                          454/2 - BARCODE_WIDTH/2,
                                          454/2 - BARCODE_HEIGHT/2,
                                          BARCODE_WIDTH,
                                          BARCODE_HEIGHT,
                                          DISPLAY_TYPE,
                                          ENCODED_TYPE);

gui_qbcode_config(qrcode, str, strlen(str) + 1, 10);
```

</details>

<br>

<center><img width= "300" src="https://foruda.gitee.com/images/1713341892058343798/70403721_10641540.jpeg" /></center>
<br>

<span id="api">

## API

</span>

```eval_rst

.. doxygenfile:: gui_qbcode.h

```
