# MP Tool

MP Tool supports debugging mode and batch production mode, integrating packaging and flash map generation functions.

* Debug Mode: Offers developers a platform for debugging and feature development.
* MP Mode: Provides an array of capabilities, including the ability to program up to 8 devices concurrently and modify the device's Bluetooth address.

##  Download to the EVB
Select the chip type and language in the MP Tool startup interface, taking RTL8762G as an example.
<br/>
<div style="text-align: center"><img width= "600" src="https://foruda.gitee.com/images/1727331633078866623/859a389b_13408154.png" alt="IMAGE"></div>
<br/>

* Load the necessary files for burning, including flash map, System Config File, APP Image, etc.
* Select User Data.
<br/>
<div style="text-align: center"><img width= "600" src="https://foruda.gitee.com/images/1727331689141596564/ebfcbd14_13408154.png" alt="IMAGE"></div>
<br/>

* Download the generated image file to the specified address (such as the file system mount address), where the 8762G address is 0x04400000.
<br/>
<div style="text-align: center"><img width= "600" src="https://foruda.gitee.com/images/1727331747989671600/86b564da_13408154.png" alt="IMAGE"></div>
<br/>

* After the file preparation is completed, first check the UART port. If it is normal, it will display 'Ready'. Then open the UART port and display 'OK'. Once this is done, click 'Download' to start the burning process.
<br/>
<div style="text-align: center"><img width= "700" src="https://foruda.gitee.com/images/1728973687663258532/d05f55d2_13408154.png" 
alt="IMAGE"></div>
<br/>
























