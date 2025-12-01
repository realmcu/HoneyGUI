# HoneyGUI Image Converter

跨平台图片转换工具，将 PNG/JPG 图片转换为 HoneyGUI bin 格式。

## 设计规范

### bin 文件格式

#### 文件结构
```
[8字节头部][像素数据]
```

#### 头部结构 (gui_rgb_data_head_t)

| 偏移 | 大小 | 字段 | 说明 |
|------|------|------|------|
| 0 | 1 | flags | 位域：scan(1), align(1), resize(2), compress(1), jpeg(1), idu(1), rsvd(1) |
| 1 | 1 | type | 像素格式 (GUI_FormatType) |
| 2 | 2 | w | 宽度 (little-endian) |
| 4 | 2 | h | 高度 (little-endian) |
| 6 | 1 | version | 版本号 |
| 7 | 1 | rsvd2 | 保留 |

#### 像素格式 (type 字段)

| 值 | 名称 | 字节/像素 | 说明 |
|----|------|-----------|------|
| 0 | RGB565 | 2 | 16位，无Alpha |
| 1 | ARGB8565 | 3 | 24位，带Alpha |
| 3 | RGB888 | 3 | 24位，无Alpha |
| 4 | ARGB8888 | 4 | 32位，带Alpha |

#### 像素数据排列

- 字节序：Little-endian
- 颜色顺序：BGR / BGRA
- 扫描顺序：从左到右，从上到下

### 使用方法

```bash
# 基本用法
python image_converter.py -i input.png -o output.bin

# 指定格式
python image_converter.py -i input.png -o output.bin -f rgb565

# 自动检测（默认）
# - 有透明通道 → ARGB8888
# - 无透明通道 → RGB565
python image_converter.py -i input.png -o output.bin -f auto
```

### 参数说明

| 参数 | 必需 | 说明 |
|------|------|------|
| -i, --input | 是 | 输入图片路径 (PNG/JPG) |
| -o, --output | 是 | 输出 bin 文件路径 |
| -f, --format | 否 | 输出格式：auto, rgb565, rgb888, argb8888, argb8565 |

### 依赖

- Python 3.6+
- Pillow (`pip install Pillow`)

### 集成示例

VSCode 插件调用：
```typescript
import { spawn } from 'child_process';
import * as path from 'path';

const script = path.join(sdkPath, 'tool', 'image-convert-tool', 'image_converter.py');
const proc = spawn('python', [script, '-i', inputPath, '-o', outputPath, '-f', 'auto']);
```
