使用 VSCode 插件开发应用程序
===============================

HoneyGUI Visual Designer 是一个强大的 VSCode 扩展,为嵌入式 GUI 应用程序开发提供可视化设计环境、代码生成和实时仿真工具。

主要特性
---------

* **完全离线开发**：无需网络依赖,所有功能本地运行
* **可视化设计**：拖放式设计界面,所见即所得
* **HML 文件格式**：基于 XML 的界面描述语言,版本控制友好
* **代码保护机制**：用户代码永不覆盖,支持多次重新设计
* **一站式工作流**：设计、编码、编译、仿真全流程集成
* **跨平台支持**：Windows/Linux/macOS 全平台支持

开发流程
---------

使用 VSCode 插件开发 HoneyGUI 应用的典型流程：

1. **创建项目** - 使用模板或从头开始
2. **可视化设计界面** - 拖放组件,配置属性
3. **生成 C 代码** - 自动扫描所有 HML 文件
4. **编译仿真** - SCons + GCC + SDL2
5. **部署到嵌入式设备** - 选择合适的部署方案

.. toctree::
    :maxdepth: 2

    快速开始 <quick_start>
    设计器使用 <designer>
    HML 文件格式 <hml_format>
    代码生成机制 <code_generation>
    事件系统 <event_system>
    资源管理 <resource_management>
    编译与仿真 <build_simulation>
    嵌入式部署 <deployment>
