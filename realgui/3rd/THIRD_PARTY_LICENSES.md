# 第三方库许可证说明

本文档记录了 HoneyGUI 项目中使用的第三方开源库及其许可证信息。

## 库列表

### 1. Arm-2D
- **描述**: ARM 官方提供的 2D 图形加速库
- **用途**: 2D 图像处理加速
- **许可证**: Apache-2.0
- **来源**: https://github.com/ARM-software/Arm-2D

### 2. box2d
- **描述**: 2D 物理引擎
- **作者**: Erin Catto
- **用途**: 2D 刚体物理模拟
- **许可证**: MIT License
- **来源**: https://github.com/erincatto/box2d

### 3. cgltf
- **描述**: glTF 2.0 文件加载器
- **用途**: 加载和解析 glTF 3D 模型文件
- **许可证**: MIT License
- **来源**: https://github.com/jkuhlmann/cgltf

### 4. ezXML
- **描述**: 轻量级 XML 解析库
- **用途**: XML 文件解析
- **许可证**: MIT License
- **来源**: http://ezxml.sourceforge.net/

### 5. gifdec
- **描述**: GIF 图像解码器
- **用途**: GIF 动画解码
- **许可证**: Public Domain (公有领域)
- **特性**: 小于 500 行 C99 代码

### 6. h264bsd
- **描述**: H.264 视频解码器
- **用途**: H.264/AVC 视频流解码
- **许可证**: Apache-2.0
- **版权**: Copyright (C) 2009 The Android Open Source Project

### 7. letter_shell
- **描述**: 嵌入式 Shell 命令行工具
- **作者**: Letter
- **用途**: 提供命令行交互接口
- **许可证**: MIT License
- **版权**: Copyright (c) 2019 Letter

### 8. Lite3D
- **描述**: Realtek 3D 渲染引擎
- **用途**: 3D 图形渲染
- **许可证**: Apache-2.0
- **版权**: Copyright (c) 2026, Realtek Semiconductor Corporation

### 9. litegfx
- **描述**: 轻量级图形库（基于 VGLite）
- **作者**: Shanghai QDay Technology Co., Ltd. / LiteGFX Team
- **用途**: 矢量图形渲染加速
- **许可证**: 专有许可证 (Proprietary)
- **版权**: 
  - 核心库: Copyright (c) 2021-2023 Shanghai QDay Technology Co., Ltd.
  - HoneyGUI 适配层: Copyright (c) 2026 Realtek Semiconductor Corporation
- **说明**: 核心库为闭源，提供预编译库文件

### 10. nanosvg
- **描述**: SVG 图像解析和光栅化库
- **用途**: SVG 矢量图形解析和渲染
- **许可证**: Zlib License
- **来源**: https://github.com/memononen/nanosvg

### 11. nanovg
- **描述**: 矢量图形渲染库
- **作者**: Mikko Mononen
- **用途**: 2D 矢量图形渲染
- **许可证**: Zlib License
- **版权**: Copyright (c) 2013 Mikko Mononen

### 12. particle
- **描述**: 粒子系统引擎
- **作者**: Realtek Semiconductor Corp.
- **用途**: 粒子特效渲染和动画
- **许可证**: Apache-2.0
- **版权**: Copyright (c) 2026 Realtek Semiconductor Corp.

### 13. stb
- **描述**: STB 单头文件库集合
- **作者**: Sean Barrett
- **用途**: 图像加载、字体渲染等
- **许可证**: MIT License / Public Domain (双许可证)
- **版权**: Copyright (c) 2017 Sean Barrett
- **来源**: https://github.com/nothings/stb

### 14. tinyobj
- **描述**: Wavefront OBJ 文件加载器
- **作者**: Syoyo Fujita
- **用途**: 加载 OBJ 3D 模型文件
- **许可证**: MIT License
- **版权**: Copyright (c) 2016-2019 Syoyo Fujita
- **来源**: https://github.com/syoyo/tinyobjloader

### 15. tjpgd
- **描述**: 微型 JPEG 解码器
- **作者**: ChaN
- **用途**: JPEG 图像解码
- **许可证**: 自由软件许可证 (无担保，可自由使用)
- **版权**: Copyright (C) 2021, ChaN
- **限制**: 源代码再分发时必须保留版权声明

## 许可证类型说明

### MIT License
- 允许商业使用
- 允许修改和再分发
- 需保留版权声明和许可证声明

### Zlib License
- 类似 MIT，但更宽松
- 允许商业使用和修改
- 修改后的版本必须标明

### Public Domain (公有领域)
- 无版权限制
- 可自由使用、修改和分发

### BSD License
- 允许商业使用
- 允许修改和再分发
- 需保留版权声明

### Apache-2.0
- 允许商业使用
- 提供专利授权
- 需保留版权和许可证声明

### Realtek 专有许可证
- 仅限 Realtek 芯片使用
- 不可逆向工程
- 有特定使用限制

## 合规说明

所有第三方库的使用均遵循其各自的许可证条款。在使用 HoneyGUI 时，请确保：

1. 保留所有第三方库的版权声明
2. 遵守各库的许可证要求
3. 如有修改，按要求标注
4. 商业使用时注意许可证限制

## 更新日期

最后更新: 2026-01-28
