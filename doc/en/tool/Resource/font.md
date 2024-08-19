# Font Convert Tool

Font conversion tool features: Obtain the Unicode code corresponding to all characters to be converted from the standard internal code table (codepage file), custom Unicode code table (or supplementary code table .txt file, custom .cst file), and find the vector font data corresponding characters according to the Unicode code from the font file (such as .ttf). Convert to a bitmap, and the output is a .bin file.

<div style="text-align: center"><img width= "500" src="https://foruda.gitee.com/images/1724051635820787454/a69d7ac6_13408154.png" ></div>

## Font bin generation
Please refer to the following steps for how to generate files:
* Copy the Font library file to the directory (`\Font Convert Tool\font`).

<div style="text-align: center"><img width= "400" src="https://foruda.gitee.com/images/1718779202121064741/2e5506f6_13408154.png" ></div>

* You can refer to the documentation under the directory (`\Font Convert Tool\doc`) for the specific meanings of each parameter to config font parameters by editing `FontConfig.json`.

<div style="text-align: center"><img width= "400" src="https://foruda.gitee.com/images/1724034571057169577/1a91aa47_9325830.png" ></div>

* Please open `setting.ini` and modify the optional configuration items.

<div style="text-align: center"><img width= "400" src="https://foruda.gitee.com/images/1724035284336578868/8affdd56_9325830.png" ></div>

* Double-click `fontDirctionary.exe` and then font bin will be generated.

<div style="text-align: center"><img width= "400" src="https://foruda.gitee.com/images/1718779549743952722/46c77609_13408154.png" ></div>

## FontConfig.json Parameter description

| Field name   | Field meaning                                                                                                |
| ------------ | ------------------------------------------------------------------------------------------------------------- |
| codePages    | A list of selected characters arranged in a specific order for the text of a language. Can be set multiple    |
| cstPaths     | Binary Unicode code table cst file path. Can be set multiple                                                  |
| customerVals | User-defined continuous unicode characters. Multiple groups can be set                                        |
| firstVal     | The starting value of a custom continuous unicode character                                                   |
| range        | A custom number of consecutive unicode character                                                              |
| mappingPaths | User-defined unicode character set file path. Multiple groups can be set                                      |
| fontSet      | Used to specify font-related settings to be converted                                                         |
| bold         | Specifies whether converted characters are bolded                                                             |
| italic       | Specifies whether the converted characters are slanted                                                          |
| scanMode     | Specifies how the converted character data is saved. If the value is "H", the fonts are saved by row; if the value is "V", the fonts are saved by column. |
| fontSize     | Specifies the converted character size                                                                        |
| font         | Specifies the font file to use for the conversion                                                             |
| renderMode   | Specifies how many bits are used to represent a pixel in the converted character bitmap. Support 1/2/4/8      |
| indexMethod  | Specify the index mode of the re-index area of the output bin file after conversion, and fill the address index with 0; offset index 1. When the number of characters exceeds 100, it is recommended to choose index mode 0. |
| crop         | Compress font file size. Always on is recommended. Currently only crop with IndexMethod=0 is supported.       |

## Setting.ini Parameter description

| Field name   | Default value | Field meaning                                                                                                 |
| ------------ | ------------- | ------------------------------------------------------------------------------------------------------------- |
| gamma        | 1             | The gamma value is a parameter used to describe the nonlinear relationship between input pixel values and output brightness. The higher the value, the higher the text brightness.|
| rotate       | 0             | Font Rotation Angle. 0: No rotation. 1: Rotate 90° clockwise. 2: Rotate 90° counterclockwise.                 |
