Resource Management
===================

HoneyGUI Design provides built-in resource conversion tools for converting images, 3D models, and other resources into binary formats supported by HoneyGUI. These tools are implemented in pure JavaScript/TypeScript, eliminating the need to install Python.

Overview
--------

The ``tools/`` directory contains pure JavaScript/TypeScript implementations of HoneyGUI conversion tools, replacing the Python tools in the SDK.

Why JS Version?
---------------

Problems
~~~~~~~~

* Users need to install Python environment
* Need to install numpy, PIL, pygltflib and other dependencies
* Complex configuration across different systems
* Performance overhead from Python interpreter startup

Solutions
~~~~~~~~~

* ✅ Pure JS/TS implementation, no Python required
* ✅ Self-contained plugin, works out of the box
* ✅ 25-40% performance improvement
* ✅ Unified tech stack, easier to maintain

Feature Comparison
------------------

.. list-table::
   :header-rows: 1
   :widths: 30 15 15 40

   * - Feature
     - Python Version
     - JS Version
     - Status
   * - PNG → bin
     - ✅
     - ✅
     - Complete
   * - JPEG → bin
     - ✅
     - ✅
     - Complete
   * - RGB565
     - ✅
     - ✅
     - Complete
   * - RGB888
     - ✅
     - ✅
     - Complete
   * - ARGB8888
     - ✅
     - ✅
     - Complete
   * - ARGB8565
     - ✅
     - ✅
     - Complete
   * - A8
     - ✅
     - ✅
     - Complete
   * - OBJ Parsing
     - ✅
     - ✅
     - Complete
   * - MTL Material
     - ✅
     - ✅
     - Complete
   * - RLE Compression
     - ✅
     - ⏳
     - Planned
   * - FastLZ Compression
     - ✅
     - ⏳
     - Planned
   * - YUV Compression
     - ✅
     - ⏳
     - Planned
   * - GLTF Parsing
     - ✅
     - ⏳
     - Planned

Tool Directory Structure
------------------------

.. code-block:: text

   tools/
   ├── image-converter/       # Image conversion module
   │   ├── types.ts          # Format definitions
   │   ├── headers.ts        # Binary headers
   │   ├── pixel-converter.ts # Pixel conversion
   │   └── converter.ts      # Main converter
   ├── model-converter/       # 3D model conversion module
   │   ├── types.ts          # Model types
   │   ├── obj-parser.ts     # OBJ parser
   │   └── obj-converter.ts  # OBJ converter
   ├── tests/                 # Test cases
   ├── examples/              # Usage examples
   ├── README.md             # Project documentation
   ├── QUICKSTART.md         # Quick start guide
   ├── INTEGRATION.md        # Integration guide
   └── COMPLETION_REPORT.md  # Completion report

Image Resource Management
-------------------------

Supported Image Formats
~~~~~~~~~~~~~~~~~~~~~~~

**Input Formats**:

* PNG (Recommended)
* JPEG/JPG

**Output Formats**:

* RGB565 - 16-bit color, no transparency
* RGB888 - 24-bit true color, no transparency
* ARGB8888 - 32-bit true color, full transparency
* ARGB8565 - 24-bit mixed format (RGB565 + 8-bit Alpha)
* A8 - 8-bit grayscale/transparency

Image Conversion Workflow
~~~~~~~~~~~~~~~~~~~~~~~~~~

1. Place image files (PNG/JPEG) in ``assets/images/`` directory
2. Automatically convert to ``.bin`` format during project build
3. Reference in HML: ``src="assets/images/logo.bin"``

Manual Image Conversion
~~~~~~~~~~~~~~~~~~~~~~~

Using in plugin:

.. code-block:: typescript

   import { ImageConverter, PixelFormat } from './tools';

   // Create converter
   const imageConverter = new ImageConverter();

   // Convert image
   await imageConverter.convert(
       'assets/icon.png',      // Input file
       'build/assets/icon.bin', // Output file
       'auto'                   // Auto format selection
   );

   // Convert with specific format
   await imageConverter.convert(
       'assets/logo.png',
       'build/assets/logo.bin',
       'ARGB8888'              // Use 32-bit format
   );

Pixel Format Selection Guide
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

.. list-table::
   :header-rows: 1
   :widths: 20 20 60

   * - Format
     - File Size
     - Use Cases
   * - RGB565
     - Smallest (2 bytes/pixel)
     - Icons, backgrounds without transparency
   * - RGB888
     - Medium (3 bytes/pixel)
     - High-quality images, no transparency needed
   * - ARGB8888
     - Largest (4 bytes/pixel)
     - Icons, UI elements requiring transparency
   * - ARGB8565
     - Medium (3 bytes/pixel)
     - Balance between transparency and file size
   * - A8
     - Smallest (1 byte/pixel)
     - Monochrome icons, masks

.. tip::
   Use ``'auto'`` mode to let the converter automatically select the optimal format:

   * Has alpha channel → ARGB8888
   * No alpha channel → RGB565

3D Model Resource Management
-----------------------------

Supported Model Formats
~~~~~~~~~~~~~~~~~~~~~~~

**Currently Supported**:

* OBJ (Wavefront Object) - ✅ Implemented
* MTL (Material Template Library) - ✅ Implemented

**Planned Support**:

* GLTF/GLB - ⏳ In development

OBJ Model Conversion
~~~~~~~~~~~~~~~~~~~~

OBJ conversion workflow:

1. Place ``.obj`` and ``.mtl`` files in ``assets/models/`` directory
2. Automatically convert to HoneyGUI binary format during build
3. Generate ``desc_{modelName}.bin`` and ``desc_{modelName}.txt``

Manual OBJ conversion:

.. code-block:: typescript

   import { OBJConverter } from './tools';

   // Create converter
   const objConverter = new OBJConverter();

   // Convert model
   objConverter.convert(
       'assets/model.obj',              // Input OBJ file
       'build/assets/desc_model.bin',   // Output binary file
       'build/assets/desc_model.txt'    // Output text description
   );

Model Optimization Recommendations
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

* **Vertex Count**: Keep under 10,000 (embedded device limitation)
* **Face Count**: Use triangles, avoid complex polygons
* **Materials**: Use simple materials, avoid complex shaders
* **Textures**: Compress texture images, use appropriate resolution

Font Resource Management
------------------------

Font Formats
~~~~~~~~~~~~

HoneyGUI supports the following font formats:

* TTF (TrueType Font)
* BIN (Pre-converted binary font)

Font Conversion
~~~~~~~~~~~~~~~

Use HoneyGUI SDK font tools for conversion:

.. code-block:: bash

   # Using SDK tools
   cd /path/to/HoneyGUI/tool/font
   python font_convert.py input.ttf output.bin --size 16

.. note::
   Font conversion currently uses Python tools provided by the SDK. The VSCode plugin will integrate a JS version of the font converter in a future release.

Using fonts in HML:

.. code-block:: xml

   <hg_text id="title" x="10" y="10" w="200" h="30"
            text="Hello"
            fontFamily="assets/fonts/arial_16.bin"
            fontSize="16"/>

Resource Packaging
------------------

ROM File System
~~~~~~~~~~~~~~~

HoneyGUI uses ROM file system to manage resources:

1. Place all resources in ``assets/`` directory
2. Use ``mkromfs`` tool to package during build
3. Generate ``root.bin`` file containing all resources
4. Embedded devices load from Flash

Packaging workflow:

.. code-block:: bash

   # Automatic packaging (during build process)
   cd win32_sim
   scons

   # Manual packaging
   cd /path/to/HoneyGUI/tool/mkromfs
   ./mkromfs assets/ root.bin

Resource Path Conventions
~~~~~~~~~~~~~~~~~~~~~~~~~~

Use relative paths when referencing resources in HML:

.. code-block:: xml

   <!-- Correct -->
   <hg_image src="assets/images/logo.bin"/>
   <hg_text fontFamily="assets/fonts/arial.bin"/>
   <hg_3d modelSrc="assets/models/cube.bin"/>

   <!-- Incorrect - Don't use absolute paths -->
   <hg_image src="/home/user/project/assets/images/logo.bin"/>

Performance Data
----------------

JS/TS tools vs Python tools performance comparison:

.. list-table::
   :header-rows: 1
   :widths: 40 20 20 20

   * - Operation
     - Python
     - JS/TS
     - Improvement
   * - 64x64 PNG → RGB565
     - 50ms
     - 30ms
     - 40%
   * - OBJ Parsing (1000 vertices)
     - 80ms
     - 60ms
     - 25%
   * - Startup Time
     - 200ms
     - 5ms
     - 97%

Usage Recommendations
---------------------

Resource Optimization
~~~~~~~~~~~~~~~~~~~~~

1. **Image Optimization**:

   * Use appropriate resolution (avoid oversized images)
   * Choose correct pixel format
   * Consider using compression (coming soon)

2. **Model Optimization**:

   * Simplify geometry (reduce vertices and faces)
   * Use low-resolution textures
   * Remove unnecessary details

3. **Font Optimization**:

   * Only include needed character sets
   * Use appropriate font sizes
   * Consider using bitmap fonts instead of vector fonts

Resource Management Best Practices
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

* **Directory Organization**: Classify resources by type (images/, models/, fonts/)
* **Naming Conventions**: Use clear file names (logo.png, button_bg.png)
* **Version Control**: Add source files (PNG, OBJ) to Git, add generated .bin files to .gitignore
* **Resource Manifest**: Maintain resource list documentation recording purpose and format

Troubleshooting
---------------

Common Issues
~~~~~~~~~~~~~

**Issue 1: Image conversion failed**

.. code-block:: text

   Error: Failed to convert image

Solutions:

* Check if image file is corrupted
* Confirm file format (only PNG/JPEG supported)
* Check file permissions

**Issue 2: Model loading failed**

.. code-block:: text

   Error: Failed to parse OBJ file

Solutions:

* Check OBJ file syntax
* Confirm MTL file path is correct
* Validate vertex data integrity

**Issue 3: Font display issues**

Solutions:

* Confirm font file format is correct
* Check font file path
* Validate character set includes needed characters

Debugging Tips
~~~~~~~~~~~~~~

1. **View conversion logs**: Check console output during build
2. **Verify binary files**: Use hex editor to check file headers
3. **Test small files**: First test conversion workflow with small files
4. **Compare output**: Verify correctness by comparing with Python tool output

Next Steps
----------

* Learn :doc:`code_generation` to understand how to use resources in code
* See :doc:`build_simulation` to understand the complete build process
* Refer to :doc:`deployment` for resource deployment on embedded devices
* Read HoneyGUI SDK tool documentation for more resource format details
