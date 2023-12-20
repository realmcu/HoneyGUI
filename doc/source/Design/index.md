# Design Spec

## Design details

## sw acc


### Overall flow chart
```{mermaid}
graph TD
source[source] -->C{judge compress}
C -->|N| no_rle[no rle]
C -->|Y| rle[rle]
no_rle --> img_type{image type}
img_type -->bypass
img_type -->filter
img_type -->source_over
rle --> rle_img_type{image type}
rle_img_type --> rle_bypass
rle_img_type --> rle_filter
rle_img_type --> rle_source_over

```
### no_rle_bypass mode
```{mermaid}
graph TD
bypass --> b{output}
b --> |Identity matrix judgment| byapss_rgb565
b --> |Identity matrix judgment| bypass_rgb888
b --> |Identity matrix judgment| bypass_rgba8888
```
```{mermaid}
graph TD
bypass_rgb565 --> bypass_565{input}
bypass_565 --> | |bypass_565_565
bypass_565_565 --> no_bypass_opacity_case565{opacity}
no_bypass_opacity_case565 --> |opacity|no_bypass_opacity_value0_255(0-255)
no_bypass_opacity_case565 --> |no opacity|no_bypass_opacity_value255(255)
no_bypass_opacity_case565 --> |break|no_bypass_opacity_value0(0)
bypass_565 --> | |bypass_565_888
bypass_565_888--> no_bypass_opacity_case888{opacity}
no_bypass_opacity_case888 --> |opacity|no_bypass_opacity_value888_0_255(0-255)
no_bypass_opacity_case888 --> |no opacity|no_bypass_opacity_value888_255(255)
no_bypass_opacity_case888 --> |break|no_bypass_opacity_value888_0(0)
bypass_565 --> | |bypass_565_8888
bypass_565_8888 --> no_bypass_opacity_case8888{opacity}
no_bypass_opacity_case8888 --> |opacity|no_bypass_opacity_value8888_0_255(0-255)
no_bypass_opacity_case8888 --> |no opacity|no_bypass_opacity_value8888_255(255)
no_bypass_opacity_case8888 --> |break|no_bypass_opacity_value8888_0(0)
```
Notice: In non-compressed bypass mode output rgb888 and rgba8888 equivalent to output as rgb565.
### overview filter
```{mermaid}
graph TD
filter --> b{Identity matrix judgment}
b --> |N|filter__no_matrix
filter__no_matrix -->fiter_no_matrix_output{output}
fiter_no_matrix_output -->filter_rgb565
fiter_no_matrix_output -->filter_rgb888
fiter_no_matrix_output -->filter_rgba8888
b --> |Y|filter_matrix
filter_matrix --> fiter_matrix_output{output}
fiter_matrix_output -->filter_matrix_rgb565
fiter_matrix_output -->filter_matirx_rgb888
fiter_matrix_output -->filter_matrix_rgba8888
```
### no_rle_filter

```{mermaid}
graph TD
filter_rgb565 --> filter_565{input}
filter_565 --> | |filter_565_565
filter_565_565 --> filter_opacity_case565{opacity}
filter_opacity_case565 --> |opacity| filter_opacity_value0_255(0-255)
filter_opacity_case565 --> |no opacity|filter_opacity_value255(255)
filter_opacity_case565 --> |break|filter_opacity_value0(0)
filter_565 --> | |filter_565_888
filter_565_888--> filter_opacity_case888{opacity}
filter_opacity_case888 --> |opacity|filter_opacity_value888_0_255(0-255)
filter_opacity_case888 --> |no opacity|filter_opacity_value888_255(255)
filter_opacity_case888 --> |break|filter_opacity_value888_0(0)
filter_565 --> | |filter_565_8888
filter_565_8888 --> filter_opacity_case8888{opacity}
filter_opacity_case8888 --> |opacity|filter_opacity_value8888_0_255(0-255)
filter_opacity_case8888 --> |no opacity|filter_opacity_value8888_255(255)
filter_opacity_case8888 --> |break|filter_opacity_value8888_0(0)
```
Notice: In non-compressed filter mode output rgb888 and rgba8888 equivalent to output as rgb565.
### no rle filter_matrix
```{mermaid}
graph TD
filter_matrix_rgb565 --> filter_matrix_565{input}
filter_matrix_565 --> | |filter_matrix_565_565
filter_matrix_565_565 --> filter_matrix_opacity_case565{opacity}
filter_matrix_opacity_case565 --> |opacity| filter_matrix_opacity_value0_255(0-255)
filter_matrix_opacity_case565 --> |no opacity|filter_matrix_opacity_value255(255)
filter_matrix_opacity_case565 --> |break|filter_matrix_opacity_value0(0)
filter_matrix_565 --> | |filter_matrix_565_888
filter_matrix_565_888--> filter_matrix_opacity_case888{opacity}
filter_matrix_opacity_case888 --> |opacity|filter_matrix_opacity_value888_0_255(0-255)
filter_matrix_opacity_case888 --> |no opacity|filter_matrix_opacity_value888_255(255)
filter_matrix_opacity_case888 --> |break|filter_matrix_opacity_value888_0(0)
filter_matrix_565 --> | |filter_matrix_565_8888
filter_matrix_565_8888 --> filter_matrix_opacity_case8888{opacity}
filter_matrix_opacity_case8888 --> |opacity|filter_matrix_opacity_value8888_0_255(0-255)
filter_matrix_opacity_case8888 --> |no opacity|filter_matrix_opacity_value8888_255(255)
filter_matrix_opacity_case8888 --> |break|filter_matrix_opacity_value8888_0(0)
```
Notice: In non-compressed filter_matrix mode output rgb888 and rgba8888 equivalent to output as rgb565.

### overview source_over

```{mermaid}
graph TD
alpha --> b{Identity matrix judgment}
b --> |N|alpha__no_matrix
alpha__no_matrix -->fiter_no_matrix_output{output}
fiter_no_matrix_output -->alpha_rgb565
fiter_no_matrix_output -->alpha_rgb888
fiter_no_matrix_output -->alpha_rgba8888
b --> |Y|alpha_matrix
alpha_matrix --> fiter_matrix_output{output}
fiter_matrix_output -->alpha_matrix_rgb565
fiter_matrix_output -->alpha_matirx_rgb888
fiter_matrix_output -->alpha_matrix_rgba8888
```
### no_rle_alpha no matrix
```{mermaid}
graph TD
alpha_rgb565 --> alpha_565{input}
alpha_565 --> | |alpha_565_565
alpha_565_565 --> alpha_opacity_case565{opacity}
alpha_opacity_case565 --> |opacity + alpha| alpha_opacity_value0_255(0-255)
alpha_opacity_case565 --> |alpha|alpha_opacity_value255(255)
alpha_opacity_case565 --> |break|alpha_opacity_value0(0)
alpha_565 --> | |alpha_565_888
alpha_565_888--> alpha_opacity_case888{opacity}
alpha_opacity_case888 --> |opacity + alpha|alpha_opacity_value888_0_255(0-255)
alpha_opacity_case888 --> |alpha|alpha_opacity_value888_255(255)
alpha_opacity_case888 --> |break|alpha_opacity_value888_0(0)
alpha_565 --> | |alpha_565_8888
alpha_565_8888 --> alpha_opacity_case8888{opacity}
alpha_opacity_case8888 --> |opacity + alpha|alpha_opacity_value8888_0_255(0-255)
alpha_opacity_case8888 --> |alpha|alpha_opacity_value8888_255(255)
alpha_opacity_case8888 --> |break|alpha_opacity_value8888_0(0)
```
Notice: In non-compressed source_over mode output rgb888 and rgba8888 equivalent to output as rgb565.

### no_rle_alpha matrix

```{mermaid}
graph TD
alpha_matrix_rgb565 --> alpha_matrix_565{input}
alpha_matrix_565 --> | |alpha_matrix_565_565
alpha_matrix_565_565 --> alpha_matrix_opacity_case565{opacity}
alpha_matrix_opacity_case565 --> |opacity + alpha| alpha_matrix_opacity_value0_255(0-255)
alpha_matrix_opacity_case565 --> |alpha|alpha_matrix_opacity_value255(255)
alpha_matrix_opacity_case565 --> |break|alpha_matrix_opacity_value0(0)
alpha_matrix_565 --> | |alpha_matrix_565_888
alpha_matrix_565_888--> alpha_matrix_opacity_case888{opacity}
alpha_matrix_opacity_case888 --> |opacity + alpha|alpha_matrix_opacity_value888_0_255(0-255)
alpha_matrix_opacity_case888 --> |alpha|alpha_matrix_opacity_value888_255(255)
alpha_matrix_opacity_case888 --> |break|alpha_matrix_opacity_value888_0(0)
alpha_matrix_565 --> | |alpha_matrix_565_8888
alpha_matrix_565_8888 --> alpha_matrix_opacity_case8888{opacity}
alpha_matrix_opacity_case8888 --> |opacity + alpha|alpha_matrix_opacity_value8888_0_255(0-255)
alpha_matrix_opacity_case8888 --> |alpha|alpha_matrix_opacity_value8888_255(255)
alpha_matrix_opacity_case8888 --> |break|alpha_matrix_opacity_value8888_0(0)
```
Notice: In non-compressed source_over matrix mode output rgb888 and rgba8888 equivalent to output as rgb565.

### rle bypass

```{mermaid}
graph TD
rle_bypass --> b{output}
b --> |Identity matrix judgment| rle_rgb565
b --> |Identity matrix judgment| rle_rgb888
b --> |Identity matrix judgment| rle_rgba8888
```
```{mermaid}
graph TD
rle_rgb565 --> rle_bypass_565{input}
rle_bypass_565 --> | |rle_bypass_565_565
rle_bypass_565_565 --> rle_bypass_opacity_case565{opacity}
rle_bypass_opacity_case565 --> |opacity|rle_bypass_opacity_value0_255(0-255)
rle_bypass_opacity_case565 --> |no opacity|rle_bypass_opacity_value255(255)
rle_bypass_opacity_case565 --> |break|rle_bypass_opacity_value0(0)
rle_bypass_565 --> | |rle_bypass_565_888
rle_bypass_565_888--> rle_bypass_opacity_case888{opacity}
rle_bypass_opacity_case888 --> |opacity|rle_bypass_opacity_value888_0_255(0-255)
rle_bypass_opacity_case888 --> |no opacity|rle_bypass_opacity_value888_255(255)
rle_bypass_opacity_case888 --> |break|rle_bypass_opacity_value888_0(0)
rle_bypass_565 --> | |rle_bypass_565_8888
rle_bypass_565_8888 --> rle_bypass_opacity_case8888{opacity}
rle_bypass_opacity_case8888 --> |opacity|rle_bypass_opacity_value8888_0_255(0-255)
rle_bypass_opacity_case8888 --> |no opacity|rle_bypass_opacity_value8888_255(255)
rle_bypass_opacity_case8888 --> |break|rle_bypass_opacity_value8888_0(0)
```
Notice: In compressed bypass mode output rle_rgb888 and rle_rgba8888 equivalent to output as rle_rgb565.

### overview rle filter
```{mermaid}
graph TD
rle_filter --> b{Identity matrix judgment}
b --> |N| rle_filter_no_matrix
rle_filter_no_matrix --> rle_fiter_no_matrix_output{output}
rle_fiter_no_matrix_output -->rle_filter_rgb565
rle_fiter_no_matrix_output -->rle_filter_rgb888
rle_fiter_no_matrix_output -->rle_filter_rgba8888
b --> |Y| rle_filter_matrix
rle_filter_matrix --> rle_fiter_matrix_output{output}
rle_fiter_matrix_output -->rle_filter_matrix_rgb565
rle_fiter_matrix_output -->rle_filter_matirx_rgb888
rle_fiter_matrix_output -->rle_filter_matrix_rgba8888
```

### rle_filter
```{mermaid}
graph TD
rle_filter_rgb565 --> rle_filter_565{input}
rle_filter_565 --> | |rle_filter_565_565
rle_filter_565_565 --> rle_filter_opacity_case565{opacity}
rle_filter_opacity_case565 --> |opacity| rle_filter_opacity_value0_255(0-255)
rle_filter_opacity_case565 --> |no opacity|rle_filter_opacity_value255(255)
rle_filter_opacity_case565 --> |break|rle_filter_opacity_value0(0)
rle_filter_565 --> | |rle_filter_565_888
rle_filter_565_888--> rle_filter_opacity_case888{opacity}
rle_filter_opacity_case888 --> |opacity|rle_filter_opacity_value888_0_255(0-255)
rle_filter_opacity_case888 --> |no opacity|rle_filter_opacity_value888_255(255)
rle_filter_opacity_case888 --> |break|rle_filter_opacity_value888_0(0)
rle_filter_565 --> | |rle_filter_565_8888
rle_filter_565_8888 --> rle_filter_opacity_case8888{opacity}
rle_filter_opacity_case8888 --> |opacity|rle_filter_opacity_value8888_0_255(0-255)
rle_filter_opacity_case8888 --> |no opacity|rle_filter_opacity_value8888_255(255)
rle_filter_opacity_case8888 --> |break|rle_filter_opacity_value8888_0(0)
```
Notice: In compressed filter mode output rle_rgb888 and rle_rgba8888 equivalent to output as rle_rgb565.

### rle_filter_matrix
```{mermaid}
graph TD
rle_filter_matrix_rgb565 --> rle_filter_matrix_565{input}
rle_filter_matrix_565 --> | |rle_filter_matrix_565_565
rle_filter_matrix_565_565 --> rle_filter_matrix_opacity_case565{opacity}
rle_filter_matrix_opacity_case565 --> |opacity| rle_filter_matrix_opacity_value0_255(0-255)
rle_filter_matrix_opacity_case565 --> |no opacity|rle_filter_matrix_opacity_value255(255)
rle_filter_matrix_opacity_case565 --> |break|rle_filter_matrix_opacity_value0(0)
rle_filter_matrix_565 --> | |rle_filter_matrix_565_888
rle_filter_matrix_565_888--> rle_filter_matrix_opacity_case888{opacity}
rle_filter_matrix_opacity_case888 --> |opacity|rle_filter_matrix_opacity_value888_0_255(0-255)
rle_filter_matrix_opacity_case888 --> |no opacity|rle_filter_matrix_opacity_value888_255(255)
rle_filter_matrix_opacity_case888 --> |break|rle_filter_matrix_opacity_value888_0(0)
rle_filter_matrix_565 --> | |rle_filter_matrix_565_8888
rle_filter_matrix_565_8888 --> rle_filter_matrix_opacity_case8888{opacity}
rle_filter_matrix_opacity_case8888 --> |opacity|rle_filter_matrix_opacity_value8888_0_255(0-255)
rle_filter_matrix_opacity_case8888 --> |no opacity|rle_filter_matrix_opacity_value8888_255(255)
rle_filter_matrix_opacity_case8888 --> |break|rle_filter_matrix_opacity_value8888_0(0)
```
Notice: In compressed filter matrix mode output rle_rgb888 and rle_rgba8888 equivalent to output as rle_rgb565.
### overview rle source_over
```{mermaid}
graph TD
rle_alpha --> b{Identity matrix judgment}
b --> |N|rle_alpha__no_matrix
rle_alpha__no_matrix -->fiter_no_matrix_output{output}
fiter_no_matrix_output -->rle_alpha_rgb565
fiter_no_matrix_output -->rle_alpha_rgb888
fiter_no_matrix_output -->rle_alpha_rgba8888
b --> |Y|rle_alpha_matrix
rle_alpha_matrix --> fiter_matrix_output{output}
fiter_matrix_output -->rle_alpha_matrix_rgb565
fiter_matrix_output -->rle_alpha_matirx_rgb888
fiter_matrix_output -->rle_alpha_matrix_rgba8888
```
### rle alpha no matrix
```{mermaid}
graph TD
rle_alpha_rgb565 --> rle_alpha_565{input}
rle_alpha_565 --> | |rle_alpha_565_565
rle_alpha_565_565 --> rle_alpha_opacity_case565{opacity}
rle_alpha_opacity_case565 --> |opacity + alpha| rle_alpha_opacity_value0_255(0-255)
rle_alpha_opacity_case565 --> |alpha|rle_alpha_opacity_value255(255)
rle_alpha_opacity_case565 --> |break|rle_alpha_opacity_value0(0)
rle_alpha_565 --> | |rle_alpha_565_888
rle_alpha_565_888--> rle_alpha_opacity_case888{opacity}
rle_alpha_opacity_case888 --> |opacity + alpha|rle_alpha_opacity_value888_0_255(0-255)
rle_alpha_opacity_case888 --> |alpha|rle_alpha_opacity_value888_255(255)
rle_alpha_opacity_case888 --> |break|rle_alpha_opacity_value888_0(0)
rle_alpha_565 --> | |rle_alpha_565_8888
rle_alpha_565_8888 --> rle_alpha_opacity_case8888{opacity}
rle_alpha_opacity_case8888 --> |opacity + alpha|rle_alpha_opacity_value8888_0_255(0-255)
rle_alpha_opacity_case8888 --> |alpha|rle_alpha_opacity_value8888_255(255)
rle_alpha_opacity_case8888 --> |break|rle_alpha_opacity_value8888_0(0)
```
Notice: In compressed source_over mode output rle_rgb888 and rle_rgba8888 equivalent to output as rle_rgb565.
### rle_alpha matrix
```{mermaid}
graph TD
rle_alpha_matrix_rgb565 --> rle_alpha_matrix_565{input}
rle_alpha_matrix_565 --> | |rle_alpha_matrix_565_565
rle_alpha_matrix_565_565 --> rle_alpha_matrix_opacity_case565{opacity}
rle_alpha_matrix_opacity_case565 --> |opacity + alpha| rle_alpha_matrix_opacity_value0_255(0-255)
rle_alpha_matrix_opacity_case565 --> |alpha|rle_alpha_matrix_opacity_value255(255)
rle_alpha_matrix_opacity_case565 --> |break|rle_alpha_matrix_opacity_value0(0)
rle_alpha_matrix_565 --> | |rle_alpha_matrix_565_888
rle_alpha_matrix_565_888--> rle_alpha_matrix_opacity_case888{opacity}
rle_alpha_matrix_opacity_case888 --> |opacity + alpha|rle_alpha_matrix_opacity_value888_0_255(0-255)
rle_alpha_matrix_opacity_case888 --> |alpha|rle_alpha_matrix_opacity_value888_255(255)
rle_alpha_matrix_opacity_case888 --> |break|rle_alpha_matrix_opacity_value888_0(0)
rle_alpha_matrix_565 --> | |rle_alpha_matrix_565_8888
rle_alpha_matrix_565_8888 --> rle_alpha_matrix_opacity_case8888{opacity}
rle_alpha_matrix_opacity_case8888 --> |opacity + alpha|rle_alpha_matrix_opacity_value8888_0_255(0-255)
rle_alpha_matrix_opacity_case8888 --> |alpha|rle_alpha_matrix_opacity_value8888_255(255)
rle_alpha_matrix_opacity_case8888 --> |break|rle_alpha_matrix_opacity_value8888_0(0)
```
Notice: In compressed source_over matrix mode output rle_rgb888 and rle_rgba8888 equivalent to output as rle_rgb565.

## image type

|  |  |  |  |
| --- | --- | --- | --- |
|  |  |  |  |
|  |  |  |  |
|  |  |  |  |
|  |  |  |  |
|  |  |  |  |
|  |  |  |  |
|  |  |  |  |


## todo
```chart
,Budget,Income,Expenses,Debt
June,5000,8000,4000,6000
July,3000,1000,4000,3000
Aug,5000,7000,6000,3000
Sep,7000,2000,3000,1000
Oct,6000,5000,4000,2000
Nov,4000,3000,5000,

type: pie
title: Monthly Revenue
x.title: Amount
y.title: Month
y.suffix: $
```




