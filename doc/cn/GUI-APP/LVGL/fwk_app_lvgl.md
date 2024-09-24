# 使用LVGL设计应用程序

LVGL是最受欢迎的免费开源嵌入式图形库，可为任何MCU、MPU和显示类型创建漂亮的用户界面。LVGL（Light and Versatile Graphics Library）是一个免费开源的图形库，提供了一切你需要创建嵌入式GUI所需的功能，包括易于使用的图形元素、美观的视觉效果和低内存占用。

## LVGL介绍

- [LVGL](https://lvgl.io/)

- [LVGL快速入门](https://lvgl.io/get-started)

- [LVGL文档](https://docs.lvgl.io/master/intro/index.html)

## LVGL基准测试

LVGL 的 Benchmark 是一个性能测试工具，用于评估 LVGL 库在各种硬件和软件环境下的图形显示性能。通过运行 Benchmark，用户可以获取帧率、渲染速度和内存使用情况等数据，从而帮助优化显示配置和调试性能问题。Benchmark 包括多种测试场景，如图形绘制、动画和文本渲染，每个场景模拟实际应用中的常见操作。用户可以通过这些测试来比较不同配置和平台的性能表现，从而做出针对性的优化调整。
LVGL基准测试的官方文档位于`\lvgl\demos\benchmark\README.md`。

### LVGL基准测试结果

|   芯片   | 处理器主频 | 加速器 | 显示面积 |   缓冲区配置  |                结果               |
|:--------:|:----------:|:------:|:--------:|:-------------:|:---------------------------------:|
| RTL8762E |    40MHz   |   SW   |  240*280 | Double Buffer | Weighted FPS:15; Opa. speed: 100% |
| RTL8762E |    40MHz   |   SW   |  80*160  | Double buffer |  Weighted FPS:34; Opa. speed: 95% |
| RTL8762D |    90MHz   |   SW   |  240*280 | Double buffer | Weighted FPS:161; Opa. speed: 77% |
| RTL8762D |    90MHz   |   SW   |  80*160  | Double buffer | Weighted FPS:337; Opa. speed: 95% |
| RTL8772G |   125MHz   | PPE1.0 |  480*480 |  Two buffing  | Weighted FPS:20; Opa. speed: 100% |
| RTL8772G |   125MHz   | PPE1.0 |  240*280 | Double buffer | Weighted FPS:721; Opa. speed: 77% |
| RTL8773E |   100MHz   | PPE2.0 |  390*450 | Double buffer | Weighted FPS:159; Opa. speed: 86% |
