# 烧录工具

##  下载到 EVB 中
在MP Tool启动界面选择芯片类型和语言，以RTL8762D为例。
<br/>
<div style="text-align: center"><img width= "600" src="https://foruda.gitee.com/images/1721812755008370232/a9e4ff39_13408154.png" alt="IMAGE"></div>
<br/>

* 加载烧录所需的文件，包含flash map、System Config File、APP Image等。 
* 选择 User Data。
<br/>
<div style="text-align: center"><img width= "600" src="https://foruda.gitee.com/images/1721812772825414074/8d3f7123_13408154.png" alt="IMAGE"></div>
<br/>

* 将生成的图像文件下载到指定的地址（例如文件系统挂载地址），8762D的地址为0x00A00000。
<br/>
<div style="text-align: center"><img width= "600" src="https://foruda.gitee.com/images/1721812789415438915/6badc5e0_13408154.png" alt="IMAGE"></div>
<br/>

* 文件准备完成后，先检测UART端口，正常则显示Ready，然后打开UART端口，显示OK后即可点击Download烧录。
<br/>
<div style="text-align: center"><img width= "600" src="https://foruda.gitee.com/images/1721812809226499244/2d835631_13408154.png" 
alt="IMAGE"></div>
<br/>
























