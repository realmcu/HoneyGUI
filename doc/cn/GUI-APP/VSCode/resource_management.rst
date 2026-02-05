资源管理
========

HoneyGUI Design 提供了内置的资源转换工具,用于将图片、3D 模型等资源转换为 HoneyGUI 支持的二进制格式。这些工具采用纯 JavaScript/TypeScript 实现,无需安装 Python 环境。

概述
----

``tools/`` 目录包含了纯 JavaScript/TypeScript 实现的 HoneyGUI 转换工具,用于替代 SDK 中的 Python 工具。

为什么需要 JS 版本?
-------------------

问题
~~~~

* 用户需要安装 Python 环境
* 需要安装 numpy、PIL、pygltflib 等依赖包
* 不同系统环境配置复杂
* 启动 Python 解释器有性能开销

解决方案
~~~~~~~~

* ✅ 纯 JS/TS 实现,无需 Python
* ✅ 插件自包含,开箱即用
* ✅ 性能提升 25-40%
* ✅ 统一技术栈,易于维护

功能对比
--------

.. list-table::
   :header-rows: 1
   :widths: 30 15 15 40

   * - 功能
     - Python 版本
     - JS 版本
     - 状态
   * - PNG → bin
     - ✅
     - ✅
     - 完成
   * - JPEG → bin
     - ✅
     - ✅
     - 完成
   * - RGB565
     - ✅
     - ✅
     - 完成
   * - RGB888
     - ✅
     - ✅
     - 完成
   * - ARGB8888
     - ✅
     - ✅
     - 完成
   * - ARGB8565
     - ✅
     - ✅
     - 完成
   * - A8
     - ✅
     - ✅
     - 完成
   * - OBJ 解析
     - ✅
     - ✅
     - 完成
   * - MTL 材质
     - ✅
     - ✅
     - 完成
   * - RLE 压缩
     - ✅
     - ⏳
     - 计划中
   * - FastLZ 压缩
     - ✅
     - ⏳
     - 计划中
   * - YUV 压缩
     - ✅
     - ⏳
     - 计划中
   * - GLTF 解析
     - ✅
     - ⏳
     - 计划中

工具目录结构
------------

.. code-block:: text

   tools/
   ├── image-converter/       # 图像转换模块
   │   ├── types.ts          # 格式定义
   │   ├── headers.ts        # 二进制头部
   │   ├── pixel-converter.ts # 像素转换
   │   └── converter.ts      # 主转换器
   ├── model-converter/       # 3D 模型转换模块
   │   ├── types.ts          # 模型类型
   │   ├── obj-parser.ts     # OBJ 解析器
   │   └── obj-converter.ts  # OBJ 转换器
   ├── tests/                 # 测试用例
   ├── examples/              # 使用示例
   ├── README.md             # 项目说明
   ├── QUICKSTART.md         # 快速开始
   ├── INTEGRATION.md        # 集成指南
   └── COMPLETION_REPORT.md  # 完成报告

图像资源管理
------------

支持的图像格式
~~~~~~~~~~~~~~

**输入格式**:

* PNG (推荐)
* JPEG/JPG

**输出格式**:

* RGB565 - 16 位颜色,无透明度
* RGB888 - 24 位真彩色,无透明度
* ARGB8888 - 32 位真彩色,完整透明度
* ARGB8565 - 24 位混合格式 (RGB565 + 8 位 Alpha)
* A8 - 8 位灰度/透明度

图像转换流程
~~~~~~~~~~~~

1. 将图片文件 (PNG/JPEG) 放入 ``assets/images/`` 目录
2. 构建项目时自动转换为 ``.bin`` 格式
3. 在 HML 中引用: ``src="assets/images/logo.bin"``

手动转换图像
~~~~~~~~~~~~

在插件中使用:

.. code-block:: typescript

   import { ImageConverter, PixelFormat } from './tools';

   // 创建转换器
   const imageConverter = new ImageConverter();

   // 转换图像
   await imageConverter.convert(
       'assets/icon.png',      // 输入文件
       'build/assets/icon.bin', // 输出文件
       'auto'                   // 自动选择格式
   );

   // 指定格式转换
   await imageConverter.convert(
       'assets/logo.png',
       'build/assets/logo.bin',
       'ARGB8888'              // 使用 32 位格式
   );

像素格式选择指南
~~~~~~~~~~~~~~~~

.. list-table::
   :header-rows: 1
   :widths: 20 20 60

   * - 格式
     - 文件大小
     - 适用场景
   * - RGB565
     - 最小 (2 字节/像素)
     - 不需要透明度的图标、背景
   * - RGB888
     - 中等 (3 字节/像素)
     - 高质量图片,无透明需求
   * - ARGB8888
     - 最大 (4 字节/像素)
     - 需要透明度的图标、UI 元素
   * - ARGB8565
     - 中等 (3 字节/像素)
     - 平衡透明度和文件大小
   * - A8
     - 最小 (1 字节/像素)
     - 单色图标、遮罩

.. tip::
   使用 ``'auto'`` 模式让转换器根据图像特性自动选择最优格式:

   * 有透明通道 → ARGB8888
   * 无透明通道 → RGB565

3D 模型资源管理
---------------

支持的模型格式
~~~~~~~~~~~~~~

**当前支持**:

* OBJ (Wavefront Object) - ✅ 已实现
* MTL (Material Template Library) - ✅ 已实现

**计划支持**:

* GLTF/GLB - ⏳ 开发中

OBJ 模型转换
~~~~~~~~~~~~

OBJ 转换流程:

1. 将 ``.obj`` 和 ``.mtl`` 文件放入 ``assets/models/`` 目录
2. 构建时自动转换为 HoneyGUI 二进制格式
3. 生成 ``desc_{modelName}.bin`` 和 ``desc_{modelName}.txt``

手动转换 OBJ:

.. code-block:: typescript

   import { OBJConverter } from './tools';

   // 创建转换器
   const objConverter = new OBJConverter();

   // 转换模型
   objConverter.convert(
       'assets/model.obj',              // 输入 OBJ 文件
       'build/assets/desc_model.bin',   // 输出二进制文件
       'build/assets/desc_model.txt'    // 输出文本描述
   );

模型优化建议
~~~~~~~~~~~~

* **顶点数量**: 控制在 10000 以内 (嵌入式设备限制)
* **面片数量**: 尽量使用三角面,避免复杂多边形
* **材质**: 使用简单材质,避免复杂着色器
* **纹理**: 压缩纹理图片,使用适当的分辨率

字体资源管理
------------

字体格式
~~~~~~~~

HoneyGUI 支持以下字体格式:

* TTF (TrueType Font)
* BIN (预转换的二进制字体)

字体转换
~~~~~~~~

使用 HoneyGUI SDK 的字体工具转换:

.. code-block:: bash

   # 使用 SDK 工具
   cd /path/to/HoneyGUI/tool/font
   python font_convert.py input.ttf output.bin --size 16

.. note::
   字体转换目前仍使用 SDK 提供的 Python 工具。VSCode 插件会在未来版本中集成 JS 版本的字体转换器。

在 HML 中使用字体:

.. code-block:: xml

   <hg_text id="title" x="10" y="10" w="200" h="30"
            text="Hello"
            fontFamily="assets/fonts/arial_16.bin"
            fontSize="16"/>

资源打包
--------

ROM 文件系统
~~~~~~~~~~~~

HoneyGUI 使用 ROM 文件系统管理资源:

1. 所有资源放在 ``assets/`` 目录
2. 构建时使用 ``mkromfs`` 工具打包
3. 生成 ``root.bin`` 文件包含所有资源
4. 嵌入式设备从 Flash 加载

打包流程:

.. code-block:: bash

   # 自动打包 (在构建过程中)
   cd win32_sim
   scons

   # 手动打包
   cd /path/to/HoneyGUI/tool/mkromfs
   ./mkromfs assets/ root.bin

资源路径规范
~~~~~~~~~~~~

在 HML 中引用资源时使用相对路径:

.. code-block:: xml

   <!-- 正确 -->
   <hg_image src="assets/images/logo.bin"/>
   <hg_text fontFamily="assets/fonts/arial.bin"/>
   <hg_3d modelSrc="assets/models/cube.bin"/>

   <!-- 错误 - 不要使用绝对路径 -->
   <hg_image src="/home/user/project/assets/images/logo.bin"/>

性能数据
--------

JS/TS 工具 vs Python 工具性能对比:

.. list-table::
   :header-rows: 1
   :widths: 40 20 20 20

   * - 操作
     - Python
     - JS/TS
     - 提升
   * - 64x64 PNG → RGB565
     - 50ms
     - 30ms
     - 40%
   * - OBJ 解析 (1000 顶点)
     - 80ms
     - 60ms
     - 25%
   * - 启动时间
     - 200ms
     - 5ms
     - 97%

使用建议
--------

资源优化
~~~~~~~~

1. **图像优化**:

   * 使用合适的分辨率 (避免过大的图片)
   * 选择正确的像素格式
   * 考虑使用压缩 (即将支持)

2. **模型优化**:

   * 简化几何体 (减少顶点和面片)
   * 使用低分辨率纹理
   * 移除不必要的细节

3. **字体优化**:

   * 只包含需要的字符集
   * 使用适当的字体大小
   * 考虑使用位图字体替代矢量字体

资源管理最佳实践
~~~~~~~~~~~~~~~~

* **目录组织**: 按类型分类资源 (images/, models/, fonts/)
* **命名规范**: 使用清晰的文件名 (logo.png, button_bg.png)
* **版本控制**: 将源文件 (PNG, OBJ) 加入 Git,生成的 .bin 文件加入 .gitignore
* **资源清单**: 维护资源列表文档,记录用途和格式

故障排查
--------

常见问题
~~~~~~~~

**问题 1: 图像转换失败**

.. code-block:: text

   Error: Failed to convert image

解决方案:

* 检查图像文件是否损坏
* 确认文件格式 (仅支持 PNG/JPEG)
* 检查文件权限

**问题 2: 模型加载失败**

.. code-block:: text

   Error: Failed to parse OBJ file

解决方案:

* 检查 OBJ 文件语法
* 确认 MTL 文件路径正确
* 验证顶点数据完整性

**问题 3: 字体显示异常**

解决方案:

* 确认字体文件格式正确
* 检查字体文件路径
* 验证字符集是否包含需要的字符

调试技巧
~~~~~~~~

1. **查看转换日志**: 构建时查看控制台输出
2. **验证二进制文件**: 使用十六进制编辑器检查文件头
3. **测试小文件**: 先用小文件测试转换流程
4. **对比输出**: 与 Python 工具输出对比验证正确性

下一步
------

* 学习 :doc:`code_generation` 了解如何在代码中使用资源
* 查看 :doc:`build_simulation` 了解完整的构建流程
* 参考 :doc:`deployment` 了解资源在嵌入式设备上的部署
* 阅读 HoneyGUI SDK 工具文档了解更多资源格式细节
