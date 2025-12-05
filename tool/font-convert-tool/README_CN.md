# 字体转换器 - Python 版本

一个基于 Python 的字体转换工具，可将 TrueType 字体转换为位图字体文件，适用于嵌入式系统和可穿戴设备。

## 功能特性

- **多种渲染模式**：支持 1 位、2 位、4 位和 8 位位图字体
- **字符集自定义**：通过 CodePage 文件、映射文件、符号文件或自定义 Unicode 范围配置字符集
- **字体样式**：粗体和斜体效果
- **图像处理**：旋转（0°、90°、180°、270°）和伽马校正
- **优化功能**：字符裁剪以减小文件大小
- **灵活索引**：地址模式或偏移模式的字符查找
- **C++ 兼容**：生成与 C++ fontDictionary 工具兼容的字体文件

## 系统要求

- Python 3.7 或更高版本
- 依赖项（见 `requirements.txt`）：
  - freetype-py >= 2.3.0
  - numpy >= 1.21.0
  - Pillow >= 9.0.0

## 安装方法

### 方式 1：作为包安装

```bash
pip install -r requirements.txt
python setup.py install
```

### 方式 2：直接运行

```bash
pip install -r requirements.txt
python -m font_converter <配置文件.json>
```

## 快速开始

1. **准备 TrueType 字体文件**（例如：`HarmonyOS.ttf`）

2. **创建配置文件**（例如：`config.json`）：

```json
{
  "fontFile": "HarmonyOS.ttf",
  "fontSize": 32,
  "renderMode": 2,
  "outputFormat": "bitmap",
  "outputFolder": "output",
  "codepages": ["CP437.txt"],
  "crop": true,
  "indexMethod": 0
}
```

3. **运行转换器**：

```bash
python -m font_converter config.json
```

4. **输出文件**：
   - `HarmonyOS_32_r2.font` - 二进制字体文件
   - `HarmonyOS_32_r2.cst` - 字符集文件
   - `NotSupportedChars.txt` - 字体中不存在的字符列表（如果有）

## 配置参数说明

### 基本设置

| 字段 | 类型 | 必需 | 说明 |
|------|------|------|------|
| `fontFile` | 字符串 | 是 | TrueType 字体文件路径（.ttf） |
| `fontSize` | 整数 | 是 | 字体大小（像素），例如 16、24、32 |
| `renderMode` | 整数 | 是 | 位深度：1、2、4 或 8 |
| `outputFormat` | 字符串 | 是 | 输出格式："bitmap" 或 "vector" |
| `outputFolder` | 字符串 | 是 | 输出目录路径 |

### 字符集配置

| 字段 | 类型 | 必需 | 说明 |
|------|------|------|------|
| `codepages` | 数组 | 否 | CodePage 文件列表（例如：["CP437.txt", "CP1252.txt"]） |
| `mappings` | 数组 | 否 | 字符替换映射文件列表 |
| `symbols` | 数组 | 否 | 特殊符号文件列表 |
| `customRanges` | 数组 | 否 | 自定义 Unicode 范围（例如：[[0x0020, 0x007E]]） |

### 字体样式

| 字段 | 类型 | 默认值 | 说明 |
|------|------|--------|------|
| `bold` | 布尔值 | false | 应用粗体效果 |
| `italic` | 布尔值 | false | 应用斜体效果 |

### 图像处理

| 字段 | 类型 | 默认值 | 说明 |
|------|------|--------|------|
| `rotation` | 整数 | 0 | 旋转角度：0、90、180 或 270 |
| `gamma` | 浮点数 | 1.0 | 伽马校正值（0.1 - 3.0） |

### 优化选项

| 字段 | 类型 | 默认值 | 说明 |
|------|------|--------|------|
| `crop` | 布尔值 | false | 启用字符裁剪以减小文件大小 |
| `indexMethod` | 整数 | 0 | 索引方法：0（地址模式）或 1（偏移模式） |
| `rvd` | 布尔值 | false | 反向垂直方向模式 |

## 使用示例

### 示例 1：基本位图字体（2 位）

```json
{
  "fontFile": "Arial.ttf",
  "fontSize": 24,
  "renderMode": 2,
  "outputFormat": "bitmap",
  "outputFolder": "output",
  "codepages": ["CP437.txt"],
  "crop": false,
  "indexMethod": 0
}
```

### 示例 2：粗斜体字体带旋转

```json
{
  "fontFile": "HarmonyOS.ttf",
  "fontSize": 32,
  "renderMode": 2,
  "outputFormat": "bitmap",
  "outputFolder": "output",
  "codepages": ["CP437.txt"],
  "bold": true,
  "italic": true,
  "rotation": 90,
  "crop": true,
  "indexMethod": 0
}
```

### 示例 3：中文字符

```json
{
  "fontFile": "SimHei.ttf",
  "fontSize": 16,
  "renderMode": 4,
  "outputFormat": "bitmap",
  "outputFolder": "output",
  "customRanges": [
    [0x4E00, 0x9FFF]
  ],
  "crop": true,
  "indexMethod": 0
}
```

### 示例 4：嵌入式系统裁剪字体

```json
{
  "fontFile": "Roboto.ttf",
  "fontSize": 16,
  "renderMode": 1,
  "outputFormat": "bitmap",
  "outputFolder": "output",
  "codepages": ["CP437.txt"],
  "crop": true,
  "indexMethod": 0,
  "gamma": 1.2
}
```

## CodePage 文件

CodePage 文件定义字符集。将它们放在配置文件相对路径的 `CodePage` 文件夹中。

**格式**：每行一个十六进制 Unicode 值（0xXXXX）

示例 `CP437.txt`：
```
0x0020
0x0021
0x0022
...
0x007E
```

## 输出文件格式

### 二进制字体文件（.font）

二进制字体文件包含：
1. **文件头**：字体元数据（大小、渲染模式、标志等）
2. **索引区**：字符查找表
3. **数据区**：每个字符的位图数据

### 字符集文件（.cst）

文本文件，列出字体中的所有 Unicode 值（每行一个十六进制值）。

### 不支持字符文件

列出无法渲染的字符（字体文件中缺失）。

## 渲染模式

| 模式 | 每像素位数 | 颜色数 | 使用场景 |
|------|-----------|--------|----------|
| 1 | 1 位 | 2（黑白） | 单色显示，最小存储 |
| 2 | 2 位 | 4（灰度） | 简单抗锯齿，低存储 |
| 4 | 4 位 | 16（灰度） | 良好抗锯齿，中等存储 |
| 8 | 8 位 | 256（灰度） | 最佳质量，较高存储 |

## 索引方法

### 地址模式（indexMethod: 0）
- 固定大小查找表（65536 个条目）
- 快速字符查找（O(1)）
- 较高内存使用
- 适用于：字符较多的字体或随机访问模式

### 偏移模式（indexMethod: 1）
- 可变大小查找表（N 个条目，N = 字符数量）
- 较慢字符查找（二分查找 O(log N)）
- 较低内存使用
- 适用于：字符较少的字体或顺序访问模式

## 常见问题

### 问题："找不到字体文件"
**解决方案**：确保配置中的字体文件路径正确且文件存在。

### 问题："字符集中没有字符"
**解决方案**：检查 CodePage 文件是否存在并包含有效的 Unicode 值，或指定 `customRanges`。

### 问题："字符不在字体中"
**解决方案**：字体文件不包含某些字符的字形。查看 `NotSupportedChars.txt` 获取列表。使用其他字体或从字符集中删除这些字符。

### 问题："输出文件太大"
**解决方案**：
- 启用 `crop` 模式以删除空白空间
- 使用较低的 `renderMode`（例如 1 位而不是 8 位）
- 减少字符集大小

### 问题："字符显示太大/太小"
**解决方案**：调整 `fontSize` 参数。工具会自动缩放字形以适应指定的大小框。

## 性能优化建议

1. **使用裁剪模式**：为嵌入式系统最小化文件大小
2. **选择合适的渲染模式**：简单显示使用 1 位，基本抗锯齿使用 2 位
3. **限制字符集**：只包含需要的字符
4. **使用地址模式**：如果内存允许，可获得更好的性能

## 兼容性

此 Python 实现生成的字体文件兼容：
- C++ fontDictionary 工具
- 使用 fontDictionary 格式的嵌入式系统
- 可穿戴设备和物联网显示器

## 许可证

此工具按原样提供，用于字体转换目的。

## 技术支持

如有问题、疑问或贡献，请参考项目仓库。

## 版本历史

### v1.0.0 (2025-12-04)
- 初始版本发布
- 完全 C++ 兼容
- 支持 1/2/4/8 位渲染的位图字体
- 字符裁剪和优化
- 支持粗体、斜体、旋转和伽马校正
