# Use LVGL Design An Application

LVGL is the most popular free and open-source embedded graphics library to create beautiful UIs for any MCU, MPU and display type. LVGL (Light and Versatile Graphics Library) is a free and open-source graphics library providing everything you need to create an embedded GUI with easy-to-use graphical elements, beautiful visual effects and a low memory footprint.

## LVGL Introduction

- [LVGL](https://lvgl.io/)

- [LVGL Get Started](https://lvgl.io/get-started)

- [LVGL Document](https://docs.lvgl.io/master/intro/index.html)

## LVGL Benchmark

LVGL Benchmark is a performance testing tool designed to evaluate the graphical display performance of the LVGL library in various hardware and software environments. By running the Benchmark, users can obtain data on frame rate, rendering speed, and memory usage, helping to optimize display configurations and debug performance issues. The Benchmark includes various test scenarios such as graphical drawing, animations, and text rendering, each simulating common operations in real applications. Users can use these tests to compare the performance of different configurations and platforms, enabling targeted optimization adjustments.
The official documentation for the LVGL benchmark test is located at `\lvgl\demos\benchmark\README.md`.

### LVGL Benchmark Results

|    SOC   | CPU CLK | Accelerator | Display Size | Buffering Configurations |               Result              |
|:--------:|:-------:|:-----------:|:------------:|:------------------------:|:---------------------------------:|
| RTL8762E |  40MHz  |      SW     |    240*280   |       Double Buffer      | Weighted FPS:15; Opa. speed: 100% |
| RTL8762E |  40MHz  |      SW     |    80*160    |       Double buffer      |  Weighted FPS:34; Opa. speed: 95% |
| RTL8762D |  90MHz  |      SW     |    240*280   |       Double buffer      | Weighted FPS:161; Opa. speed: 77% |
| RTL8762D |  90MHz  |      SW     |    80*160    |       Double buffer      | Weighted FPS:337; Opa. speed: 95% |
| RTL8772G |  125MHz |    PPE1.0   |    480*480   |        Two buffing       | Weighted FPS:20; Opa. speed: 100% |
| RTL8772G |  125MHz |    PPE1.0   |    240*280   |       Double buffer      | Weighted FPS:721; Opa. speed: 77% |
| RTL8773E |  100MHz |    PPE2.0   |    390*450   |       Double buffer      | Weighted FPS:159; Opa. speed: 86% |
