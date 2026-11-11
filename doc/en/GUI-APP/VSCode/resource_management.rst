Resource Management
===================

HoneyDesigner manages project assets in the left **Assets** tab and converts source assets into target-engine formats before a build or simulation. Conversion uses the built-in JavaScript/TypeScript tools and does not require Python. See :ref:`VSCode Plugin Resource Conversion Tool <VSCode Plugin Resource Conversion>` for details.

Asset Types
-----------

.. list-table::
   :header-rows: 1
   :widths: 22 28 50

   * - Type
     - Extensions
     - Description
   * - Images
     - ``png``, ``jpg``, ``jpeg``, ``gif``, ``bmp``, ``webp``
     - Converted to RGB565, RGB888, ARGB, and related formats
   * - SVG
     - ``svg``
     - Vector graphics
   * - Video
     - ``mp4``, ``avi``, ``mov``, ``mkv``, ``webm``
     - Converted to MJPEG, AVI, H264, and related formats
   * - 3D models
     - ``gltf``, ``glb``, ``obj``
     - Used by the HoneyGUI 3D control
   * - Fonts
     - ``ttf``, ``otf``, ``woff``, ``woff2``
     - Converted to bitmap fonts or processed for the target engine
   * - Glass
     - ``glass``
     - Glass/blur effect parameters
   * - Lottie
     - ``json``, ``lottie``
     - Lottie animations
   * - TRMap
     - ``trmap``
     - Texture-map resources

Dependencies
------------

* Image, font, SVG, 3D model, and Glass conversion use TypeScript/JavaScript tools bundled with the extension.
* Lottie and TRMap assets are processed by the extension during the build; some are copied directly.
* Video conversion requires FFmpeg. Video assets are skipped when FFmpeg is unavailable.
* romfs packaging still uses a Python script.

.. _romfs-packaging-en:

ROMFS Packaging
---------------

HoneyGUI projects package resources into romfs automatically during the build; you do not need to run a packaging script manually. The flow is:

1. Collect assets referenced by HML or listed in ``alwaysConvert``.
2. Convert them into target-format ``.bin`` files under ``build/assets/``.
3. Use ``romfsBaseAddr`` from ``project.json`` as the base address and package ``build/app_romfs_<baseAddr>.bin``.
4. Generate a resource-address header such as ``ui_resource.h`` for direct address access from firmware.

When you run conversion from the resource tool panel, the output directory is the one selected in the panel. Project builds always write ``build/app_romfs_<baseAddr>.bin``.

Image Formats
-------------

Supported output formats:

.. list-table::
   :header-rows: 1
   :widths: 22 18 60

   * - Format
     - Bytes per pixel
     - Use case
   * - ``RGB565``
     - 2
     - Opaque icons and backgrounds; smallest size
   * - ``RGB888``
     - 3
     - High-quality opaque images
   * - ``ARGB8565``
     - 3
     - Transparency with moderate size
   * - ``ARGB8888``
     - 4
     - Full transparency and highest quality
   * - ``A8``
     - 1
     - 8-bit alpha or grayscale mask
   * - ``A4`` / ``A2`` / ``A1``
     - 0.5 / 0.25 / 0.125
     - Low-bit alpha masks
   * - ``I8``
     - 1
     - 256-color indexed image

Compression methods:

* ``none``: no compression.
* ``rle``: run-length encoding.
* ``fastlz``: FastLZ lossless compression.
* ``yuv``: lossy YUV compression with sampling, blur bits, and optional FastLZ.
* ``jpeg``: lossy JPEG compression with sampling, quality, and background color.
* ``adaptive``: compares FastLZ, RLE, and no compression, then picks the smallest result.

Conversion settings can be applied to files or folders and inherited from a parent folder.

Video Formats
-------------

Supported output formats:

* ``MJPEG``
* ``AVI``
* ``H264``
* ``MSV1``
* ``Cinepak``

Configurable options:

* Quality
* Frame rate
* Scaling by pixels or percentage
* Cropping (width, height, position)
* Crop/scale order
* Whether GIF files are treated as video

Video conversion requires FFmpeg. Converted output is packaged into the build directory.

Fonts
-----

* Place ``ttf``, ``otf``, ``woff``, or ``woff2`` files under ``assets/``.
* Reference them in ``hg_label`` with ``fontFile`` and a leading ``/``, for example ``/NotoSansSC-Medium.ttf``.
* Character-set conversion keeps only the characters used by the project and reduces firmware size.
* Fonts must be inside ``assets/`` so the device can load them.

3D Models
---------

* Supports OBJ, GLTF, and GLB.
* Model textures are converted together with the model.
* ``hg_3d`` references the model with ``modelPath``, for example ``assets/models/cube.bin``.
* Keep vertex and face counts within the target device's performance budget.

Glass, Lottie, and TRMap
------------------------

* ``hg_glass`` uses ``.glass`` parameter files to generate glass/blur effects.
* ``hg_lottie`` uses JSON or ``.lottie`` animation files.
* ``trmap`` files provide texture-map resources.

Workflow
--------

1. Put assets under the project ``assets/`` directory.
2. Open the **Assets** tab.
3. Select an image, video, model, or font and configure its target format and compression.
4. Drag images onto the canvas or enter paths manually in component properties.
5. Run code generation or a build; the extension converts assets automatically.

Resource Paths
--------------

.. code-block:: xml

   <!-- Images start with assets/ -->
   <hg_image id="img_logo" x="20" y="20" width="120" height="120"
             src="assets/images/logo.png" />

   <!-- Fonts start with / and are relative to assets/ -->
   <hg_label id="lbl_title" x="20" y="160" width="240" height="40"
             text="Hello" fontFile="/NotoSansSC-Medium.ttf" fontSize="24" />

   <!-- 3D model -->
   <hg_3d id="model_cube" x="40" y="40" width="320" height="320"
          modelPath="assets/models/cube.bin" />

Do not use absolute host paths such as ``/home/user/project/assets/logo.png``.

Conversion Configuration
------------------------

Resource conversion settings are stored in the project. Key capabilities:

* Folder-level settings with inheritance.
* Smart packing: convert only assets referenced by HML plus entries in ``alwaysConvert``.
* Full packing: convert every asset under ``assets/``.
* Convert a single asset manually or as part of the build.

LVGL Resource Deployment
------------------------

LVGL projects can use:

* ``c-array``: resources are compiled into C arrays and linked into firmware.
* ``external-bin``: resources are packaged into romfs and loaded at runtime.

HoneyGUI projects generate romfs resources during the build flow.

Best Practices
--------------

* Use appropriate image dimensions and the smallest acceptable format.
* Prefer ``ARGB8565`` or ``ARGB8888`` when transparency is required.
* Prefer ``RGB565`` when transparency is not required.
* Keep only the required character set in fonts.
* Choose reasonable resolution, frame rate, and quality for videos.
* Keep source files in version control; build output belongs under ``build/``.
* Use clear file names such as ``logo.png`` and ``btn_normal.png``.

Troubleshooting
---------------

**Image conversion fails**

* Check that the file is not corrupted.
* Verify that the extension matches the actual format.
* Check output-directory permissions.

**Videos are not converted**

* Install FFmpeg and add it to ``PATH``.
* Check whether the video codec is supported.

**Fonts render incorrectly**

* Verify that the font is under ``assets/``.
* Verify that ``fontFile`` starts with ``/``.
* Verify that the character set contains the displayed text.

**Model loading fails**

* Check that the model and texture files are complete.
* Verify that ``modelPath`` points to the converted resource.

Next Steps
----------

* See :doc:`build_simulation` to understand the complete build process
* Refer to :doc:`deployment` for resource deployment on embedded devices
