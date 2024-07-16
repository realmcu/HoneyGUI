# Font Convert Tool

Font conversion tool features: Obtain the Unicode code corresponding to all characters to be converted from the standard internal code table (codepage file), custom Unicode code table (or supplementary code table.txt file, custom.cST file), and find the vector font data corresponding characters according to the Unicode code from the font file (such as.ttf). Convert to a bitmap, and the output is a.bin file.

<div style="text-align: center"><img width= "500" src="https://foruda.gitee.com/images/1708226065411340283/b7deeeff_13674272.png" ></div>

## Font bin generation
Please refer to the following steps for how to generate files:
* Copy the Font library file to the directory (`\Font Convert Tool\font`).

<div style="text-align: center"><img width= "400" src="https://foruda.gitee.com/images/1718779202121064741/2e5506f6_13408154.png" ></div>

* You can refer to the documentation under the directory (`\Font Convert Tool\doc`) for the specific meanings of each parameter to config font parameters by edit FontConfig.json.

<div style="text-align: center"><img width= "400" src="https://foruda.gitee.com/images/1718779327750693194/d2cd240c_13408154.png" ></div>

* Double-click fontDirctionary.exe and then font bin will be Generated.

<div style="text-align: center"><img width= "400" src="https://foruda.gitee.com/images/1718779549743952722/46c77609_13408154.png" ></div>

## FontConfig.json Parameter description

| field name | Field meaning|
| ----- | ---- |
| codePages | A list of selected characters arranged in a specific order for the text of a language. Can be set multiple |
| cstPaths | Binary Unicode code table cst file path. Can be set multiple |
| mappingPaths|User - defined unicode character set file path. Multiple groups can be set|
|customerVals|User-defined continuous unicode characters. Multiple groups can be set|
|firstVal|The starting value of a custom continuous unicode character|
|range|A custom number of consecutive unicode character|
|fontSet|Used to specify font-related Settings to be converted|
|bold|Specifies whether converted characters are bolded|
|italic|Specifies whether the converted character is slanted|
|scanMode|Specifies how the converted character data is saved. If the value is "H", the fonts are saved by row, and if the value is "V", the fonts are saved by column|
|fontSize|Specifies the converted character size|
|font|Specifies the font file to use for the conversion|
|renderMode|Specifies how many bits are used to represent a pixel in the converted character bitmap|
|indexMethod|Specify the index mode of the re-index area of the output bin file after conversion, and fill the address index with 0; offset index 1|




   


   
    

   































