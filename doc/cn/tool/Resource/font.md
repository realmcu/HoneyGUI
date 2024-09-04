# 字体转换工具

字体转换工具功能：从标准内部码表（代码页文件）、自定义Unicode码表（或补充码表.txt文件，自定义.cst文件）中获取所有待转换字符对应的Unicode码。根据Unicode编码从字体文件（如.ttf）中找到相应字符的矢量字体数据。将其转换为位图，并输出为.bin文件。

<div style="text-align: center"><img width= "500" src="https://foruda.gitee.com/images/1725358504194067891/e9f8aa2f_13674272.png" ></div>

## 生成字体bin文件
如何生成对应的文件，请参考以下步骤:
* 将字体库文件复制到该目录（`\Font Convert Tool\font`）。

<div style="text-align: center"><img width= "400" src="https://foruda.gitee.com/images/1718779202121064741/2e5506f6_13408154.png" ></div>

* 可以参考（`\Font Convert Tool\doc`）目录下的文档，了解每个参数的具体含义，通过编辑`FontConfig.json`配置字体参数。

<div style="text-align: center"><img width= "400" src="https://foruda.gitee.com/images/1724034571057169577/1a91aa47_9325830.png" ></div>

* 打开`setting.ini`，修改可选配置项。

<div style="text-align: center"><img width= "400" src="https://foruda.gitee.com/images/1724035284336578868/8affdd56_9325830.png" ></div>

* 双击`fontDirctionary.exe`生成font.bin。

<div style="text-align: center"><img width= "400" src="https://foruda.gitee.com/images/1718779549743952722/46c77609_13408154.png" ></div>

## FontConfig.json参数说明

| 字段名称                | 字段含义                                                                           |
| ----------------------- | --------------------------------------------------------------------------------- |
| 代码页 (codePages)      | 一组按特定顺序排列的选定字符，用于某种语言的文本。可以设置多个。                        |
| cst路径 (cstPaths)      | 二进制 Unicode 代码表 cst 文件路径。可以设置多个。                                   |
| 自定义值 (customerVals) | 用户定义的连续 unicode 字符。可以设置多组。                                          |
| 起始值 (firstVal)       | 自定义连续 unicode 字符的起始值。                                                   |
| 范围 (range)           | 自定义的连续 unicode 字符数量。                                                      |
| 映射路径 (mappingPaths) | 用户定义的 unicode 字符集文件路径。可以设置多组。                                     |
| 字体设置 (fontSet)      | 用于指定要转换的字体相关设置。                                                       |
| 加粗 (bold)            | 指定转换后的字符是否加粗。                                                           |
| 斜体 (italic)          | 指定转换后的字符是否倾斜。                                                           |
| 扫描模式 (scanMode)    | 指定转换后的字符数据如何保存。如果值为 "H"，字体按行保存；如果值为 "V"，字体按列保存。    |
| 字号 (fontSize)        | 指定转换后的字符大小。                                                               |
| 字体 (font)            | 指定用于转换的字体文件。                                                             |
| 渲染模式 (renderMode)  | 指定在转换后的字符位图中表示一个像素使用的位数。支持 1/2/4/8。                          |
| 索引方法 (indexMethod) | 指定转换后输出 bin 文件的重索引区域的索引模式，并用 0 填充地址索引；偏移索引用 1。当字符数量超过 100 时，建议选择索引模式 0。 |
| 裁剪 (crop)            | 压缩字体文件大小。建议始终开启。目前仅支持 IndexMethod=0 时的裁剪。                     |

## Setting.ini 参数说明

| 字段名称     | 默认值 | 字段含义                                                                               |
| ------------ | ------ | ------------------------------------------------------------------------------------ |
| gamma        | 1      | gamma 值是用于描述输入像素值和输出亮度之间非线性关系的参数。该值越大，文本亮度越高。        |
| rotate       | 0      | 字体旋转角度。0：不旋转。1：顺时针旋转 90°。2：逆时针旋转 90°。                           |
