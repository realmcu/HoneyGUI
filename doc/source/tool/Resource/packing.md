# GUI-APP: UI resource 


##  Packing a filesystem image
Take RTL8762D as an example as follows:
* Copy all the generated bin files to the folder ```\src\app\dashboard\application\root_image\root```.
* Double-click ```mkromfs_0xa00000.bat``` in the ```\src\app\dashboard\application\root_image``` directory to execute the script and generate an image of the root folder. A new .bin file and .h file will appear in the directory.
* Between them, .bin is the image file, and .h is the address offset of each file in the file system, which can be accessed directly without using the file system.


![Image description](https://foruda.gitee.com/images/1688020664001827838/9ac00e5e_10088396.png "20230629143256.png")



























