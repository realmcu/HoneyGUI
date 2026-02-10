==========================
Resource Processing Tools
==========================

Resource processing tools are used to convert various resource file formats (images, fonts, 3D models, videos, etc.) into binary formats usable by embedded devices and package them.

**VSCode Plugin Resource Conversion Tool**

An integrated solution built into the HoneyGUI Visual Designer plugin:

- **Features**: Resource conversion + automatic packaging
- **Characteristics**: Visual interface, drag-and-drop operation, one-click conversion and packaging
- **Output**: Resource bin files + ``app_romfs.bin`` + ``app_romfs.c`` + ``ui_resource.h``, supports both file system access and direct memory address access
- **Use Cases**: Multiple resource files, complex format conversion

**Standalone Conversion Tools**

Exe programs that need to be downloaded and run separately:

- **Features**: Resource conversion only
- **Characteristics**: GUI interface, batch conversion
- **Output**: Resource bin files
- **Use Cases**: Standalone use or batch conversion
- **Note**: After conversion, packing tool is required for packaging

**Packing Tool**

Packing script depends on Python environment:

- **Features**: Package individual resource bin files
- **Output**: ``root_xxx.bin`` + ``ui_resource.h``
- **Use Cases**: Used with standalone conversion tools


.. note::
   We recommend using the VSCode plugin tools, which integrate conversion and packaging functions, making operation simpler without additional packaging steps.


.. toctree::
    :maxdepth: 2

    VSCode Plugin Resource Conversion Tool <resource_converter>
    Image Conversion Tool <image>
    Font Conversion Tool <font>
    Packing Tool <packing>
