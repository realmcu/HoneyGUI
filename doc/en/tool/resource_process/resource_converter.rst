.. _VSCode Plugin Resource Conversion:

========================================
VSCode Plugin Resource Conversion Tool
========================================

HoneyGUI Visual Designer includes a complete set of resource conversion tools that can convert various resource file formats into binary formats usable by embedded devices. These tools are implemented in pure JavaScript/TypeScript and integrated into the plugin's compilation process, requiring no additional Python environment or standalone tools.

.. note::
  
   This chapter introduces the resource conversion tools built into the HoneyGUI Visual Designer VSCode plugin, which differ from traditional standalone conversion tools (exe programs):
   
   - **VSCode Plugin Tools**: Integrated into the plugin, automated conversion, visual configuration, supports images, 3D models, videos, fonts, and glass effects.
   - **Standalone Tools**: Separate exe programs that need to be downloaded and run manually. See :ref:`Image Convert Tool` and :ref:`Font Convert Tool` for details.

Tool Overview
-----------------

The resource conversion tools in HoneyGUI Visual Designer plugin support the following resource types:

- **Image Conversion**: PNG, JPEG, BMP, GIF → bin files
- **3D Model Conversion**: OBJ, GLTF → descriptor bin files
- **Video Conversion**: MP4, AVI, MOV, etc. → MJPEG/AVI/H264 formats
- **Font Conversion**: TTF, OTF → bitmap or vector font bin files
- **Glass Effect Conversion**: SVG (``.glass`` extension) → blur effect bin files

.. _Quick Start:

Quick Start
-----------------

.. figure:: https://foruda.gitee.com/images/1770691129178222517/6964fad9_13408154.png
   :align: center
   :width: 800px

   Plugin Resource Conversion Tool Interface

**Step 1: Open Plugin Panel**

1. In the VSCode activity bar on the left, click the **HoneyGUI Visual Designer** plugin icon.
2. The plugin sidebar will display the **WELCOME** area and **ENVIRONMENT** area.

**Step 2: Check Environment (Optional)**

The resource conversion tool itself does not require additional environment dependencies, but if you need to crop or scale videos, FFmpeg must be installed.

You can check if FFmpeg is installed in the **ENVIRONMENT** area:

- ✅ **FFmpeg**: Optional, only required for video cropping/scaling

**Step 3: Open Resource Conversion Tool**

In the **WELCOME** area, click the :guilabel:`Resource Conversion Tools` button to open the resource conversion tool panel.

**Step 4: Select Output Directory**

1. In the resource conversion tool panel, click the :guilabel:`Select Directory` button.
2. Select a directory as the output location for resource conversion.
3. The plugin will automatically create an ``origin`` folder in the selected directory to store original resource files and configurations.
4. A ``conversion.json`` configuration file will be generated in the ``origin`` folder to save all resource conversion configurations.

**Step 5: Add Resource Files**

Drag and drop resource files to be converted into the resource conversion tool panel:

1. Select resource files from the file manager (images, fonts, videos, 3D models, glass effects, etc.).
2. Drag and drop them into the resource list area of the tool panel.
3. Resources will be automatically added to the list and displayed on the left.


**Step 6: Configure Conversion Parameters**

In the resource conversion tool panel:

1. **Left Resource List**: Displays all added resource files.
2. **Select Resource**: Click on a resource file in the left list.
3. **Right Properties Panel**: Configure conversion parameters for that resource.

   - Images: Format (RGB565, ARGB8888, etc.), compression method (RLE, FastLZ, YUV)
   - Fonts: Font size, render mode (1/2/4/8 bpp), character set, output format
   - Videos: Output format (MJPEG, AVI, H264), frame rate, quality, crop/scale
   - 3D Models: Format automatically recognized, no additional configuration needed
   - Glass Effects: Effect area (0-100%), effect intensity (0-100%)


**Step 7: Set Base Address**

Before starting conversion, you need to set the Base Address:

1. Find the **Base Address** input box in the tool panel.
2. Enter the starting address of resources in device memory (hexadecimal format, e.g., ``0x704D1000``).
3. Base Address is used to calculate resource memory addresses when generating the romfs file system.

.. note::
   The Base Address value depends on your device's memory layout and firmware configuration. Please refer to your device's memory mapping documentation or firmware configuration files.

**Step 8: Start Conversion**

1. After configuring all resources and Base Address, click the :guilabel:`Start Convert` button in the lower right corner.
2. The plugin will automatically perform the following operations:

   - Convert all resource files to binary format.
   - Package resources into romfs file system.
   - Generate :file:`app_romfs.bin`, :file:`app_romfs.c`, and :file:`ui_resource.h`.

3. After conversion is complete, the following files are generated in the output directory:

   - ``Resource bin files``: Binary files for each resource (e.g., :file:`image.bin`, :file:`font.bin`).
   - :file:`app_romfs.bin`: Packaged romfs file system image.
   - :file:`app_romfs.c`: C language array form of romfs (can be compiled directly into the program).
   - :file:`ui_resource.h`: Contains resource address offset macro definitions (supports direct memory address access).

.. tip::
   - Conversion progress will be displayed in real-time in the output panel.
   - If conversion fails, detailed error messages will be displayed.


Image Conversion
-----------------

Supported Formats
^^^^^^^^^^^^^^^^^

Input Formats:
  - PNG (supports transparency)
  - JPEG/JPG
  - BMP
  - GIF (directly packages original data)

Output Formats:
  - RGB565: 16-bit color, 2 bytes per pixel
  - RGB888: 24-bit true color, 3 bytes per pixel
  - ARGB8888: 32-bit with transparency, 4 bytes per pixel
  - ARGB8565: 24-bit with transparency, 3 bytes per pixel
  - A8: 8-bit alpha channel
  - I8: 8-bit grayscale

Compression Algorithms
^^^^^^^^^^^^^^^^^^^^^^^

- **RLE Compression**: Run-length encoding, suitable for images with large solid color areas
- **FastLZ Compression**: Fast compression algorithm, balances compression ratio and decompression speed
- **YUV Compression**: Suitable for photographic images, supports multiple sampling modes

  - YUV444: No chroma sampling, highest quality
  - YUV422: Horizontal 2:1 sampling, recommended
  - YUV411: Horizontal 4:1 sampling, highest compression ratio
  - Blur bits: 0/1/2/4 bit, reduces chroma precision to improve compression ratio
  - FastLZ secondary compression: Optional, further improves compression ratio

Configuration Method
^^^^^^^^^^^^^^^^^^^^^^^

The plugin provides a visual configuration interface without manual editing of configuration files. The configuration interface includes:

- **Format Selection**: Dropdown menu to select output format (RGB565, ARGB8888, etc.)
- **Compression Algorithm**: Select compression method (no compression, RLE, FastLZ, YUV)
- **YUV Parameters**: If YUV compression is selected, configure sampling mode, blur bits, etc.

.. tip::
   Configuration is automatically saved to the ``origin/conversion.json`` file and will be automatically loaded the next time the file is opened.

Adaptive Formats
^^^^^^^^^^^^^^^^^

- **adaptive16**: Automatically selects 16-bit format

  - With transparency → ARGB8565
  - Without transparency → RGB565

- **adaptive24**: Automatically selects 24-bit format

  - With transparency → ARGB8888
  - Without transparency → RGB888

Usage
^^^^^^^^^^^^^^^^^

After opening the resource conversion tool panel following the steps in :ref:`Quick Start`:

1. **Drag Images**: Drag image files from the file manager to the resource list area of the tool panel.
2. **Select Image**: Click to select an image in the left resource list.
3. **Configure Parameters**: Set conversion parameters in the right properties panel.

   - **Output Format**: RGB565, RGB888, ARGB8888, ARGB8565, etc.
   - **Compression Method**: No compression, RLE, FastLZ, YUV.
   - **YUV Parameters** (if YUV compression is selected): Sampling mode, blur bits, secondary compression.

4. **Start Conversion**: Click the :guilabel:`Start Convert` button in the lower right corner.
5. **View Output**: Converted files in the output directory:

   - ``imagename.bin``: Converted image binary file.
   - ``app_romfs.bin``: Packaged file system.
   - ``app_romfs.c``: C array form.
   - ``ui_resource.h``: Address offset macro definitions.


3D Model Conversion
---------------------

Supported Formats
^^^^^^^^^^^^^^^^^

Input Formats:
  - OBJ: Wavefront OBJ format (supports MTL materials)
  - GLTF: GL Transmission Format 2.0 (requires accompanying ``.bin`` file)

Output Files:
  - Descriptor bin file: Contains vertices, normals, texture coordinates, materials, etc.
  - OBJ format: ``desc_xxx.bin``
  - GLTF format: ``gltf_desc_xxx.bin``


Usage
^^^^^^^^^^^^^^^^^

After opening the resource conversion tool panel following the steps in :ref:`Quick Start`:

1. **Drag Model**: Drag 3D model files (``.obj`` or ``.gltf``) to the tool panel.
2. **Drag Textures**: If there are texture images, also drag them to the panel.
3. **View Configuration**: View model information in the right properties panel (usually no additional configuration needed).
4. **Start Conversion**: Click the :guilabel:`Start Convert` button in the lower right corner.
5. **View Output**: Converted files in the output directory:

   - ``desc_xxx.bin`` or ``gltf_desc_xxx.bin``: Model descriptor.
   - ``texturename.bin``: Binary file of texture image.
   - ``app_romfs.bin``, ``app_romfs.c``, and ``ui_resource.h``: Package files.

Notes:

- GLTF files require accompanying ``.bin`` files (buffer data), drag them together.
- Texture images will be automatically converted to bin format and associated with the model.


Video Conversion
-----------------

Supported Formats
^^^^^^^^^^^^^^^^^

Input Formats:
  - MP4, AVI, MOV, MKV, WebM
  - FLV, WMV, M4V, 3GP
  - ASF, RM, RMVB, VOB, TS

Output Formats:
  - **MJPEG**: Motion JPEG, frame-by-frame JPEG encoding, simple decoding
  - **AVI**: AVI container + MJPEG encoding
  - **H264**: H.264/AVC encoding, high compression ratio (requires hardware decoding support)

Conversion Options
^^^^^^^^^^^^^^^^^^^^^

Basic Parameters:
  - **Frame Rate**: Output video frame rate (e.g., 30 fps)
  - **Quality**: MJPEG/AVI is 1-31 (1 is highest), H264 is CRF 0-51

Video Processing:
  - **Crop**: Crop video region (x, y, width, height)
  - **Scale**: Adjust video size (width, height)
  - **Keep Aspect Ratio**: Maintain original aspect ratio when scaling

Usage
^^^^^^^^^^^^^^^^^

After opening the resource conversion tool panel following the steps in :ref:`Quick Start`:

1. **Drag Video**: Drag video files to the resource list area of the tool panel.
2. **Select Video**: Click to select a video file in the left resource list.
3. **Configure Parameters**: Set video conversion parameters in the right properties panel.

   - **Output Format**: MJPEG, AVI, H264.
   - **Frame Rate**: e.g., 30 fps.
   - **Quality**: MJPEG/AVI is 1-31 (1 is highest), H264 is CRF 0-51.
   - **Crop Parameters** (optional): x, y, width, height.
   - **Scale Parameters** (optional): width, height, keep aspect ratio.

4. **Start Conversion**: Click the :guilabel:`Start Convert` button in the lower right corner.
5. **View Output**: Converted files in the output directory:

   - ``videoname.mjpeg`` (or ``.avi``, ``.h264``): Converted video file.
   - ``app_romfs.bin``, ``app_romfs.c``, and ``ui_resource.h``: Package files.


Performance Tips:

- MJPEG format has the simplest decoding, suitable for low-end devices.
- H264 has the highest compression ratio but requires hardware decoder support.
- Reducing frame rate and quality can significantly reduce file size.
- Cropping and scaling require FFmpeg support.

Font Conversion
----------------

Supported Formats
^^^^^^^^^^^^^^^^^^

Input Formats:
  - TTF: TrueType Font
  - OTF: OpenType Font

Output Formats:
  - **Bitmap Font**: Pre-rendered bitmap, smaller file size, fast rendering
  - **Vector Font**: Retains vector data, can be scaled arbitrarily, larger file size

Character Set Configuration
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

Supports multiple character set sources (union):

1. **Unicode Range**: ``0x20-0x7E`` (ASCII)
2. **Code Page**: ``GB2312``, ``GBK``, ``CP932`` (Japanese), ``KSX1001`` (Korean)
3. **Character File**: ``.txt`` file containing required characters
4. **Custom String**: Directly specify characters

Common character sets: ASCII (95 characters), ISO8859-1 (193 characters), GB2312 (7,478 characters), GBK (21,886 characters), CP932 (7,483 characters), KSX1001 (8,187 characters), etc.

Render Mode
^^^^^^^^^^^^^^^^^

Bit depth determines font display effect and file size:

- **1bpp**: Black and white display, smallest file size
- **2bpp**: 4-level grayscale, recommended (best cost-performance)
- **4bpp**: 16-level grayscale, excellent display effect
- **8bpp**: 256-level grayscale, best display effect

Crop Optimization
^^^^^^^^^^^^^^^^^^^

Font conversion tool supports automatic crop optimization:

- **Auto Mode** (recommended): Character count ≤ 2048 uses offset index (no crop), character count > 2048 uses address index (enable crop)
- **Manual Mode**: Can manually enable or disable crop

Usage
^^^^^^^^^^^^^^^^^

After opening the resource conversion tool panel following the steps in :ref:`Quick Start`:

1. **Drag Font**: Drag font files (.ttf or .otf) to the tool panel.
2. **Select Font**: Click to select a font file in the left resource list.
3. **Configure Parameters**: Set font conversion parameters in the right properties panel.

   - **Font Size**: e.g., 24.
   - **Render Mode**: 1bpp, 2bpp, 4bpp, 8bpp.
   - **Output Format**: Bitmap font, vector font.
   - **Character Set**: Multiple selection (ASCII, GB2312, GBK, CP932, KSX1001, etc.).
   - **Crop Mode**: Auto, enable, disable.

4. **Start Conversion**: Click the :guilabel:`Start Convert` button in the lower right corner.
5. **View Output**: Converted files in the output directory:

   - ``font.bin``: Converted font binary file.
   - ``app_romfs.bin``, ``app_romfs.c``, and ``ui_resource.h``: Package files.

File Size Estimation
^^^^^^^^^^^^^^^^^^^^^^

Font file size is affected by character set, font size, bit depth, and other factors. For detailed file size reference data, comparison analysis of different configurations, and selection recommendations, please refer to :ref:`Font Size Evaluation`, including:

- Common character set size comparison (ASCII, GB2312, GBK, CP932, KSX1001, etc.)
- Impact of bit depth on file size (1bpp, 2bpp, 4bpp, 8bpp)
- Impact of font size on file size (16pt, 24pt, 32pt, 48pt)
- Vector font size reference
- Configuration recommendations for different application scenarios


Glass Effect Conversion
------------------------

Glass effect is a visual effect that blurs the background, commonly used to achieve frosted glass, matte glass, and other UI effects.

Supported Formats
^^^^^^^^^^^^^^^^^

Input Format:
  - SVG files (must use ``.glass`` extension)

Output Format:
  - Blur effect bin file

Conversion Parameters
^^^^^^^^^^^^^^^^^^^^^^

Glass effect supports the following parameter configuration:

- **Effect Area** (Blur Radius): 0-100%, controls the range of the blur effect
- **Effect Intensity** (Blur Intensity): 0-100%, controls the strength of the blur

Usage
^^^^^^^^^^^^^^^^^

After opening the resource conversion tool panel following the steps in :ref:`Quick Start`:

1. **Prepare SVG File**: Create an SVG file and change the extension to ``.glass``.
2. **Drag File**: Drag the ``.glass`` file to the resource list area of the tool panel.
3. **Select File**: Click to select the glass effect file in the left resource list.
4. **Configure Parameters**: Set glass effect parameters in the right properties panel.

   - **Effect Area**: 0-100% (default 50%)
   - **Effect Intensity**: 0-100% (default 50%)

5. **Preview Effect**: You can preview the blur result of the glass effect in real-time.
6. **Start Conversion**: Click the :guilabel:`Start Convert` button in the lower right corner.
7. **View Output**: Converted files in the output directory:

   - ``effectname.bin``: Converted glass effect binary file.
   - ``app_romfs.bin``, ``app_romfs.c``, and ``ui_resource.h``: Package files.


Resource Directory Structure
------------------------------

After successful resource conversion, the plugin will automatically create the following structure:

.. code-block:: text

    Output Directory/
    ├── origin/                    # Original resources and configuration
    │   ├── conversion.json        # Conversion configuration file
    │   └── [resource files]       # Original resources dragged in
    ├── [resourcename].bin         # Converted resource binary files
    ├── app_romfs.bin              # Packaged romfs file system
    ├── app_romfs.c                # C array form of romfs
    └── ui_resource.h              # Resource address offset macro definitions

**Output File Description**:

- ``Resource bin files``: Binary files after each resource conversion, can be used separately.
- ``app_romfs.bin``: File system image packaged from all resources, used for flashing to device.
- ``app_romfs.c``: C language array of romfs, can be compiled directly into firmware.
- ``ui_resource.h``: Contains resource address offset macro definitions, supports direct memory address access.

.. note::
   The ``conversion.json`` file records the conversion configuration of all resources, including format, compression method, parameters, etc. This file is automatically managed and usually does not need to be manually edited.
