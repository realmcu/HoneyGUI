资源管理
========

HoneyDesigner 在左侧 **Assets** 标签页中管理项目资源，并在编译或仿真前将其转换为目标引擎格式。转换由内置的 JavaScript/TypeScript 工具完成，资源转换本身不依赖 Python。详细用法见 :ref:`VSCode 插件资源转换工具 <VSCode插件资源转换>`。

资源类型
---------

.. list-table::
   :header-rows: 1
   :widths: 22 28 50

   * - 类型
     - 扩展名
     - 说明
   * - 图片
     - ``png``、``jpg``、``jpeg``、``gif``、``bmp``、``webp``
     - 可转换为 RGB565、RGB888、ARGB 等格式
   * - SVG
     - ``svg``
     - 矢量图资源
   * - 视频
     - ``mp4``、``avi``、``mov``、``mkv``、``webm``
     - 转换为 MJPEG、AVI、H264 等格式
   * - 3D 模型
     - ``gltf``、``glb``、``obj``
     - HoneyGUI 3D 控件使用
   * - 字体
     - ``ttf``、``otf``、``woff``、``woff2``
     - 转换为点阵字体或按目标引擎处理
   * - Glass
     - ``glass``
     - 玻璃/模糊效果参数文件
   * - Lottie
     - ``json``、``lottie``
     - Lottie 动画
   * - TRMap
     - ``trmap``
     - 纹理映射资源

依赖说明
---------

* 图片、字体、SVG、3D 模型、Glass 等资源由扩展内置的 TypeScript/JavaScript 工具转换。
* Lottie 和 TRMap 资源在构建时由插件处理，部分情况下直接拷贝。
* 视频转换需要 FFmpeg。未安装 FFmpeg 时会跳过视频资源。
* romfs 打包仍使用 Python 脚本。

.. _romfs-packaging-cn:

romfs 打包
-----------

HoneyGUI 项目在构建时会自动完成 romfs 打包，不需要手动运行打包脚本。流程如下：

1. 收集 HML 引用的资源，或按 ``alwaysConvert`` 配置收集资源。
2. 将资源转换为目标格式的 ``.bin`` 文件，输出到 ``build/assets/``。
3. 使用 ``project.json`` 中的 ``romfsBaseAddr`` 作为基地址，打包为 ``build/app_romfs_<baseAddr>.bin``。
4. 生成资源地址宏头文件（如 ``ui_resource.h``），供固件按地址直接访问资源。

资源转换工具面板单独执行转换时，输出目录以面板中选择的目录为准；项目构建则统一写入 ``build/app_romfs_<baseAddr>.bin``。

图片格式
---------

支持的输出格式：

.. list-table::
   :header-rows: 1
   :widths: 22 18 60

   * - 格式
     - 每像素
     - 适用场景
   * - ``RGB565``
     - 2 字节
     - 无透明度的图标、背景，体积最小
   * - ``RGB888``
     - 3 字节
     - 高质量无透明图片
   * - ``ARGB8565``
     - 3 字节
     - 需要透明度且希望控制体积
   * - ``ARGB8888``
     - 4 字节
     - 完整透明度，质量最高
   * - ``A8``
     - 1 字节
     - 8 位透明度/灰度遮罩
   * - ``A4`` / ``A2`` / ``A1``
     - 0.5 / 0.25 / 0.125 字节
     - 低位数透明度遮罩
   * - ``I8``
     - 1 字节
     - 256 色索引图

压缩方式：

* ``none``：不压缩。
* ``rle``：游程压缩。
* ``fastlz``：FastLZ 无损压缩。
* ``yuv``：YUV 有损压缩，可配置采样、模糊位数和 FastLZ 二次压缩。
* ``jpeg``：JPEG 有损压缩，可配置采样、质量和背景色。
* ``adaptive``：自动比较 FastLZ、RLE 和不压缩，选择最小结果。

转换设置可以在单个文件或文件夹上配置，并支持从父文件夹继承。

视频格式
---------

支持的目标格式：

* ``MJPEG``
* ``AVI``
* ``H264``
* ``MSV1``
* ``Cinepak``

可配置：

* 质量
* 帧率
* 缩放（按像素或按百分比）
* 裁剪（宽、高、位置）
* 裁剪和缩放的先后顺序
* GIF 是否按视频处理

视频转换依赖 FFmpeg。转换结果会作为资源打包到 build 目录。

字体
-----

* 将 ``ttf``、``otf``、``woff``、``woff2`` 放入 ``assets/``。
* 在 ``hg_label`` 中使用 ``fontFile`` 引用，路径以 ``/`` 开头，例如 ``/NotoSansSC-Medium.ttf``。
* 可以按字符集转换，只保留项目实际使用的字符，减少固件体积。
* 字体必须位于 ``assets/`` 目录中，否则设备上无法加载。

3D 模型
--------

* 支持 OBJ、GLTF、GLB。
* 模型纹理图片会一起参与资源转换。
* ``hg_3d`` 使用 ``modelPath`` 引用模型，例如 ``assets/models/cube.bin``。
* 建议控制顶点数和面片数，避免超出嵌入式设备性能。

Glass、Lottie 和 TRMap
-----------------------

* ``hg_glass`` 使用 ``.glass`` 参数文件生成玻璃/模糊效果。
* ``hg_lottie`` 使用 JSON 或 ``.lottie`` 动画文件。
* ``trmap`` 用于纹理映射资源。

使用流程
---------

1. 把资源放入项目的 ``assets/`` 目录。
2. 打开左侧 **Assets** 标签页。
3. 选择图片、视频、模型、字体等资源，配置目标格式和压缩方式。
4. 把图片等资源直接拖到画布，或手动在组件属性中填写路径。
5. 执行代码生成或编译仿真，插件自动转换资源。

资源路径
---------

.. code-block:: xml

   <!-- 正确：图片以 assets/ 开头 -->
   <hg_image id="img_logo" x="20" y="20" width="120" height="120"
             src="assets/images/logo.png" />

   <!-- 正确：字体以 / 开头，相对 assets/ -->
   <hg_label id="lbl_title" x="20" y="160" width="240" height="40"
             text="Hello" fontFile="/NotoSansSC-Medium.ttf" fontSize="24" />

   <!-- 正确：3D 模型 -->
   <hg_3d id="model_cube" x="40" y="40" width="320" height="320"
          modelPath="assets/models/cube.bin" />

不要使用本机绝对路径，例如 ``/home/user/project/assets/logo.png``。

转换配置
---------

资源转换配置保存在项目中，用于记录每个文件或文件夹的目标格式。主要能力：

* 按文件夹继承转换设置。
* 智能打包：只转换 HML 实际引用的资源和 ``alwaysConvert`` 中列出的资源。
* 全量打包：转换 ``assets/`` 下的全部资源。
* 可以单独转换某个资源，也可以随构建流程自动转换。

LVGL 资源配置
--------------

LVGL 项目可以选择：

* ``c-array``：资源编译为 C 数组，链接进固件。
* ``external-bin``：资源打包为 romfs，运行时从外部存储加载。

HoneyGUI 项目会生成 romfs 资源，并通过构建流程打包。

最佳实践
---------

* 图片使用合适的分辨率和最低可接受格式。
* 有透明需求时优先使用 ``ARGB8565`` 或 ``ARGB8888``。
* 不需要透明度时优先使用 ``RGB565``。
* 字体只保留项目需要的字符集。
* 视频使用合适的分辨率、帧率和质量，避免资源过大。
* 源文件纳入版本控制，生成文件放入 ``build/`` 并由构建流程管理。
* 使用清晰的文件名，例如 ``logo.png``、``btn_normal.png``。

故障排查
---------

**图片转换失败**

* 检查文件是否损坏。
* 确认扩展名和实际格式一致。
* 检查输出目录权限。

**视频没有转换**

* 安装 FFmpeg 并加入 ``PATH``。
* 检查视频编码是否受支持。

**字体显示异常**

* 确认字体位于 ``assets/``。
* 确认 ``fontFile`` 以 ``/`` 开头。
* 确认字符集包含实际显示的文字。

**模型加载失败**

* 检查模型和纹理文件是否完整。
* 确认 ``modelPath`` 指向转换后的资源。

下一步
-------

* 查看 :doc:`build_simulation` 了解完整的构建流程
* 参考 :doc:`deployment` 了解资源在嵌入式设备上的部署
