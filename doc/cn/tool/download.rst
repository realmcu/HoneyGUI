========
烧录工具
========

MP Tool 支持调试模式和批量生产模式，集成了打包和 flash map 生成功能。

+ 调试模式：为开发人员提供一个平台，用于调试和功能开发。
+ 批量生产模式：提供一系列功能，包括能够同时支持多达 8 个设备 flash 的下载和修改设备的蓝牙地址。

下载到 EVB 中
--------------
在 MP Tool 启动界面选择芯片类型和语言，以 RTL8762G 为例。

.. figure:: https://foruda.gitee.com/images/1735624989517380575/77a119ab_13408154.png
   :width: 800px
   :align: center

   MP Tool 启动界面

+ 加载烧录所需的文件，包含 flash map、System Config File、:term:`APP` Image 等。 
+ 选择 User Data。

.. figure:: https://foruda.gitee.com/images/1761026187912386940/43f9518e_13408154.png
   :width: 800px
   :align: center

   MP Tool 主界面

+ 将生成的图像文件下载到指定的地址（例如文件系统挂载地址）， RTL8762G 的地址为 0x04400000。

.. figure:: https://foruda.gitee.com/images/1761015541576180221/a4c2ba8b_13408154.png
   :width: 800px
   :align: center

   User Data 加载界面

+ 文件准备完成后，先检测 UART 端口，正常则显示 :guilabel:`Ready`。然后打开 UART 端口，显示 :guilabel:`OK` 后即可点击 :guilabel:`Download` 烧录。

.. figure:: https://foruda.gitee.com/images/1761017037961739146/da1c9699_13408154.png
   :width: 900px
   :align: center

   进入烧录模式界面
