# 生成代码流程图步骤

## 安装 graphviz

请参考官网安装说明：https://graphviz.org/download/

安装完成后，将 graphviz 安装目录添加到环境变量中。

## 运行 doxywizard

1. 打开 doxywizard，点击 ``File`` -> ``Open``，选择 ``doxygen_graphviz/Doxyfile`` 文件。
2. 根据需要可以修改 ``source code directory``， 该目录为需要生成流程图的代码目录，默认是 ``Lite3D/widget``。

<div align=center><img width= "600"  src ="https://foruda.gitee.com/images/1757488330596915483/cf7dd978_13408154.png"/></div><br/>

3. 点击 ``Run`` -> ``Run doxygen`` 生成流程图，等完成后打开 ``doxygen_graphviz/_build/html/index.html`` 文件。

