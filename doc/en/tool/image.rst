.. _Image Convert Tool:

==================
Image Convert Tool
==================

Image Format Conversion
-------------------------
Convert pictures in various formats into RGB raw pictures

* Open the converter. Please refer to this section for the download link of the image conversion tool: :ref:`Tool`.
* The operation steps and detailed instructions are as follows:

.. figure:: https://foruda.gitee.com/images/1725345166232084759/b1982ac0_13671125.png
   :align: center
   :width: 600px

   Image Conversion Tool Interface

1. Open the image folder.
2. Open the settings.
3. Select the output folder.
4. Check the color information header.
5. Choose the image format to be configured.
6. Set the conversion parameters.
7. Convert.

Configuration
^^^^^^^^^^^^^^^^^
- Color Head : BeeGUI requires this header information for rendering images.
- Big-Endian : Whether the input image uses big-endian format.
- Compress : Enable image compression.
- MixAlphaChannel Flag : Whether to mix alpha channel to RGB when converting RGBA to RGB or RGB565.
- Scan Mode : Select whether the scan direction is horizontal or vertical. BeeGUI only requires horizontal.
- Color Space : Select color space (RGB565, RGBA, BINARY...). BeeGUI can display all of them.

Color Space
^^^^^^^^^^^^^^^^^
- RGB565: Colorful but with low rendering cost and storage. 2 bytes per pixel.

+-----+-------+------+
| Red | Green | Blue |
+=====+=======+======+
| 5bit| 6bit  | 5bit |
+-----+-------+------+

- ARGB8565: 24-bit ARGB mode.

+---------+------+-------+------+
| Opacity | Red  | Green | Blue |
+=========+======+=======+======+
| 8bit    | 5bit | 6bit  | 5bit |
+---------+------+-------+------+

- RTKRGAB: 16-bit RGAB mode.

+------+-------+---------+------+
| Red  | Green | Opacity | Blue |
+======+=======+=========+======+
| 5bit | 5bit  | 1bit    | 5bit |
+------+-------+---------+------+

- RGB : 24-bit RGB mode. 3 bytes per pixel.

+------+-------+------+
| Red  | Green | Blue |
+======+=======+======+
| 8bit | 8bit  | 8bit |
+------+-------+------+

- ARGB : True color with opacity. Enhance the display quality with transparency effects. 4 bytes per pixel.

+---------+------+-------+------+
| Opacity | Red  | Green | Blue |
+=========+======+=======+======+
| 8bit    | 8bit | 8bit  | 8bit |
+---------+------+-------+------+

- BINARY : Use one bit for a pixel.

- RTKARGB8565 : RTK 24-bit ARGB8565 mode.

+---------+---------+-----+-----+-------+------+-----+-------+------+-----+
| Opacity | Opacity | ... | Red | Green | Blue | Red | Green | Blue | ... |
+=========+=========+=====+=====+=======+======+=====+=======+======+=====+
|  8bit   |  8bit   | ... | 5bit| 6bit  | 5bit | 5bit|  6bit | 5bit | ... |
+---------+---------+-----+-----+-------+------+-----+-------+------+-----+

Output Files
-----------------
The following files will be generated.

.. figure:: https://foruda.gitee.com/images/1718776957325643948/803a9ab3_13408154.png
   :align: center
   :width: 300px

   Binary File Generation

By using the image conversion tool, we can convert the three ``JPG files a, b, and c`` into three ``binary files a, b, and c``.

Place the ``binary file`` into the ``root`` folder of the packaging directory. For the packaging process, please refer to the :ref:`Pack Tool` section.

.. figure:: https://foruda.gitee.com/images/1718786296198222012/e213af1a_13408154.png
   :align: center
   :width: 200px

   Root Folder

