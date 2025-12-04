# mkromfs_for_honeygui 使用文档

## 概述

`mkromfs_for_honeygui.py` 是为 HoneyGUI 定制的 ROM 文件系统生成和解压工具。它可以将目录树打包成二进制文件系统，也可以从二进制文件系统中提取文件。

## 功能特性

### 1. 文件系统生成
- 支持递归目录结构
- 自动生成 C 代码或二进制格式
- 生成资源访问头文件（`ui_resource.h`）
- 按字母顺序排序文件和目录

### 2. 文件系统解压
- 从二进制文件系统提取所有文件
- 保持原始目录结构
- 自动处理地址偏移

### 3. 调试功能
- 支持文件系统层级结构输出（`--dump`）

## 使用方法

### 生成二进制文件系统

```bash
# 基本用法
python mkromfs_for_honeygui.py --binary --addr 0x704D1400 root root_0x704D1400.bin

# 参数说明：
# --binary          : 生成二进制格式（不加则生成 C 代码）
# --addr 0x704D1400 : 指定基地址（十六进制）
# root              : 源目录路径
# root_0x704D1400.bin : 输出文件名
```

### 生成 C 代码格式

```bash
python mkromfs_for_honeygui.py root romfs.c
```

### 解压文件系统

```bash
# 基本用法（自动检测基地址）
python mkromfs_for_honeygui.py --extract root_0x704D1400.bin

# 指定输出目录
python mkromfs_for_honeygui.py --extract root_0x704D1400.bin --output-dir extracted_files

# 手动指定基地址（可选）
python mkromfs_for_honeygui.py --extract --addr 0x704D1400 root_0x704D1400.bin

# 参数说明：
# --extract         : 启用解压模式
# --addr 0x704D1400 : 基地址（可选，工具会自动检测）
# --output-dir      : 输出目录（默认为 extracted_romfs）
```

### 查看文件系统结构

```bash
python mkromfs_for_honeygui.py --dump root
```

## 文件系统特性

### 数据结构

每个目录项（dirent）包含 16 字节：
```c
struct romfs_dirent {
    uint32_t type;      // 0=文件, 1=目录
    uint32_t name;      // 文件名地址
    uint32_t data;      // 数据地址
    uint32_t size;      // 文件大小（字节）或子项数量
};
```

### 对齐规则

1. **文件名对齐**：4 字节边界
2. **文件数据对齐**：特殊对齐方式
   - 文件起始地址满足：`(地址 + 8) % 64 == 0`
   - 即：文件地址相对于 64 字节边界偏移 56 字节
   - 设计目的：优化硬件 DMA 访问或配合特定数据结构

### 布局示例

```
[Root Dirent (16B)] -> 指向根目录名称和数据
[Root Name ("/\0\0\0")] -> 4字节对齐
[Child Dirents...] -> 每个16字节
[File Names...] -> 各自4字节对齐
[Padding] -> 使文件数据满足对齐要求
[File Data 1]
[Padding]
[File Data 2]
...
```

## 生成的头文件（ui_resource.h）

### 文件位置
生成在源目录的上一级目录中：`../ui_resource.h`

### 文件内容

```c
#if defined _HONEYGUI_SIMULATOR_
extern unsigned char *resource_root;

#define   IMAGE_PNG                              (void *)(resource_root + 0x00000038)
#define   ICON_BIN                               (void *)(resource_root + 0x00001234)
#define   FONT_TTF                               (void *)(resource_root + 0x00005678)

#else

#define   IMAGE_PNG                              (void *)(0x704D1438)
#define   ICON_BIN                               (void *)(0x704D2634)
#define   FONT_TTF                               (void *)(0x704D6A78)

#endif
```

### 特性说明

1. **条件编译**
   - `_HONEYGUI_SIMULATOR_`：模拟器环境，使用相对地址
   - 其他环境：使用绝对地址

2. **宏命名规则**
   - 文件名转大写
   - `.` 替换为 `_`
   - 例如：`image.png` → `IMAGE_PNG`

3. **地址计算**
   - 模拟器：`resource_root + 偏移量`
   - 实际硬件：`基地址 + 偏移量`

## 使用示例

### 示例 1：打包资源文件

```bash
# 目录结构
root/
├── images/
│   ├── logo.png
│   └── icon.bin
└── fonts/
    └── arial.ttf

# 生成文件系统
python mkromfs_for_honeygui.py --binary --addr 0x4400000 root root_0x4400000.bin

# 生成的 ui_resource.h 包含：
# LOGO_PNG, ICON_BIN, ARIAL_TTF
```

### 示例 2：在代码中使用资源

```c
#include "ui_resource.h"

void load_image(void) {
    // 模拟器和硬件都可以使用相同的宏
    uint8_t *image_data = (uint8_t *)LOGO_PNG;
    uint8_t *icon_data = (uint8_t *)ICON_BIN;
    
    // 使用资源数据...
}
```

### 示例 3：解压并验证

```bash
# 解压文件系统（自动检测基地址）
python mkromfs_for_honeygui.py --extract root_0x4400000.bin --output-dir verify

# 对比原始文件
diff -r root verify
```

## 注意事项

1. **基地址自动检测**
   - 解压时通常**不需要**指定 `--addr` 参数
   - 工具会自动从二进制文件中检测基地址
   - 仅在自动检测失败时才需要手动指定

2. **文件名限制**
   - 避免使用特殊字符
   - 建议使用字母、数字、下划线和点

3. **路径要求**
   - 生成模式：`rootdir` 必须是目录
   - 解压模式：`rootdir` 必须是二进制文件

4. **头文件生成**
   - 仅在 `--binary` 模式下生成 `ui_resource.h`
   - 头文件位置固定为源目录的上一级

5. **Python 版本**
   - 需要 Python 3.x
   - 已在 Python 3.9+ 测试通过

## 错误处理

### 常见错误

1. **文件读取失败**
   ```
   Error reading file xxx: [Errno 2] No such file or directory
   ```
   解决：检查文件路径和权限

2. **地址格式错误**
   ```
   invalid literal for int() with base 16
   ```
   解决：确保 `--addr` 参数为有效的十六进制数（如 0x4400000）

3. **解压失败**
   ```
   Error extracting filesystem: ...
   ```
   解决：检查二进制文件完整性和基地址是否正确

## 技术细节

### 对齐算法

```python
# 文件名对齐到 4 字节
ALIGNMENT_4 = 4

# 文件数据对齐算法
ALIGNMENT_64 = 64
if (data_addr + 8) % ALIGNMENT_64 != 0:
    padding = ALIGNMENT_64 - (data_addr + 8) % ALIGNMENT_64
    # 添加填充字节
```

### 目录遍历

- 深度优先遍历（Depth-First）
- 按字母顺序排序
- 递归处理子目录

### 二进制格式

- 小端序（Little-Endian）
- 所有整数为 32 位无符号整数
- 字符串以 null 结尾

## 版本历史

- **v2.0** (2025-12-03)
  - 重构代码，移除全局变量
  - 添加文件系统解压功能
  - 改进错误处理
  - 更新宏定义为 `_HONEYGUI_SIMULATOR_`
  - Python 3 完全兼容

- **v1.0**
  - 初始版本
  - 支持文件系统生成

## 许可证

本工具为 HoneyGUI 项目的一部分，遵循项目许可证。
