# HoneyGUI Image Converter - 快速参考

## 一行命令

```bash
# 最简单：自动选择最优压缩
python image_converter.py -i input.png -o output.bin -c adaptive

# UI 图标（推荐）
python image_converter.py -i icon.png -o icon.bin -f argb8888 -c rle

# 照片（推荐）
python image_converter.py -i photo.jpg -o photo.bin -f rgb565 -c yuv-fastlz --yuv-mode yuv422
```

## 压缩算法选择

| 图像类型 | 推荐命令 |
|---------|---------|
| 纯色/渐变 | `-c rle` |
| UI 图标 | `-c rle` 或 `-c adaptive` |
| 照片 | `-c yuv-fastlz --yuv-mode yuv422` |
| 不确定 | `-c adaptive` |

## 像素格式选择

| 需求 | 格式 | 说明 |
|------|------|------|
| 最小体积 | `rgb565` | 2字节/像素，无透明 |
| 高质量无透明 | `rgb888` | 3字节/像素 |
| 透明图标 | `argb8888` | 4字节/像素，最高质量 |
| 透明+小体积 | `argb8565` | 3字节/像素，平衡 |
| 仅透明通道 | `a8` | 1字节/像素，蒙版 |

## 常用参数

```bash
-i <file>           # 输入文件
-o <file>           # 输出文件
-f <format>         # 像素格式：auto, rgb565, rgb888, argb8888, argb8565, a8
-c <compress>       # 压缩：none, rle, fastlz, yuv, yuv-fastlz, adaptive
--rle-run1 <0-3>    # RLE Run Length (默认3)
--yuv-mode <mode>   # YUV模式：yuv444, yuv422, yuv411
--blur-bits <0-4>   # Blur位数：0, 1, 2, 4
```

## 批量转换

```bash
# 转换所有 PNG
for f in *.png; do python image_converter.py -i "$f" -o "${f%.png}.bin" -c adaptive; done

# 转换所有图片
for f in assets/*; do python image_converter.py -i "$f" -o "build/$(basename $f).bin" -c adaptive; done
```

## 文件大小对比

假设 100x100 像素图片：

| 格式 | 未压缩 | RLE | FastLz | YUV422 |
|------|--------|-----|--------|--------|
| RGB565 | 20KB | 5-15KB | 8-18KB | - |
| RGB888 | 30KB | 8-25KB | 12-27KB | 10-20KB |
| ARGB8888 | 40KB | 10-35KB | 15-36KB | - |

*实际压缩率取决于图片内容*

## 故障排除

```bash
# 检查依赖
pip list | grep -E "Pillow|fastlz"

# 测试工具
python test_compress.py

# 查看帮助
python image_converter.py --help
```
