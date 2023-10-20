# GUI-APP: UI resource 
## 2.1 Image format conversion
    Convert pictures in various formats into RGB raw pictures
* Open the converter. ```ImageConvert\ImageConverter.exe``` 
* The operation steps and detailed instructions are as follows: 
![Image description](https://foruda.gitee.com/images/1686896876324689552/b3be8646_10088396.png "converter.png")

### 2.1.1 Configuration
- compress : Enable image compression
- scanMode : Select whether the scan direction is horizontal or vertical, BeeGUI only wants horizontal.
- Color Space : Select colorSpace (RGB565, RGBA, BINARY...), BeeGUI can display all of them.
- Color head : BeeGUI wants this head to display.

### 2.1.2 Color Space

- RGB565: Colorful but with low rendering cost and storage. 2 bytes per pixel.


| 5bit red | 6bit green | 5bit blue |
|----------|------------|-----------|


- RGB : True color. 3 bytes per pixel.

| 8bit red | 8bit green | 8bit blue |
|----------|------------|-----------|

- RGBA : True color with opacity. Enhance the display quality with transparency effects. 4 bytes per pixel.

| 8bit red | 8bit green | 8bit blue | 8bit opacity |
|----------|------------|-----------|--------------|

- BINARY : Use one bit for a pixel.



## 2.2 Packing a filesystem image
* Copy all the generated bin files to the folder ```\src\app\dashboard\application\root_image\root```.
* Double-click ```mkromfs_0xa00000.bat``` in the ```\src\app\dashboard\application\root_image``` directory to execute the script and generate an image of the root folder. A new .bin file and .h file will appear in the directory.
* Between them, .bin is the image file, and .h is the address offset of each file in the file system, which can be accessed directly without using the file system.
![Image description](https://foruda.gitee.com/images/1688020664001827838/9ac00e5e_10088396.png "20230629143256.png")

## 2.3 Download to the EVB
Open the MPTool tool to download UI resources, and download the generated image file to the specified address (such as the file system mount address).
* Select Userdata in the MPTool
![IMAGE](https://foruda.gitee.com/images/1661756726607530695/04525d61_11406702.png "IMAGE")
* After the file preparation is completed, you can start downloading.
![IMAGE](https://foruda.gitee.com/images/1661757159634868348/d20d4a49_11406702.png "IMAGE")
























