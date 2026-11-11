.. _浏览器仿真:

==========
浏览器仿真
==========

HoneyGUI 引擎可编译为 WebAssembly，在浏览器中运行，无需连接硬件设备。

优势
----

**无需编译 C 代码**

引擎自带 HML 运行时，直接解析设计器导出的 ``ui/*.xml`` 并渲染。改界面不产生编译，保存即可看到结果。

**使用者零安装**

只需一个浏览器。引擎可预编译后随工具或网站分发，使用者无需安装 MinGW、Python、SCons、SDL2。

**跨平台一致**

Windows、Linux、macOS 加载同一个预编译 wasm，不因平台差异产生行为不一致。

**分发成本低**

``gui.js`` 与资源文件部署到静态服务器即可使用，无需使用者下载工具链。

**可以嵌进任意网页**

提供 ``startHoneyGUI()`` 接口，能在自己的网页、文档或 VS Code 插件里嵌一个 GUI 预览窗口。

局限
----

- 没有调试器：不能打断点、单步执行
- 工程 C 代码（回调、业务函数）在预览模式下不执行
- 单线程协作调度，比原生慢
- 没有 LetterShell：不能在控制台敲命令改运行时数据

  桌面仿真器提供命令行终端，工程可注册自定义命令，在运行中修改参数并立即查看效果，无需重新编译。浏览器目标不具备该机制，因此浏览器仿真中没有终端。

所以浏览器仿真适合看界面、做演示、把预览发给别人；要调试 C 代码时用桌面仿真器。

快速上手
--------

需要先装好 emsdk 并激活，否则 ``emcmake`` 不存在。安装步骤见 :ref:`安装 emsdk`。

仓库不提供预编译的引擎，``gui.js`` 需本地构建；仅 `HoneyDesigner <https://realmcu.github.io/HoneyDesigner/>`_ 在线站点自带预编译版本，无需本地环境。

在已激活的终端中，于仓库根目录执行：

.. code-block:: shell

   > emcmake cmake -S simulator/web -B simulator/web/build -G Ninja
   > cmake --build simulator/web/build
   > python simulator/web/pack_files.py

然后打开 ``simulator/web/index.html``。

上述流程使用仓库自带的默认配置：包含引擎与 HML 运行时，**不含示例程序**，因此打开页面后画面是空的。这就是设计器预览所使用的引擎；想直接看到编译进引擎的示例程序，见 `选择要运行的示例程序`_。预览设计器导出的工程见 `设计器界面预览`_；嵌入自有网页见 `嵌入到自己的网页`_。

.. _安装 emsdk:

构建与运行
----------

环境准备
~~~~~~~~

所需组件：Python、CMake 3.13+、Ninja、emsdk。

Python 随 :ref:`入门指南` 安装；CMake 与 Ninja 可通过包管理器安装，或使用 emsdk 自带的版本。仅 emsdk 需要单独下载：

1. 下载 emsdk：`https://github.com/emscripten-core/emsdk <https://github.com/emscripten-core/emsdk>`_
2. 安装并激活：

.. code-block:: shell

   > cd emsdk
   > emsdk.bat install latest      :: Linux/macOS: ./emsdk install latest
   > emsdk.bat activate latest     :: Linux/macOS: ./emsdk activate latest
   > emsdk_env.bat                 :: Linux/macOS: source ./emsdk_env.sh

3. 确认安装：

.. code-block:: shell

   > emcc --version

每打开一个新终端，均需重新执行 ``emsdk_env``。

.. note::

   Windows 使用 ``emsdk.bat`` / ``emsdk_env.bat``；Linux 与 macOS 使用 ``./emsdk`` / ``source emsdk_env.sh``。下文命令统一采用 Windows 写法，Linux/macOS 将 ``>`` 替换为 ``$`` 即可。

构建
~~~~

在仓库根目录：

.. code-block:: shell

   > emcmake cmake -S simulator/web -B simulator/web/build -G Ninja
   > cmake --build simulator/web/build

产物位于 ``simulator/web/build/``，其中 ``gui.js`` 即引擎。

.. note::

   必须用 ``emcmake cmake``，直接 ``cmake`` 会报 “must be configured with the Emscripten toolchain”。

选择要运行的示例程序
~~~~~~~~~~~~~~~~~~~~

和桌面仿真器共用 ``Kconfig.gui``：

.. code-block:: shell

   > cd simulator/web
   > menuconfig ../../Kconfig.gui

生成的 ``.config`` 决定引擎包含的控件与内置示例程序。仓库自带的默认 ``.config`` 包含全部控件与 HML 运行时、不含示例程序：打开后画面为空，设计器预览即使用此引擎。

示例程序只差一个 Kconfig 选项。任意一份 CI 配置都会选中一个，例如 466x466 示例：

.. code-block:: shell

   > emcmake cmake -S simulator/web -B simulator/web/build -G Ninja \
       -DWEB_SIM_CONFIG=example/ci_build_scripts/configs/application/.config_realtek_build_gui_466_466_demo
   > cmake --build simulator/web/build

示例程序从 romfs 镜像读取资源，因此执行 ``pack_files.py`` 之前须把该镜像列入 ``simulator/web/files.json``。该文件默认为空。

用 ``menuconfig`` 改过配置后，重新构建的命令不变（默认读取 ``.config``）：

.. code-block:: shell

   > emcmake cmake -S simulator/web -B simulator/web/build -G Ninja
   > cmake --build simulator/web/build

在浏览器里打开
~~~~~~~~~~~~~~

``index.html`` 通过 ``<script>`` 加载引擎，因此可直接从磁盘打开，无需 HTTP 服务器。资源打包为一个 ``files.js``：

.. code-block:: shell

   > python simulator/web/pack_files.py

然后打开 ``simulator/web/index.html``。

默认配置不含示例程序，因此画布是空的、页面只显示分辨率，这是预期结果而非故障。选中示例程序或加载设计器工程（见 `设计器界面预览`_）后页面才有内容。

窗口内的操作对应关系：鼠标拖动为滑动手势，点击为触摸，滚轮为列表滚动，Home / Back / Menu / Power 按钮为硬件按键。

.. note::

   ``files.js`` 由 ``files.json`` 中列出的文件打包生成，该文件默认为空。选中的示例程序需要把它的 romfs 镜像列进去；更换示例程序后需修改 ``files.json`` 并重新执行 ``pack_files.py``。``files.js`` 不入库。

打包成可分发的引擎
~~~~~~~~~~~~~~~~~~

若需将预览能力随工具分发给他人（使用者无需安装 emsdk），可预编译引擎：

.. code-block:: shell

   > emcmake cmake -S simulator/web -B simulator/web/build -G Ninja \
       -DWEB_SIM_MAIN_MODULE=ON
   > cmake --build simulator/web/build
   > cmake --install simulator/web/build --prefix <安装目录>

生成的目录结构：

.. code-block:: text

   engine/
       gui.js        引擎（全局 createHoneyGUI）
       gui.wasm
       web_sim.js    页面脚本（全局 startHoneyGUI）
   include/
       web_sim.h
       其他引擎头文件

将 ``engine/`` 中的三个文件放置到网页可访问的位置。

.. note::

   ``WEB_SIM_MAIN_MODULE=ON`` 会导出全部引擎符号，产物体积显著增大。若仅做界面预览、不加载已编译的工程，可不启用该选项。

屏幕尺寸
~~~~~~~~

预编译的引擎由多个工程共用，屏幕尺寸并非编译期固定，而是在启动时由页面指定（见 `渲染与缩放`_）。仓库自带的 ``index.html`` 使用配置中的默认尺寸。

常见问题
~~~~~~~~

``emcmake`` 找不到
  未激活 emsdk，或新开终端后未重新激活。

``must be configured with the Emscripten toolchain``
   使用了 ``cmake`` 而非 ``emcmake cmake``。

页面提示 ``build/gui.js is missing``
   引擎未构建，或构建输出位于其他目录。确认 ``simulator/web/build/gui.js`` 存在。

页面提示 ``files.js is missing``
   未执行 ``python simulator/web/pack_files.py``。

页面提示 ``Failed to start`` 或界面全黑
   页面编码不正确。``gui.js`` 中内嵌的是 UTF-8 文本形式的 wasm，页面须声明 ``<meta charset="UTF-8">``。

提示 ``This engine was built without the HML runtime``
   引擎配置未启用 HML 运行时。使用默认的 ``simulator/web/.config`` 构建即可，它已包含。

引擎启动但界面空白
   默认配置不含示例程序，这是预期结果。若要看到编译进引擎的示例，按 `选择要运行的示例程序`_ 选中一个。若已选中示例，则检查 ``files.json`` 中的路径是否存在，并重新执行 ``pack_files.py``。

设计器界面预览
--------------

适用场景：已使用 :doc:`HoneyGUI Visual Designer <../VSCode/index>` 完成界面设计，需要在浏览器中查看效果，且不希望为此编译工程的 C 代码。

浏览器仿真的引擎内置 HML 运行时，可直接解析设计器导出的 ``ui/*.xml`` 并渲染。修改界面无需重新构建，也无需下载编译器。

操作流程
~~~~~~~~

**第一步：准备引擎**

默认的 ``simulator/web/.config`` 就是该引擎：全部控件、HML 运行时、不含示例程序。

.. code-block:: shell

   > emcmake cmake -S simulator/web -B simulator/web/build -G Ninja \
       -DWEB_SIM_MAIN_MODULE=ON
   > cmake --build simulator/web/build
   > cmake --install simulator/web/build --prefix out

装出来是：

.. code-block:: text

   out/engine/    gui.js、gui.wasm、web_sim.js
   out/include/   头文件

**第二步：放工程资源**

设计器转换后的资源目录（``<project>/build/assets``）结构：

.. code-block:: text

   assets/
       ui/
           Main.xml            界面文件
           Settings.xml
       resource/
           map/map_00.bin      图片，保持设计器的子目录
       HarmonyOS_Sans_Bold..._bitmap.bin    字库
       project.json

把这个目录放到网页能访问的位置，记作 ``sim/assets``。

**第三步：页面里启动预览**

用 ``startHoneyGUI()``，把资源写进引擎的文件系统，再告诉它 HML 在哪：

.. code-block:: javascript

   const files = { /* 'assets/ui/Main.xml': Uint8Array, ... */ };

   const sim = await startHoneyGUI({
       createModule: globalThis.createHoneyGUI,
       canvas,
       files,
       screen: { width: 410, height: 502, pixelBits: 32 },
       hml: { dir: 'assets' },        // HML 在 assets/ui 下
   });

``hml.dir`` 为模块工作目录下的相对路径。引擎扫描其下的 ``ui/``，注册所有视图并显示入口视图。

如需指定打开的视图，使用 ``hml: { dir: 'assets', startView: 'Settings' }``；名称不存在时仍显示入口视图。

**第四步（可选）：修改后立即刷新**

保存 HML 后无需重新构建，也无需重新执行 ``pack_files.py``：

.. code-block:: javascript

   const bytes = new TextEncoder().encode('<hml>...</hml>');
   sim.module.FS.writeFile('/assets/ui/Main.xml', bytes);   // 路径要和 hml.dir 对应
   sim.reloadHml();

引擎在下一帧重建当前视图。解析失败的文件保留原有版本。

.. note::

   该刷新由页面调用 ``reloadHml()`` **主动触发**，引擎不会主动轮询文件变化。引擎中的 ``hml_watch()`` 接口尚未接入工程。

.. warning::

   每次刷新都会在引擎堆中多保留一份被替换的文档（隐藏视图可能仍在引用旧节点）。引擎堆为 10 MB，反复刷新同一个较大文件数十次后可能耗尽。页面自行控制刷新时，累计约 3 MB 后应重启引擎。

只预览界面，不执行 C 代码
~~~~~~~~~~~~~~~~~~~~~~~~~~

HML 中按名称引用的业务函数（``functionName``、定时器 ``callback``）属于工程 C 代码，预览时不会执行。需要这部分逻辑时，请使用 VS Code 扩展仿真，或将工程编译为 ``project.wasm`` 加载（见 `加载编译好的工程`_）。

在桌面仿真器上预览同一份资源
~~~~~~~~~~~~~~~~~~~~~~~~~~~~

``menuconfig`` 中选择 ``HML Designer Application``，构建后将资源目录作为参数传入：

.. code-block:: shell

   > .\simulator\win32\gui.exe ..\my_project\build\assets

常见问题
~~~~~~~~

提示 ``This engine was built without the HML runtime``
   引擎未启用 ``CONFIG_REALTEK_BUILD_XML_LOADER``。使用默认的 ``simulator/web/.config`` 构建即可，它已包含。

界面元素错位或图片缺失
   HML 中的资源路径按 ``assets/resource/xxx.png`` → ``/resource/xxx.bin`` 的规则解析。确认图片已由设计器转换为 ``.bin``，且子目录一致。

修改界面后无变化
   确认已调用 ``sim.reloadHml()``，且写入路径与 ``hml.dir`` 一致（``assets/ui/xxx.xml`` 对应 MEMFS 中的 ``/assets/ui/xxx.xml``）。

界面上缺少控件
   HML 运行时与设计器代码生成器支持的组件并不完全一致。预览中缺失的部分，以 VS Code 扩展中的仿真结果为准。

嵌入到自己的网页
----------------

适用场景：已完成引擎打包（见 `打包成可分发的引擎`_），需要在自有网页、文档或插件中嵌入 GUI 预览窗口。

最小可运行页面
~~~~~~~~~~~~~~

.. code-block:: html

   <!DOCTYPE html>
   <html>
   <head><meta charset="UTF-8"><title>HoneyGUI</title></head>
   <body>
   <canvas id="screen"></canvas>
   <script src="engine/gui.js"></script>
   <script src="engine/web_sim.js"></script>
   <script>
       (async () => {
           const canvas = document.getElementById('screen');
           const sim = await startHoneyGUI({
               createModule: globalThis.createHoneyGUI,
               canvas,
               files: {},                                  // 运行时资源
               screen: { width: 410, height: 502, pixelBits: 32 },
           });
           canvas.style.width  = `${sim.width * 2}px`;     // 按需缩放
           canvas.style.height = `${sim.height * 2}px`;
       })();
   </script>
   </body>
   </html>

三点注意事项：

1. **页面须声明 UTF-8**：``<meta charset="UTF-8">``。``gui.js`` 中内嵌的是 UTF-8 文本形式的 wasm，字符集错误将导致启动失败
2. **加载顺序**：先加载 ``gui.js``\ （定义 ``createHoneyGUI``），再加载 ``web_sim.js``\ （定义 ``startHoneyGUI``）
3. **两个脚本均为普通脚本**，非 ES module，因此从 ``file://`` 打开亦可运行

三种常用配置
~~~~~~~~~~~~

**1. 仅运行引擎内置的示例程序**

``files: {}``，``screen`` 填写配置对应的分辨率。使用仓库自带的 ``index.html`` 时，资源已打包进 ``files.js``。

**2. 预览设计器工程**

.. code-block:: javascript

   await startHoneyGUI({ createModule, canvas, files, screen,
                         hml: { dir: 'assets' } });

见 `设计器界面预览`_。

**3. 加载编译好的工程**

.. code-block:: javascript

   await startHoneyGUI({ createModule, canvas, files, screen,
                         project: projectWasmBytes });

工程须先编译为 Emscripten 侧模块（``-sSIDE_MODULE``，并带 ``-sASYNCIFY``），引擎亦须以 ``WEB_SIM_MAIN_MODULE=ON`` 构建。

.. _加载编译好的工程:

``files`` 参数
~~~~~~~~~~~~~~

``{ 相对路径: Uint8Array }``，在引擎启动前写入内存文件系统，目录自动创建。用于向引擎提供工程运行所需的资源（romfs 镜像、图片、字库）。在仓库自带的页面中，该步骤由 ``pack_files.py`` 生成的 ``files.js`` 完成。

.. _渲染与缩放:

渲染与缩放
~~~~~~~~~~

``startHoneyGUI()`` 内部已实现每帧刷新循环，页面只需：

- 将 canvas 的 ``width`` / ``height`` 属性设置为 ``sim.width`` / ``sim.height``\ （引擎像素）
- 通过 CSS 的 ``style.width`` / ``style.height`` 决定显示尺寸

.. code-block:: javascript

   canvas.width  = sim.width;
   canvas.height = sim.height;
   canvas.style.width  = `${sim.width * 2}px`;
   canvas.style.height = `${sim.height * 2}px`;
   canvas.style.imageRendering = 'pixelated';   // 放大时保持像素清晰

指针、滚轮与按键的事件监听已由 ``startHoneyGUI()`` 绑定并完成坐标换算，缩放无需额外处理。

无响应或位置偏移
~~~~~~~~~~~~~~~~

.. list-table::
   :header-rows: 1
   :widths: 40 60

   * - 现象
     - 原因
   * - 触摸无响应
     - 页面使用 ``click`` 而非 ``pointerdown`` / ``pointerup``；或 canvas 被其他元素遮挡，指针事件被截获
   * - 触摸位置偏移
     - canvas 显示尺寸与 ``width`` / ``height`` 属性不一致，且页面自行换算过一次坐标。应交由内置监听处理，避免重复换算
   * - 画面被裁切或拉伸
     - 仅修改 CSS 尺寸而未修改 ``width`` / ``height`` 属性，或传入的 ``screen`` 与 canvas 尺寸不一致
   * - 调用 ``stop()`` 后再次启动报错
     - Emscripten 模块实例不可复用，需重新创建（常见做法为刷新 iframe）

完整接口列表见 ``simulator/web/port/web_sim.h`` 与 ``simulator/web/web_sim.js`` 中的注释。

部署与分发
----------

HoneyGUI 仓库只提供引擎和页面宿主代码，不托管在线站点。想让人在浏览器里看到界面，有三条路：

.. list-table::
   :header-rows: 1
   :widths: 26 30 44

   * - 方式
     - 怎么做
     - 适合
   * - 本地打开
     - 构建后双击 ``simulator/web/index.html``
     - 自己确认效果，不涉及部署
   * - 用在线设计器
     - 打开 `HoneyDesigner <https://realmcu.github.io/HoneyDesigner/>`_
     - 直接用现成的网页版，不碰 HoneyGUI 仓库
   * - 自己静态托管
     - 见下面的完整步骤
     - 把预览嵌进自己的工具、文档或内网站点；无法访问 github.io 时走这条

自己静态托管的完整步骤
~~~~~~~~~~~~~~~~~~~~~~

**第一步：编译引擎**

.. code-block:: shell

   > emcmake cmake -S simulator/web -B simulator/web/build -G Ninja
   > cmake --build simulator/web/build

产物是单个 ``simulator/web/build/gui.js``\ （wasm 已内嵌），配 ``simulator/web/web_sim.js`` 使用。

.. note::

   默认的 ``.config`` 已编入 HML 运行时，这一步需要它。若用 ``-DWEB_SIM_CONFIG`` 指向未编入 HML 运行时的配置，页面会报 ``This engine was built without the HML runtime``。

仅在引擎需要更新时重做本步骤。修改界面不需要重新编译引擎，但需重新执行后续步骤。

**第二步：准备工程资源**

用 VS Code 扩展打开工程并执行一次「仿真」，资源将转换到 ``<project>/build/assets``。将该目录下每个文件的路径写入 ``simulator/web/files.json``：

.. code-block:: json

   [
     "my_project/build/assets/ui/Main.xml",
     "my_project/build/assets/resource/map/map_00.bin",
     "my_project/build/assets/HarmonyOS_Sans_Bold_size30_bits4_bitmap.bin"
   ]

两条约束：

- 路径须相对仓库根目录，不能包含 ``..``。打包脚本将路径原样用作引擎内的文件名，包含 ``..`` 会导致文件无法写入内存文件系统
- HML 中 ``src="assets/xxx.png"`` 引用的图片按 ``xxx.bin`` 解析，列表须填写转换后的 ``.bin``

.. note::

   应打包构建产物 ``build/assets``，而非设计器中编辑的工程目录。

**第三步：打包**

.. code-block:: shell

   > python simulator/web/pack_files.py

输出 ``packed N file(s), ...``，生成 ``simulator/web/files.js``。

修改 HML 后须重复第二步至第四步：重新仿真 → 更新 ``files.json``\ （新增或删除文件时）→ 重新执行 ``pack_files.py`` → 将新的 ``files.js`` 复制到站点目录。

**第四步：拼出站点**

将以下四个文件放入同一目录：

.. code-block:: text

   mysite/
       index.html     从 simulator/web/index.html 复制
       gui.js         从 simulator/web/build/ 复制
       web_sim.js     从 simulator/web/ 复制
       files.js       上一步生成

``index.html`` 里要改三处：

.. code-block:: html

   <!-- 1. 脚本路径：原来是 build/gui.js -->
   <script src="gui.js"></script>

   <!-- 2. 指定屏幕，三个值来自工程 project.json 的 resolution / pixelMode / cornerRadius
           （ARGB8888 = 32 位，cornerRadius 为负表示圆屏） -->
   screen: { width: 410, height: 502, pixelBits: 32 },

   <!-- 3. 加上 hml 选项，指向 files.json 里那批资源所在的目录 -->
   startHoneyGUI({ createModule: globalThis.createHoneyGUI, canvas, files,
                   screen, hml: { dir: 'my_project/build/assets' } });

``files`` 由 ``files.js`` 提供\ （页面从 ``HONEYGUI_WEB_SIM_FILES`` 解码，无需自行编写加载代码）。``hml.dir`` 须与 ``files.json`` 中的路径前缀一致，引擎会扫描其下的 ``ui/``。

**第五步：启动服务**

浏览器不允许页面 ``fetch()`` 本地文件，因此不能直接双击打开，需启动 HTTP 服务：

.. code-block:: shell

   > cd mysite
   > python -m http.server 8000

然后访问 ``http://localhost:8000/``。

正式部署时将该目录交给任意静态服务器（nginx、IIS、GitHub Pages、内网文件服务器均可）。无后端，无额外运行时依赖。

.. note::

   **在线设计器运行的是 HML 预览**：解析 ``ui/*.xml`` 渲染界面，不执行工程 C 代码（``callFunction``、各类回调）。需要这些逻辑时使用 VS Code 扩展或桌面仿真器。
