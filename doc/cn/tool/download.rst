========
烧录工具
========

MP Tool支持调试模式和批量生产模式，集成了打包和flash map生成功能。

+ 调试模式：为开发人员提供一个平台，用于调试和功能开发。
+ 批量生产模式：提供一系列功能，包括能够同时支持多达8个设备flash的下载和修改设备的蓝牙地址。

下载到EVB中
------------
在MP Tool启动界面选择芯片类型和语言，以RTL8762G为例。

.. figure:: https://foruda.gitee.com/images/1735624989517380575/77a119ab_13408154.png
   :width: 800px
   :align: center

   MP Tool启动界面

+ 加载烧录所需的文件，包含flash map、System Config File、APP Image等。 
+ 选择User Data。

.. figure:: https://foruda.gitee.com/images/1727331689141596564/ebfcbd14_13408154.png
   :width: 800px
   :align: center

   MP Tool 主界面

+ 将生成的图像文件下载到指定的地址（例如文件系统挂载地址），8762G的地址为0x04400000。

.. figure:: https://foruda.gitee.com/images/1727331747989671600/86b564da_13408154.png
   :width: 800px
   :align: center

   User Data加载界面

+ 文件准备完成后，先检测UART端口，正常则显示 :guilabel:`Ready`。然后打开UART端口，显示 :guilabel:`OK` 后即可点击 :guilabel:`Download` 烧录。

.. figure:: https://foruda.gitee.com/images/1728973687663258532/d05f55d2_13408154.png
   :width: 900px
   :align: center

   进入烧录模式界面
