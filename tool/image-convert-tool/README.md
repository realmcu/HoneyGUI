# HoneyGUI Image Converter

跨平台图片转换工具，将 PNG/JPG/BMP 等图片转换为 HoneyGUI bin 格式，支持多种压缩算法。

## 功能特性

- 支持多种输入格式：PNG、JPEG、BMP、GIF、TIFF 等（基于 Pillow）
- 支持多种输出像素格式：RGB565、RGB888、ARGB8888、ARGB8565、A8
- 支持多种压缩算法：RLE、FastLz、YUV、Adaptive
- 模块化架构，易于扩展
- 命令行工具，易于集成到构建流程

## 安装依赖

```bash
# 基础依赖
pip install Pillow

# 可选：FastLz 压缩支持
pip install fastlz
```

## bin 文件格式规范

### 文件结构

#### 未压缩格式
```
[gui_rgb_data_head_t (8字节)]
[像素数据]
```

#### 压缩格式
```
[gui_rgb_data_head_t (8字节)]
[imdc_file_header_t (12字节)]
[compressed_addr[] (每行一个uint32地址)]
[压缩数据]
```

### 头部结构

#### gui_rgb_data_head_t (8 bytes)

| 偏移 | 大小 | 字段 | 说明 |
|------|------|------|------|
| 0 | 1 | flags | 位域：scan(1), align(1), resize(2), compress(1), jpeg(1), idu(1), rsvd(1) |
| 1 | 1 | type | 像素格式 (GUI_FormatType) |
| 2 | 2 | w | 宽度 (little-endian) |
| 4 | 2 | h | 高度 (little-endian) |
| 6 | 1 | version | 版本号 |
| 7 | 1 | rsvd2 | 保留 |

#### flags 字段

| Bit | 名称 | 说明 |
|-----|------|------|
| 0 | scan | 扫描方向 |
| 1 | align | 对齐方式 |
| 2-3 | resize | 缩放模式 (0=无, 1=50%, 2=70%, 3=80%) |
| 4 | compress | 是否压缩 (1=压缩, 0=未压缩) |
| 5 | jpeg | JPEG 格式标志 |
| 6 | idu | IDU 硬件加速标志 |
| 7 | rsvd | 保留位 |

#### imdc_file_header_t (12 bytes)

| 偏移 | 大小 | 字段 | 说明 |
|------|------|------|------|
| 0 | 1 | algorithm_type | 压缩算法配置（位域） |
| 1-3 | 3 | reserved | 保留字节 |
| 4 | 4 | raw_pic_width | 原始图片宽度 |
| 8 | 4 | raw_pic_height | 原始图片高度 |

#### algorithm_type 字段

| Bits | 字段 | 值 | 说明 |
|------|------|-----|------|
| 0-1 | algorithm | 0 | RLE 压缩 |
|  |  | 1 | FastLz 压缩 |
|  |  | 2 | YUV_Sample + FastLz |
|  |  | 3 | YUV_Sample |
| 2-3 | feature_1 | 0-3 | 1阶 RLE Run Length |
| 4-5 | feature_2 | 0-3 | 2阶 RLE Run Length |
| 6-7 | pixel_bytes | 0 | 2 字节/像素 (RGB565, ARGB8565) |
|  |  | 1 | 3 字节/像素 (RGB888) |
|  |  | 2 | 4 字节/像素 (ARGB8888) |
|  |  | 3 | 1 字节/像素 (A8, GRAY) |

### 像素格式 (type 字段)

#### 基础 RGB 格式

| 值 | 名称 | 字节/像素 | 说明 |
|----|------|-----------|------|
| 0 | RGB565 | 2 | 16位，无Alpha |
| 1 | ARGB8565 | 3 | 24位，带Alpha |
| 3 | RGB888 | 3 | 24位，无Alpha |
| 4 | ARGB8888 | 4 | 32位，带Alpha |
| 5 | XRGB8888 | 4 | 32位，X通道固定为0xFF |

#### 灰度/蒙版格式

| 值 | 名称 | 字节/像素 | 说明 |
|----|------|-----------|------|
| 6 | BINARY | 1/8 | 1位单色（黑白） |
| 7 | GRAY | 1 | 8位灰度 |
| 9 | ALPHAMASK | 1 | 8位Alpha通道 |
| 10 | PALETTE | 1 | 调色板索引 |

#### 编码格式

| 值 | 名称 | 说明 |
|----|------|------|
| 11 | BMP | BMP 图片文件格式 |
| 12 | JPEG | JPEG 图片文件格式 |
| 13 | PNG | PNG 图片文件格式 |
| 14 | GIF | GIF 图片文件格式 |

#### 特殊格式

| 值 | 名称 | 说明 |
|----|------|------|
| 15 | RTKARGB8565 | 平面格式：[A 8bpp plane][RGB565 plane] |

### 像素数据排列

- **字节序**: Little-endian
- **颜色顺序**: BGR / BGRA
- **扫描顺序**: 从左到右，从上到下

## 压缩算法

### 1. RLE 压缩 (Run-Length Encoding)

**节点格式**:
- RGB565: `[len:1] [pixel:2]`
- ARGB8565: `[len:1] [pixel:2] [alpha:1]`
- RGB888: `[len:1] [b:1] [g:1] [r:1]`
- ARGB8888: `[len:1] [pixel:4]`

**参数**:
- `run_length_1`: 1阶 RLE Run Length (0-3)
- `run_length_2`: 2阶 RLE Run Length (0-3)
- `level`: RLE 级别 (1 or 2)

**适用场景**: 纯色、渐变、简单图形、UI 元素

**使用示例**:
```bash
python image_converter_new.py -i input.png -o output.bin -c rle --rle-run1 3
```

### 2. FastLz 压缩

快速无损压缩算法，基于 LZ77。

**特点**:
- 压缩速度快
- 解压速度快
- 无需配置参数

**适用场景**: 有重复模式的图像

**使用示例**:
```bash
python image_converter_new.py -i input.png -o output.bin -c fastlz
```

### 3. YUV 降采样 + Blur

**YUV 格式**:
- **YUV444**: `[Y U V]` 每像素3字节，无降采样
- **YUV422**: `[Y0 U0 Y1 V0 Y2 U1 Y3 V1]` 每4像素8字节
- **YUV411**: `[Y0 U0 Y1 Y2 V0 Y3]` 每4像素6字节

**参数**:
- `sample_mode`: YUV444, YUV422, YUV411
- `blur_bits`: 丢弃低位 bit 数 (0, 1, 2, 4)
- `use_fastlz`: 是否在 YUV 后再用 FastLz 压缩

**适用场景**: 照片类图像，可接受轻微质量损失

**使用示例**:
```bash
# YUV 降采样
python image_converter_new.py -i photo.jpg -o photo.bin -c yuv --yuv-mode yuv422 --blur-bits 2

# YUV + FastLz
python image_converter_new.py -i photo.jpg -o photo.bin -c yuv-fastlz --yuv-mode yuv422
```

### 4. Adaptive 自适应压缩

自动尝试 RLE 和 FastLz，选择压缩率更高的算法。

**使用示例**:
```bash
python image_converter_new.py -i input.png -o output.bin -c adaptive
```

### 压缩效果对比

| 图像类型 | 推荐算法 | 压缩率 |
|---------|---------|--------|
| 纯色/渐变 | RLE | 高 (>70%) |
| 简单图形/UI | RLE / Adaptive | 中-高 (40-70%) |
| 照片 | YUV422 + FastLz | 中 (30-50%) |
| 复杂纹理 | FastLz / 不压缩 | 低-中 (10-30%) |
| 噪点图像 | 不压缩 | 可能增大 |

## 使用方法

### 基本用法

```bash
# 无压缩，自动检测格式
python image_converter_new.py -i input.png -o output.bin

# 指定像素格式
python image_converter_new.py -i input.png -o output.bin -f rgb565

# 提取 Alpha 通道
python image_converter_new.py -i icon.png -o alpha.bin -f a8
```

### 命令行参数

#### 基础参数

| 参数 | 必需 | 说明 |
|------|------|------|
| -i, --input | 是 | 输入图片路径 |
| -o, --output | 是 | 输出 bin 文件路径 |
| -f, --format | 否 | 像素格式：auto, rgb565, rgb888, argb8888, argb8565, a8 |
| -c, --compress | 否 | 压缩算法：none, rle, fastlz, yuv, yuv-fastlz, adaptive |

#### RLE 参数

| 参数 | 默认值 | 说明 |
|------|--------|------|
| --rle-level | 1 | RLE 级别 (1 or 2) |
| --rle-run1 | 3 | 1阶 Run Length (0-3) |
| --rle-run2 | 3 | 2阶 Run Length (0-3) |

#### YUV 参数

| 参数 | 默认值 | 说明 |
|------|--------|------|
| --yuv-mode | yuv444 | 采样模式：yuv444, yuv422, yuv411 |
| --blur-bits | 0 | 丢弃低位 bit 数 (0, 1, 2, 4) |

### 使用示例

```bash
# 1. UI 图标（纯色多）
python image_converter_new.py -i icon.png -o icon.bin -f argb8888 -c rle

# 2. 照片
python image_converter_new.py -i photo.jpg -o photo.bin -f rgb565 -c yuv-fastlz --yuv-mode yuv422

# 3. 不确定类型，自动选择
python image_converter_new.py -i image.png -o image.bin -c adaptive

# 4. 批量转换
for file in assets/*.png; do
    python image_converter_new.py -i "$file" -o "${file%.png}.bin" -c adaptive
done
```

## 模块化架构

```
image-convert-tool/
├── formats/                    # 格式定义模块
│   ├── format_types.py        # 常量定义
│   ├── headers.py             # 头部结构
│   └── pixel_converter.py     # 像素转换
├── compress/                   # 压缩算法模块
│   ├── base.py                # 基类
│   ├── rle.py                 # RLE 压缩
│   ├── fastlz.py              # FastLz 压缩
│   ├── yuv.py                 # YUV 压缩
│   └── adaptive.py            # 自适应压缩
├── converter.py               # 主转换器
├── image_converter.py         # 旧版（简化版）
├── image_converter_new.py     # 新版（完整功能）
├── test_compress.py           # 测试脚本
└── README.md                  # 本文档
```

## 测试

```bash
# 运行测试
python test_compress.py

# 测试特定算法
python image_converter_new.py -i test.png -o test_rle.bin -c rle
python image_converter_new.py -i test.png -o test_fastlz.bin -c fastlz
python image_converter_new.py -i test.png -o test_adaptive.bin -c adaptive
```

## 集成示例

### VSCode 插件调用

```typescript
import { spawn } from 'child_process';
import * as path from 'path';

const script = path.join(sdkPath, 'tool', 'image-convert-tool', 'image_converter_new.py');
const proc = spawn('python3', [
    script,
    '-i', inputPath,
    '-o', outputPath,
    '-f', 'auto',
    '-c', 'adaptive'
]);
```

### 构建脚本集成

```bash
#!/bin/bash
# convert_assets.sh

TOOL="python3 image_converter_new.py"
ASSETS_DIR="assets"
OUTPUT_DIR="build/assets"

mkdir -p "$OUTPUT_DIR"

for img in "$ASSETS_DIR"/*.png; do
    name=$(basename "$img" .png)
    $TOOL -i "$img" -o "$OUTPUT_DIR/$name.bin" -c adaptive
done
```

## 注意事项

1. **压缩效果**: 某些图片压缩后可能比原图更大，建议使用 adaptive 模式
2. **FastLz 依赖**: 需要安装 `pip install fastlz`
3. **YUV 质量**: YUV 压缩会有轻微质量损失，适合照片不适合 UI
4. **Blur 模式**: blur_bits 越大，质量损失越大，但压缩率越高
5. **兼容性**: 生成的 bin 文件与 HoneyGUI SDK 完全兼容

## 参考资料

- [HoneyGUI SDK 文档](https://docs.realmcu.com/)
- [官方 Image Convert Tool 文档](https://docs.realmcu.com/tools/media_convert_tool/)
- [GUI_FormatType 定义](../../realgui/engine/draw_img.h)
- [解压实现参考](../../realgui/engine/sw/idu/acc_sw_idu.c)
- [RLE 解压参考](../../realgui/engine/sw/rle/acc_sw_rle.c)

## 版本历史

### v2.0.0 (2025-12-03)
- 完全模块化重构
- 实现 RLE 压缩
- 实现 FastLz 压缩
- 实现 YUV 降采样 + Blur 压缩
- 实现 Adaptive 自适应压缩
- 添加 A8 格式支持
- 完善文档

### v1.0.0
- 基础图片转换功能
- 支持 RGB565/RGB888/ARGB8888/ARGB8565 格式
- 无压缩功能
