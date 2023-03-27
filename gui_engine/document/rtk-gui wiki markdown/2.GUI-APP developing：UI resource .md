## 2.1 Image format conversion
    Convert pictures in various formats into RGB raw pictures
* Open the converter. ```rtk-gui-app-designer\WristbandGUI.exe``` 
* The operation steps and detailed instructions are as follows：
![输入图片说明](https://foruda.gitee.com/images/1669961948006214471/cf69a8dd_10088396.jpeg "20221202141623.jpg")
1. Click on Image button to select the picture folder that needs to be converted.
The left side is the description of some attributes, which can be modified according to the needs：
    - compressFlag 
    - alignFlag 
    - scanMode 
    - colorSpace 
2. Click the wrench icon to set the output path.
Select the output path in the Bin Path input box. It is recommended to create a new folder to save the generated files. Color Head needs to be turned on.
3. Perform the conversion.
After which the newly generated bin files will be displayed under the ```resource``` directory in the output folder.

## 2.2 Packing a filesystem image
* Copy all the generated bin files to the folder ```\sdk\src\app\gui_engine\example\resource\root```.
* Double-click ```mkromfs.bat``` in the ```\sdk\src\app\gui_engine\example\resource``` directory to execute the script and generate an image of the root folder. A new .bin file and .h file will appear in the directory.
* Between them, .bin is the image file, and .h is the address offset of each file in the file system, which can be accessed directly without using the file system.
![输入图片说明](https://foruda.gitee.com/images/1669961917947697024/66502364_10088396.jpeg "20221202141747.jpg")

## 2.3 Download to the EVB
Open the MPTool tool to download UI resources, and download the generated image file to the specified address (such as the file system mount address).
* Select Userdata in the MPTool
![输入图片说明](https://foruda.gitee.com/images/1661756726607530695/04525d61_11406702.png "微信图片_20220829150514.png")
* After the file preparation is completed, you can start downloading.
![输入图片说明](https://foruda.gitee.com/images/1661757159634868348/d20d4a49_11406702.png "微信图片_20220829151224.png")
























