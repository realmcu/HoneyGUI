# 打包工具

## RTL87x2G
RTL87x2G 是一系列IC类型的缩写。
以下以 RTL8762G 为例进行说明。

在开始之前，请在SDK目录下的（`\subsys\gui\gui_engine\example\screen_800_480\root_image_800_480`）选择一个合适的示例，或者基于示例创建一个新的打包目录。
然后，将bat和py脚本复制到该目录中，确保root文件夹以及bat和py脚本都处于该目录下。

<br/>
<div style="text-align: center"><img width= "600" src="https://foruda.gitee.com/images/1718779885964281659/e73b5078_13408154.png"></div>
<br/>

1. 将所有生成的`bin`文件从resource文件夹复制到`root`文件夹中。
2. 双击批处理文件（`.bat`）以运行它，这将执行打包过程并生成root `bin`文件和`.h`文件
3. `.h`文件包含了文件系统中每个文件的地址偏移量，可以在不使用文件系统的情况下直接访问。在开发GUI代码之前，请将包含`.h`的文件夹添加到包含目录中。
4. 使用MPTool工具将`root_xx.bin`文件烧录到Flash存储中。


##  RTL8762D
以下以RTL8762D为例进行说明：
1. 将所有生成的`bin`文件复制到文件夹```\src\app\dashboard\application\root_image\root```中。
2. 在```\src\app\dashboard\application\root_image```目录下，双击`mkromfs_0xa00000.bat`脚本文件以执行它，并生成`root`文件夹的镜像。执行后，该目录下将出现一个新的`.bin`文件和`.h`文件。
3. 其中，`.bin`文件是根文件夹的镜像文件，而`.h`文件则包含了文件系统中每个文件的地址偏移量。这样，开发者可以在不使用文件系统的情况下直接访问这些文件。

<br/>
<div style="text-align: center"><img width= "600" src="https://foruda.gitee.com/images/1718786488596930495/a813d294_13408154.png"></div>
<br/>

##  RTL8763E and RTL8773DO
RTL8763E是一个IC系列的名称，包含RTL8763EWE-VP和RTL8763EW-VC。RTL8763E和RTL877DO有不同的打包流程。

### 解压 userdata.zip
解压 userdata.zip 文件，并将 userdata.zip 中的文件移动到 `sdk/src/sample/bt_audio_trx/panel/userdata` 目录下。

<br/>
<div style="text-align: center"><img width= "600" src="https://foruda.gitee.com/images/1718780909907949565/4ca96246_13408154.png" ></div>
<br/>

### 配置设定
由于IC类型不同，批处理文件（bat）也会不一样。

如果IC类型是RTL8763EWE-VP或RTL8763EW-VC。

<br/>
<div style="text-align: center"><img width= "600" src="https://foruda.gitee.com/images/1718781307571552882/d7f3f5ee_13408154.png " ></div>
<br/>

如果IC类型是RTL8773DO。
<br/>
<div style="text-align: center"><img width= "600" src="https://foruda.gitee.com/images/1718781327901813150/409bf9a6_13408154.png " ></div>
<br/>

请根据自己的IC类型设置批处理（bat）配置。

### 生成用户数据（userdata bin）
生成过程如下：
1. 将所有生成的bin文件复制到文件夹`\src\sample\bt_audio_trx\panel\user_data\root`中。
2. 双击`\src\sample\bt_audio_trx\panel\user_data`目录中的`gen_root_image.bat`文件以执行脚本并生成root文件夹的镜像。一个新的`.bin`文件和`.h`文件将出现在目录中。
3. 在这两个文件中，`.bin`文件是镜像文件，而`.h`文件是文件系统中每个文件的地址偏移量，可以直接访问而无需使用文件系统。

<br/>
<div style="text-align: center"><img width= "600" src="https://foruda.gitee.com/images/1718781540264130584/8ab9bacc_13408154.png" ></div>
<br/>


##  RTL8773E
RTL8773E是一个IC系列的名称，包含RTL8773EWE和RTL8773EWE-VP等，用户数据（userdata）打包处理过程如下

### 生成用户数据（userdata）

1. 将生成的图像`bin`文件复制到 `\src\app\watch\gui_application\root_image\root\8773e_watch`目录，并将生成的字体`bin`文件复制到 `\src\app\watch\gui_application\root_image\root\font`目录。
2. 修改构建地址（`build address`）：开发者需要通过修改 `mkromfs_0x4400000.bat` 文件（python_bin_mkromfs_0x4400000.py --binary `--addr 0x238b400` root root(0x4400000).bin）。这里的 `--addr` 参数对应于闪存映射中的 userdata 地址加上 0x400（图像头部大小）。
3. 在 `\src\app\watch\gui_application\root_image`目录中双击`mkromfs_0x4400000.bat`文件来执行脚本，并生成root文件夹的镜像。一个新的`bin`文件 root(0x4400000).bin 和头文件 ui_resource.h 将出现在目录中。
4. 在这两个文件中，`.bin` 文件是包含root目录内容的镜像文件，而 `.h` 文件则包含了文件系统中每个文件的地址偏移量，这些偏移量允许开发者在不使用文件系统的情况下直接访问这些文件。

<br/>
<div style="text-align: center"><img width= "600" src="https://foruda.gitee.com/images/1718785235574270822/e3a612e2_13408154.png"></div>
<br/>

注意：生成的 ui_resource.h 文件需要手动添加以下代码。

```C
#if defined _WIN32
#else
#include "flash_map.h"

#define   MUSIC_NAME_BIN_ADDR       APP_DEFINED_SECTION_ADDR
#define   MUSIC_HEADER_BIN_ADDR     (MUSIC_NAME_BIN_ADDR + 0xA000)
#define   MUSIC_NAME_BIN_SIZE       (MUSIC_HEADER_BIN_ADDR - MUSIC_NAME_BIN_ADDR)
#define   MUSIC_HEADER_BIN_SIZE     0x5000
#endif
```

### 生成用户数据（userdata bin）
使用MPPGTOOL来生成用户数据，生成过程如下：
<br/>
<div style="text-align: center"><img width= "600" src="https://foruda.gitee.com/images/1718782467179312695/69433669_13408154.png"></div>
<br/>   
注意：最大size必须大于实际size，否则，需要更改 flash_map 中的用户数据大小。