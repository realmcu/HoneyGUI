# Design Spec

## Design details

## sw acc

```{mermaid}
graph TD
A[mode A] -->|A1| B(model B)
B --> C{Judgment conditions C}
C -->|conditionC1| D[module D]
C -->|conditionC2| E[module E]
C -->|conditionC3| F[module F]
```
### no_rle_bypass mode
```{mermaid}
graph TD
NO_RLE --> | | NO_BYPASS(bypassD)
NO_BYPASS --> b{output}
b --> |Identity matrix judgment| J(RGB565)
J --> e{input}
e --> | |ea(RGB565)
ea --> no_bypass_opacity_case565{opacity}
no_bypass_opacity_case565 --> |opacity|no_bypass_opacity_value0_255(0-255)
no_bypass_opacity_case565 --> |no opacity|no_bypass_opacity_value255(255)
no_bypass_opacity_case565 --> |break|no_bypass_opacity_value0(0)

e --> | |eb(RGB888)
eb --> no_bypass_opacity_case888{opacity}
no_bypass_opacity_case888 --> |opacity|no_bypass_opacity_value888_0_255(0-255)
no_bypass_opacity_case888 --> |no opacity|no_bypass_opacity_value888_255(255)
no_bypass_opacity_case888 --> |break|no_bypass_opacity_value888_0(0)
e --> | |ec(RGBA8888)
ec --> no_bypass_opacity_case8888{opacity}
no_bypass_opacity_case8888 --> |opacity|no_bypass_opacity_value8888_0_255(0-255)
no_bypass_opacity_case8888 --> |no opacity|no_bypass_opacity_value8888_255(255)
no_bypass_opacity_case8888 --> |break|no_bypass_opacity_value8888_0(0)

b --> |Identity matrix judgment| K(RGB888)
K --> f{input format}
f --> fa(RGB565)
fa--> no_bypass_opacity_case888_565{opacity}
no_bypass_opacity_case888_565 --> |opacity|no_bypass_opacity_value888_565_a0_255(0-255)
no_bypass_opacity_case888_565 --> |no opacity|no_bypass_opacity_value888_888_a255(255)
no_bypass_opacity_case888_565 --> |break|no_bypass_opacity_value888_8888_a0(0)

f --> fb(RGB888)
fb--> no_bypass_opacity_case888_888{opacity}
no_bypass_opacity_case888_888 --> |opacity|no_bypass_opacity_value888_888_b0_255(0-255)
no_bypass_opacity_case888_888 --> |no opacity|no_bypass_opacity_value888_888_b255(255)
no_bypass_opacity_case888_888 --> |break|no_bypass_opacity_value888_8888_b0(0)

f --> fc(RGBA8888)
fc--> no_bypass_opacity_case888_8888{opacity}
no_bypass_opacity_case888_8888 --> |opacity|no_bypass_opacity_value888_8888_c0_255(0-255)
no_bypass_opacity_case888_8888 --> |no opacity|no_bypass_opacity_value888_8888_c255(255)
no_bypass_opacity_case888_8888 --> |break|no_bypass_opacity_value888_8888_c0(0)

b --> |Identity matrix judgment| L(RGBA8888)
L --> g{input format}
g --> ga(RGB565)
ga --> no_bypass_opacity_case8888_565{opacity}
no_bypass_opacity_case8888_565 --> |opacity|no_bypass_opacity_value8888_8888_a0_255(0-255)
no_bypass_opacity_case8888_565 --> |no opacity|no_bypass_opacity_value8888_8888_a255(255)
no_bypass_opacity_case8888_565 --> |break|no_bypass_opacity_value8888_8888_a0(0)

g --> gb(RGB888)
gb --> no_bypass_opacity_case8888_888{opacity}
no_bypass_opacity_case8888_888 --> |opacity|no_bypass_opacity_value8888_8888_b0_255(0-255)
no_bypass_opacity_case8888_888--> |no opacity|no_bypass_opacity_value8888_8888_b255(255)
no_bypass_opacity_case8888_888 --> |break|no_bypass_opacity_value8888_8888_b0(0)

g --> gc(RGBA8888)
gc --> no_bypass_opacity_case8888_8888{opacity}
no_bypass_opacity_case8888_8888 --> |opacity|no_bypass_opacity_value8888_8888_c0_255(0-255)
no_bypass_opacity_case8888_8888--> |no opacity|no_bypass_opacity_value8888_8888_c255(255)
no_bypass_opacity_case8888_8888 --> |break|no_bypass_opacity_value8888_8888_c0(0)
```

### no_rle_filter
```{mermaid}
graph TD
NO_RLE --> | | no_filter(filter)
no_filter --> c{output format}
c --> |matix| filter_matrix_ture(ture)
filter_matrix_ture --> | |filter_matrix_ture565(rgb565)
filter_matrix_ture565 --> filter_matrix_judge{input format}
filter_matrix_judge --> filter_matrix_565_565(rgb565)
filter_matrix_565_565 --> filter_matrix_565_565_opacity{opacity}
filter_matrix_565_565_opacity --> |opacity |filter_matrix_565_565_a0_255(0-255)
filter_matrix_565_565_opacity --> |no opacity |filter_matrix_565_565_a255(255)
filter_matrix_565_565_opacity --> |break |filter_matrix_565_565_a0(0)

filter_matrix_judge --> filter_matrix_565_888(rgb888)
filter_matrix_565_888 --> filter_matrix_565_888_opacity{opacity}
filter_matrix_565_888_opacity --> |opacity |filter_matrix_565_888_a0_255(0-255)
filter_matrix_565_888_opacity --> |no opacity |filter_matrix_565_888_a255(255)
filter_matrix_565_888_opacity --> |break |filter_matrix_565_888_a0(0)

filter_matrix_judge --> filter_matrix_565_8888(rgba8888)
filter_matrix_565_8888 --> filter_matrix_565_8888_opacity{opacity}
filter_matrix_565_8888_opacity --> |opacity |filter_matrix_565_8888_a0_255(0-255)
filter_matrix_565_8888_opacity --> |no opacity |filter_matrix_565_8888_a255(255)
filter_matrix_565_8888_opacity --> |break |filter_matrix_565_8888_a0(0)

filter_matrix_ture --> | |filter_matrix_ture888(rgb888)
filter_matrix_ture888 --> filter_matrix_judge_888{input format}
filter_matrix_judge_888 -->filter_matrix_888_565
filter_matrix_888_565 -->filter_matrix_888_565_opacity{opacity}
filter_matrix_888_565_opacity --> |opacity|filter_matrix_888_565_0_255(0-255)
filter_matrix_888_565_opacity --> |no opacity|filter_matrix_888_565_255(255)
filter_matrix_888_565_opacity --> |break|filter_matrix_888_565_0(0)

filter_matrix_judge_888 -->filter_matrix_888_888
filter_matrix_888_888 --> filter_matrix_888_888_opacity{opacity}
filter_matrix_888_888_opacity --> |opacity|filter_matrix_888_888_0_255(0-255)
filter_matrix_888_888_opacity --> |no opacity|filter_matrix_888_888_255(255)
filter_matrix_888_888_opacity --> |break|filter_matrix_888_888_0(0)

filter_matrix_judge_888 -->filter_matrix_888_8888
filter_matrix_888_8888 --> filter_matrix_888_8888_opacity{opacity}
filter_matrix_888_8888_opacity --> |opacity|filter_matrix_888_8888_0_255(0-255)
filter_matrix_888_8888_opacity --> |no opacity|filter_matrix_888_8888_255(255)
filter_matrix_888_8888_opacity --> |break|filter_matrix_888_8888_0(0)

filter_matrix_ture --> | |filter_matrix_ture8888(rgba8888)
filter_matrix_ture8888 -->filter_matrix_judge_8888{input format}
filter_matrix_judge_8888 --> filter_matrix_8888_565
filter_matrix_8888_565 --> filter_matrix_8888_565_opacity{opacity}
filter_matrix_8888_565_opacity --> |opacity|filter_matrix_8888_565_0_255(0-255)
filter_matrix_8888_565_opacity --> |no opacity|filter_matrix_8888_565_255(255)
filter_matrix_8888_565_opacity --> |break|filter_matrix_8888_565_0(0)

filter_matrix_judge_8888 --> filter_matrix_8888_888
filter_matrix_8888_888 --> filter_matrix_8888_888_opacity{opacity}
filter_matrix_8888_888_opacity --> |opacity|filter_matrix_8888_888_0_255(0-255)
filter_matrix_8888_888_opacity --> |no opacity|filter_matrix_8888_888_255(255)
filter_matrix_8888_888_opacity --> |break|filter_matrix_8888_888_0(0)

filter_matrix_judge_8888 --> filter_matrix_8888_8888
filter_matrix_8888_8888 --> filter_matrix_8888_8888_opacity{opacity}
filter_matrix_8888_8888_opacity --> |opacity|filter_matrix_8888_8888_0_255(0-255)
filter_matrix_8888_8888_opacity --> |no opacity|filter_matrix_8888_8888_255(255)
filter_matrix_8888_8888_opacity --> |break|filter_matrix_8888_8888_0(0)

c --> | no_matrix| filter_matrix_F_1(false)
```

### no rle filter_matrix
```{mermaid}
graph TD

filter_matrix_F[flase] --> | |filter_matrix_F565(rgb565)
filter_matrix_F565 --> filter_judge_565{input format}
filter_judge_565 --> filter_565_565
filter_565_565 --> filter_565_565_opacity{opacity}
filter_565_565_opacity -->|opacity|filter_565_565_0_255(0-255)
filter_565_565_opacity -->|no opacity|filter_565_565_255(255)
filter_565_565_opacity -->|break|filter_565_565_0(0)

filter_judge_565 --> filter_565_888
filter_565_888 --> filter_565_888_opacity{opacity}
filter_565_888_opacity -->|opacity|filter_565_888_0_255(0-255)
filter_565_888_opacity -->|no opacity|filter_565_888_255(255)
filter_565_888_opacity -->|break|filter_565_888_0(0)

filter_judge_565 --> filter_565_8888
filter_565_8888 --> filter_565_8888_opacity{opacity}
filter_565_8888_opacity -->|opacity|filter_565_8888_0_255(0-255)
filter_565_8888_opacity -->|no opacity|filter_565_8888_255(255)
filter_565_8888_opacity -->|break|filter_565_8888_0(0)

filter_matrix_F(flase) --> | |filter_matrix_F888(rgb888)
filter_matrix_F888 --> filter_judge_888{input format}
filter_judge_888 --> filter_888_565
filter_888_565 --> filter_888_565_opacity{opacity}
filter_888_565_opacity -->|opacity|filter_888_565_0_255(0-255)
filter_888_565_opacity -->|no opacity|filter_888_565_255(255)
filter_888_565_opacity -->|break|filter_888_565_0(0)

filter_judge_888 --> filter_888_888
filter_888_888 --> filter_888_888_opacity{opacity}
filter_888_888_opacity -->|opacity|filter_888_888_0_255(0-255)
filter_888_888_opacity -->|no opacity|filter_888_888_255(255)
filter_888_888_opacity -->|break|filter_888_888_0(0)

filter_judge_888 --> filter_888_8888
filter_888_8888 --> filter_888_8888_opacity{opacity}
filter_888_8888_opacity -->|opacity|filter_888_8888_0_255(0-255)
filter_888_8888_opacity -->|no opacity|filter_888_8888_255(255)
filter_888_8888_opacity -->|break|filter_888_8888_0(0)

filter_matrix_F(false) --> | |filter_matrix_F8888(rgba8888)
filter_matrix_F8888 --> filter_judge_8888{input format}
filter_judge_8888 --> filter_8888_565
filter_8888_565 --> filter_8888_565_opacity{opacity}
filter_8888_565_opacity --> |opacity|filter_8888_565_0_255(0-255)
filter_8888_565_opacity --> |no opacity|filter_8888_565_255(255)
filter_8888_565_opacity --> |break|filter_8888_565_0(0)

filter_judge_8888 --> filter_8888_888
filter_8888_888 --> filter_8888_888_opacity{opacity}
filter_8888_888_opacity --> |opacity|filter_8888_888_0_255(0-255)
filter_8888_888_opacity --> |no opacity|filter_8888_888_255(255)
filter_8888_888_opacity --> |break|filter_8888_888_0(0)

filter_judge_8888 --> filter_8888_8888
filter_8888_8888 --> filter_8888_8888_opacity{opacity}
filter_8888_8888_opacity --> |opacity|filter_8888_8888_0_255(0-255)
filter_8888_8888_opacity --> |no opacity|filter_8888_8888_255(255)
filter_8888_8888_opacity --> |break|filter_8888_8888_0(0)
```


### no_rle_source_over_matrix
```{mermaid}
graph TD
source_over --> c{output format}
c --> |matix| alpha_matrix_ture(ture)
c --> |matix| alpha_matrix_false(false)

alpha_matrix_ture --> | |alpha_matrix_ture565(rgb565)
alpha_matrix_ture565 --> alpha_matrix_judge{input format}
alpha_matrix_judge --> alpha_matrix_565_565(rgb565)
alpha_matrix_565_565 --> alpha_matrix_565_565_opacity{opacity}
alpha_matrix_565_565_opacity --> |opacity+alpha|alpha_matrix_565_565_a0_255(0-255)
alpha_matrix_565_565_opacity --> |alpha|alpha_matrix_565_565_a255(255)
alpha_matrix_565_565_opacity --> |break|alpha_matrix_565_565_a0(0)

alpha_matrix_judge --> alpha_matrix_565_888(rgb888)
alpha_matrix_565_888 --> alpha_matrix_565_888_opacity{opacity}
alpha_matrix_565_888_opacity --> |opacity+alpha|alpha_matrix_565_888_a0_255(0-255)
alpha_matrix_565_888_opacity --> |alpha blending|alpha_matrix_565_888_a255(255)
alpha_matrix_565_888_opacity --> |break|alpha_matrix_565_888_a0(0)

alpha_matrix_judge --> alpha_matrix_565_8888(rgba8888)
alpha_matrix_565_8888 --> alpha_matrix_565_8888_opacity{opacity}
alpha_matrix_565_8888_opacity --> |opacity+alpha|alpha_matrix_565_8888_a0_255(0-255)
alpha_matrix_565_8888_opacity --> |alpha blending|alpha_matrix_565_8888_a255(255)
alpha_matrix_565_8888_opacity --> |no opacity|alpha_matrix_565_8888_a0(0)

alpha_matrix_ture --> | |alpha_matrix_ture888(rgb888)
alpha_matrix_ture888 --> alpha_matrix_judge_888{input format}
alpha_matrix_judge_888 -->alpha_matrix_888_565
alpha_matrix_888_565 -->alpha_matrix_888_565_opacity{opacity}
alpha_matrix_888_565_opacity --> |opacity+alpha| alpha_matrix_888_565_0_255(0-255)
alpha_matrix_888_565_opacity --> |alpha blending|alpha_matrix_888_565_255(255)
alpha_matrix_888_565_opacity --> |break|alpha_matrix_888_565_0(0)

alpha_matrix_judge_888 -->alpha_matrix_888_888
alpha_matrix_888_888 --> alpha_matrix_888_888_opacity{opacity}
alpha_matrix_888_888_opacity --> |opacity+alpha| alpha_matrix_888_888_0_255(0-255)
alpha_matrix_888_888_opacity --> |alpha blending| alpha_matrix_888_888_255(255)
alpha_matrix_888_888_opacity --> |break| alpha_matrix_888_888_0(0)

alpha_matrix_judge_888 -->alpha_matrix_888_8888
alpha_matrix_888_8888 --> alpha_matrix_888_8888_opacity{opacity}
alpha_matrix_888_8888_opacity --> |opacity+alpha|alpha_matrix_888_8888_0_255(0-255)
alpha_matrix_888_8888_opacity --> |alpha blending|alpha_matrix_888_8888_255(255)
alpha_matrix_888_8888_opacity --> |break|alpha_matrix_888_8888_0(0)

alpha_matrix_ture --> | |alpha_matrix_ture8888(rgba8888)
alpha_matrix_ture8888 --> alpha_matrix_judge_8888{input format}
alpha_matrix_judge_8888 --> alpha_matrix_8888_565
alpha_matrix_8888_565 --> alpha_matrix_8888_565_opacity{opacity}
alpha_matrix_8888_565_opacity --> |opacity+alpha| alpha_matrix_8888_565_0_255(0-255)
alpha_matrix_8888_565_opacity --> |alpha blending| alpha_matrix_8888_565_255(255)
alpha_matrix_8888_565_opacity --> |break| alpha_matrix_8888_565_0(0)

alpha_matrix_judge_8888 --> alpha_matrix_8888_888
alpha_matrix_8888_888 --> alpha_matrix_8888_888_opacity{opacity}
alpha_matrix_8888_888_opacity --> |opacity+alpha| alpha_matrix_8888_888_0_255(0-255)
alpha_matrix_8888_888_opacity --> |alpha| alpha_matrix_8888_888_255(255)
alpha_matrix_8888_888_opacity --> |break|alpha_matrix_8888_888_0(0)

alpha_matrix_judge_8888 --> alpha_matrix_8888_8888
alpha_matrix_8888_8888 --> alpha_matrix_8888_8888_opacity{opacity}
alpha_matrix_8888_8888_opacity --> |opacity+alpha| alpha_matrix_8888_8888_0_255(0-255)
alpha_matrix_8888_8888_opacity --> |alpha| alpha_matrix_8888_8888_255(255)
alpha_matrix_8888_8888_opacity --> |break| alpha_matrix_8888_8888_0(0)
```

### rle
```{mermaid}
graph TD
RLE --> | | RLE_BYPASS(bypassG)
RLE --> | | RLE_FILTER(filterH)
RLE --> | | RLE_SOURCE_OVER(source_overI)
```

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




