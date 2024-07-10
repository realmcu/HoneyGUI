# 字体转换工具

字体转换工具功能：从标准内部码表(codepage文件)、自定义Unicode码表(或补充码表.txt文件，cST文件)中获取所有待转换字符对应的Unicode码。并根据字体文件(如.ttf)中的Unicode代码查找向量字体数据对应的字符。转换为位图，输出为.bin文件。

<div style="text-align: center"><img width= "500" src="https://foruda.gitee.com/images/1721186859304603819/25785a2f_13408154.png" ></div>

## 生成字体bin文件
如何生成对应的文件，请参考以下步骤:
* 将字体库文件复制到该目录 (`\Font Convert Tool\font`)。

<div style="text-align: center"><img width= "400" src="https://foruda.gitee.com/images/1718779202121064741/2e5506f6_13408154.png" ></div>

* 可以参考(`\Font Convert Tool\doc`)目录下的文档，了解每个参数的具体含义，通过编辑FontConfig.json配置字体参数。

<div style="text-align: center"><img width= "400" src="https://foruda.gitee.com/images/1718779327750693194/d2cd240c_13408154.png" ></div>

* 双击fontDirctionary.exe生成font.bin。

<div style="text-align: center"><img width= "400" src="https://foruda.gitee.com/images/1718779549743952722/46c77609_13408154.png" ></div>

## FontConfig.json参数说明

| 字段名 | 字段含义|
| ----- | ---- |
| codePages | 按一种语言文本的特定顺序排列的选定字符的列表。可设置多个|
| cstPaths | 二进制Unicode码表.cst文件路径。可设置多个|
| mappingPaths|用户定义的unicode字符集文件路径。可以设置多个分组|
|customerVals|用户定义的连续unicode字符。可以设置多个分组|
|firstVal|自定义连续unicode字符的起始值|
|range|自定义的连续unicode字符数|
|fontSet|用于指定要转换的与字体相关的设置|
|bold|指定是否保留转换后的字符|
|italic|指定转换后的字符是否倾斜|
|scanMode|指定如何保存转换后的字符数据。如果该值为“H”，则字体按行保存；如果该值为“V”，则字体按列保存|
|fontSize|指定转换后的字符大小|
|font|指定要用于转换的字体文件|
|renderMode|指定在转换后的字符位图中使用多少位来表示像素|
|indexMethod|指定转换后输出bin文件的重索引区域的索引方式，地址：0；偏移：1|




   


   
    

   































