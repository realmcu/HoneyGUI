使用 VSCode 插件开发应用程序
===============================

HoneyDesigner 是为嵌入式 :term:`GUI` 应用提供可视化设计、HML 编辑、C 代码生成和本地仿真的一体化 VSCode 扩展。

主要特性
---------

* **双引擎代码生成**：支持 HoneyGUI 与 LVGL，由 ``project.json`` 的 ``targetEngine`` 决定。
* **可视化设计**：拖放式画布、组件树、属性/事件面板、组件对齐与分布、视图导航关系图。
* **HML 文件格式**：基于 XML，适合版本控制，可由设计器或文本编辑器维护。
* **代码保护**：UI 代码每次覆盖；回调代码保留保护区；``src/user`` 只生成一次。
* **资源转换**：图片、字体、视频、3D 模型、SVG、Lottie、Glass、TRMap 等资源可在资源面板配置并转换。
* **编译与仿真**：内置构建流程，支持 HoneyGUI（SCons）和 LVGL（CMake）目标。
* **模板与 AI 协作**：支持空项目和在线模板；可选分发 ``.claude/skills`` 与 ``AGENTS.md``。
* **UART 下载**：编译后可把 romfs 镜像下载到支持的开发板。

.. note::

   * Windows 和 Linux/WSL 是主要支持平台；macOS 尚未完整验证。
   * 设计、代码生成、编译和仿真可在本地完成；从模板创建项目需要 ``git`` 和网络。
   * 首次编译仿真需要 Python 3.9.7、SCons 4.4.0 和 GCC/MinGW；Linux 还需要 SDL2；视频转换需要 FFmpeg。

开发流程
---------

使用 VSCode 插件开发 HoneyGUI 应用的典型流程：

1. 创建或打开项目。
2. 在 HML 设计器中完成界面和事件配置。
3. 生成 C 代码（HoneyGUI 或 LVGL）。
4. 编译并在 PC 上仿真。
5. 集成到固件，或通过 UART 下载 romfs 资源。

.. toctree::
    :maxdepth: 2

    快速开始 <quick_start>
    设计器使用 <designer>
    HML 文件格式 <hml_format>
    事件系统 <event_system>
    代码生成 <code_generation>
    资源管理 <resource_management>
    编译与仿真 <build_simulation>
    嵌入式部署 <deployment>
