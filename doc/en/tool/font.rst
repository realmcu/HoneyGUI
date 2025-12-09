==================
Font Convert Tool
==================

Font conversion tool features: Obtain the Unicode code corresponding to all characters to be converted from the standard internal code table (codepage file), custom Unicode code table (or supplementary code table .txt file, custom .cst file), and find the vector font data corresponding to characters according to the Unicode code from the font file (such as .ttf). Convert to a bitmap, and the output is a .bin file.

.. figure:: https://foruda.gitee.com/images/1725358504194067891/e9f8aa2f_13674272.png
   :width: 500px
   :align: center

   Font Convert Tool Flowchart

Font Bin Generation
--------------------
Please refer to the following steps for how to generate files:

1. Copy the Font library file to the directory ``\Font Convert Tool\font``.

.. figure:: https://foruda.gitee.com/images/1718779202121064741/2e5506f6_13408154.png
   :width: 500px
   :align: center

   Copy Font Library File

2. Please refer to the documentation under the directory ``\Font Convert Tool\doc`` for the specific meanings of each parameter to configure font parameters by editing :red-text:`FontConfig.json`.

.. figure:: https://foruda.gitee.com/images/1724034571057169577/1a91aa47_9325830.png
   :width: 500px
   :align: center

   Edit FontConfig.json File

3. Please open :red-text:`setting.ini` and modify the optional configuration items.

.. figure:: https://foruda.gitee.com/images/1724035284336578868/8affdd56_9325830.png
   :width: 500px
   :align: center

   Edit Setting.ini File

4. Double-click :red-text:`fontDirctionary.exe` and the font bin will be generated.

.. figure:: https://foruda.gitee.com/images/1718779549743952722/46c77609_13408154.png
   :width: 500px
   :align: center

   Generate Font.bin File

FontConfig.json Parameter Description
-------------------------------------

.. table:: FontConfig.json parameter description
   :widths: 25 75 
   :align: center
   :name: FontConfig.json parameter description

   +--------------+--------------------------------------------------------------------------------------------------------------------------+
   | Field name   | Field meaning                                                                                                            |
   +==============+==========================================================================================================================+
   | codePages    | A list of selected characters arranged in a specific order for the text of a language. Multiple sets can be configured.  |
   +--------------+--------------------------------------------------------------------------------------------------------------------------+
   | cstPaths     | Binary Unicode code point CST file path. Multiple paths can be set.                                                      |
   +--------------+--------------------------------------------------------------------------------------------------------------------------+
   | customerVals | User-defined continuous Unicode characters. Multiple groups can be set.                                                  |
   +--------------+--------------------------------------------------------------------------------------------------------------------------+
   | firstVal     | The starting value of a custom continuous Unicode character.                                                             |
   +--------------+--------------------------------------------------------------------------------------------------------------------------+
   | range        | A custom number of consecutive Unicode character.                                                                        |
   +--------------+--------------------------------------------------------------------------------------------------------------------------+
   | mappingPaths | User-defined Unicode character set file path. Multiple groups can be set.                                                |
   +--------------+--------------------------------------------------------------------------------------------------------------------------+
   | fontSet      | Used to specify font-related settings to be converted.                                                                   |
   +--------------+--------------------------------------------------------------------------------------------------------------------------+
   | bold         | Specifies whether converted characters are bolded.                                                                       |
   +--------------+--------------------------------------------------------------------------------------------------------------------------+
   | italic       | Specifies whether the converted characters are slanted.                                                                  |
   +--------------+--------------------------------------------------------------------------------------------------------------------------+
   | scanMode     | Specifies how the converted character data is saved. If the value is 'H', the fonts are saved by row;                    |
   |              | if the value is 'V', the fonts are saved by column.                                                                      |
   +--------------+--------------------------------------------------------------------------------------------------------------------------+
   | fontSize     | Specifies the converted character size.                                                                                  |
   +--------------+--------------------------------------------------------------------------------------------------------------------------+
   | font         | Specifies the font file to use for the conversion.                                                                       |
   +--------------+--------------------------------------------------------------------------------------------------------------------------+
   | renderMode   | Specifies how many bits are used to represent a pixel in the converted character bitmap. Supports 1/2/4/8.               |
   +--------------+--------------------------------------------------------------------------------------------------------------------------+
   | indexMethod  | Specifies the index mode of the re-index area of the output bin file after conversion, and fills the address             |
   |              | index with 0; offset index 1. When the number of characters exceeds 100, it is recommended to choose index mode 0.       |
   +--------------+--------------------------------------------------------------------------------------------------------------------------+
   | crop         | Compresses font file size. Always on is recommended. Currently only crop with IndexMethod=0 is supported.                |
   +--------------+--------------------------------------------------------------------------------------------------------------------------+

Setting.ini Parameter Description
---------------------------------
.. table:: Setting.ini parameter description
   :widths: 25 20  55
   :align: center
   :name: Setting.ini parameter description

   +--------------+--------+-------------------------------------------------------------------------------------------------------+
   | gamma        | 1      | The gamma value is a parameter used to describe the nonlinear relationship between input pixel        |
   |              |        | values and output brightness. The higher the value, the higher the text brightness.                   |
   +--------------+--------+-------------------------------------------------------------------------------------------------------+
   | rotate       | 0      | Font Rotation Angle. 0: No rotation. 1: Rotate 90° clockwise. 2: Rotate 90° counterclockwise.         |
   +--------------+--------+-------------------------------------------------------------------------------------------------------+
