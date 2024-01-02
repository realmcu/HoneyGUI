# GUI-APP: UI resource 


##  Packing a filesystem image
Take RTL8762D as an example as follows:
* Copy all the generated bin files to the folder ```\src\app\dashboard\application\root_image\root```.
* Double-click ```mkromfs_0xa00000.bat``` in the ```\src\app\dashboard\application\root_image``` directory to execute the script and generate an image of the root folder. A new .bin file and .h file will appear in the directory.
* Between them, .bin is the image file, and .h is the address offset of each file in the file system, which can be accessed directly without using the file system.


![Image description](https://foruda.gitee.com/images/1688020664001827838/9ac00e5e_10088396.png "20230629143256.png")

##  Additional infoamtion for RTL8763E 
RTL8763E is the name of a series IC type, incluing RTL8763EWE-VP/RTL8763EW-VC/RTL877DO. These IC type has defferent packing process as follow. 
### Unzip userdata.zip
Unzip userdata.zip and move files in userdata.zip to sdk/src/sample/bt_audio_trx/panel/userdata.

![Alt text](Userdata_folder.png)

### Set configution 
Bat file has some difference due to IC type.
If IC is RTL8763EWE-VP or RTL8763EW-VC.

![Alt text](RTL87X3E_packing.png)

If IC is RTL8773DO.

![Alt text](RTL87X3D_packing.png)

Please reset bat configution due to onw IC type.

### Generate userdata bin
Generation process please see follow:
* Copy all the generated bin files to the folder ```\src\sample\bt_audio_trx\panel\user_data\root```.
* Double-click ```gen_root_image.bat``` in the ```\src\sample\bt_audio_trx\panel\user_data``` directory to execute the script and generate an image of the root folder. A new .bin file and .h file will appear in the directory.
* Between them, .bin is the image file, and .h is the address offset of each file in the file system, which can be accessed directly without using the file system.

![Alt text](release_bin_and_header.png)

































