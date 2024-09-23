# 烧录工具

MP Tool支持调试模式和批量生产模式，集成了打包和flash map生成功能。

* 调试模式：为开发人员提供一个平台，用于调试和功能开发。
* 批量生产模式：提供一系列功能，包括能够同时支持多达8个设备flash的下载和修改设备的蓝牙地址。

##  下载到EVB中
在MP Tool启动界面选择芯片类型和语言，以RTL8762D为例。
<br/>
<div style="text-align: center"><img width= "600" src="https://foruda.gitee.com/images/1724143547149951697/bc248044_13408154.png" alt="IMAGE"></div>
<br/>

* 加载烧录所需的文件，包含flash map、System Config File、APP Image等。 
* 选择User Data。
<br/>
<div style="text-align: center"><img width= "600" src="https://foruda.gitee.com/images/1721812772825414074/8d3f7123_13408154.png" alt="IMAGE"></div>
<br/>

* 将生成的图像文件下载到指定的地址（例如文件系统挂载地址），8762D的地址为0x00A00000。
<br/>
<div style="text-align: center"><img width= "600" src="https://foruda.gitee.com/images/1721812789415438915/6badc5e0_13408154.png" alt="IMAGE"></div>
<br/>

* 文件准备完成后，先检测UART端口，正常则显示“Ready”，然后打开UART端口，显示“OK”后即可点击“Download”烧录。
<br/>
<div style="text-align: center"><img width= "600" src="https://foruda.gitee.com/images/1721812809226499244/2d835631_13408154.png" 
alt="IMAGE"></div>
<br/>
