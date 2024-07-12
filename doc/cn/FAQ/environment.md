# 开发环境常见问题

## 在VSCode中使用模拟器

如果您首次在VSCode中运行模拟器时遇到问题，请检查开发环境中的以下配置：

### 安装适当版本的工具链

在VSCode中使用MinGW工具链进行模拟器开发（参见入门指南中的第3.1节），推荐使用MinGW版本8.1.0，您可以从 [MinGW v8.1.0 Download](https://sourceforge.net/projects/mingw-w64/files/Toolchains%20targetting%20Win64/Personal%20Builds/mingw-builds/8.1.0/threads-posix/sjlj/x86_64-8.1.0-release-posix-sjlj-rt_v6-rev0.7z)下载获取。不能保证所有更新版本的MinGW能够正常工作。
> 已知问题：VSCode目前不支持使用高于v9.2.0的MinGW中的gdb版本。（推荐使用MinGW v8.1.0中的gdb v8.1）

### 添加工具链到系统环境变量

请确保 `C:/mingw64/bin` 已添加到系统环境变量 `Path`中。
