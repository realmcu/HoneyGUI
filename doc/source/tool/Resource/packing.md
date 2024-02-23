# GUI-APP: UI resource 

## RTL87x2G
RTL87x2G is the abbreviation of a series IC type.
Take RTL8762G as an example as follows.

Before starting, select the appropriate demo under the sdk directory (`\subsys\gui\gui_engine\example\screen_800_480\root_image_800_480`), or create a new packaging directory based on the example. 
Then copy the `bat and py scripts` to that directory, ensuring that the `root folder` and the `bat and py scripts` exist under the directory.
<br/>
<div style="text-align: center"><img width= "600" src="https://foruda.gitee.com/images/1706842716741631765/5fa58e98_13671125.png "></div>
<br/>

1. Copy `resource` all the generated bin files to the `root` folder.
2. Double-click the batch file (`.bat`) to run it, which will execute the packaging process and generate `.bin` file and `.h` file.
3. The `.h` is the address offset of each file in the file system, which can be accessed directly without using the file system. Before developing gui code, please add the folder containing `.h` to the include directory.
4. Use the MPTool tool to burn the `.bin `file into Flash memory.

##  RTL8762D
Take RTL8762D as an example as follows:
1. Copy all the generated bin files to the folder ```\src\app\dashboard\application\root_image\root```.
2. Double-click ```mkromfs_0xa00000.bat``` in the ```\src\app\dashboard\application\root_image``` directory to execute the script and generate an image of the root folder. A new `.bin` file and `.h` file will appear in the directory.
3. Between them, `.bin` is the image file, and `.h` is the address offset of each file in the file system, which can be accessed directly without using the file system.
<br/>
<div style="text-align: center"><img width= "600" src="https://foruda.gitee.com/images/1688020664001827838/9ac00e5e_10088396.png "></div>
<br/>

##  RTL8763E and RTL8773DO
RTL8763E is the name of a series IC type, including RTL8763EWE-VP/RTL8763EW-VC. RTL8763E and RTL877DO have different packaging processes as follows.

### Unzip userdata.zip
Unzip userdata.zip and move files in userdata.zip to sdk/src/sample/bt_audio_trx/panel/userdata.
<br/>
<div style="text-align: center"><img width= "600" src="https://foruda.gitee.com/images/1706846422105482929/bba657c8_13671125.png " ></div>
<br/>

### Set configuration
Bat file has some distinctive due to IC type.
If IC is RTL8763EWE-VP or RTL8763EW-VC.
<br/>
<div style="text-align: center"><img width= "600" src="https://foruda.gitee.com/images/1706846422059814248/38baffe8_13671125.png " ></div>
<br/>

If IC is RTL8773DO.
<br/>
<div style="text-align: center"><img width= "600" src="https://foruda.gitee.com/images/1706846422296936616/21ef576c_13671125.png " ></div>
<br/>

Please set bat configuration due to own IC type.

### Generate userdata bin
Generate process as follows:
1. Copy all the generated bin files to the folder ```\src\sample\bt_audio_trx\panel\user_data\root```.
2. Double-click ```gen_root_image.bat``` in the ```\src\sample\bt_audio_trx\panel\user_data``` directory to execute the script and generate an image of the root folder. A new .bin file and .h file will appear in the directory.
3. Between them, `.bin` is the image file, and `.h` is the address offset of each file in the file system, which can be accessed directly without using the file system.
<br/>
<div style="text-align: center"><img width= "600" src="https://foruda.gitee.com/images/1706846422190018072/39526a71_13671125.png " ></div>
<br/>


##  RTL8773E
RTL8773E is the name of a series IC type, including RTL8773EWE/RTL8773EWE-VP, The packaging process is as follows

### Generate root bin
1. Copy all the generated bin files to the folder ```\src\app\HoneyGUI\gui_engine\example\screen_454_454_bb2plus_watch\root_image\root```.
2. Double-click ```mkromfs_0x4400000.bat``` in the ```\src\app\HoneyGUI\gui_engine\example\screen_454_454_bb2plus_watch\root_image``` directory to execute the script and generate an image of the root folder. A new `.bin: root(0x4400000).bin` file and `.h: ui_resource.h` file will appear in the directory.
3. Between them, `.bin` is the image file, and `.h` is the address offset of each file in the file system, which can be accessed directly without using the file system.
<br/>
<div style="text-align: center"><img width= "600" src="https://foruda.gitee.com/images/1707042532979406541/35eceac9_13674272.png " ></div>
<br/>

### Generate userdata bin
It needs to use MPPGTOOL to generate user data Generate process as follows:
<br/>
<div style="text-align: center"><img width= "600" src="https://foruda.gitee.com/images/1707186704197038539/69fcffc1_13674272.png " ></div>
<br/>   
Note: The Max size must be larger than the Actual size; otherwise, the user data size in the flash_map needs to be changed
