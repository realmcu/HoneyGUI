Embedded Deployment
===================

This chapter explains how to integrate a HoneyDesigner project into firmware and how HoneyGUI and LVGL deployments differ.

Overview
--------

The current extension converts HML into **C code** and produces firmware-ready resources through conversion and romfs packaging. It does not provide a runtime HML (XML) parser or an HML binary descriptor export.

.. list-table::
   :header-rows: 1
   :widths: 20 30 50

   * - Target engine
     - Main output
     - Deployment
   * - HoneyGUI
     - C code, Entry.c, SConscript, romfs
     - Integrate into firmware or download romfs over UART
   * - LVGL
     - C code, resource descriptors, romfs
     - c-array or external-bin

HoneyGUI Deployment
-------------------

Generated Output
~~~~~~~~~~~~~~~~

.. code-block:: text

   src/
   ├── MyProjectEntry.c
   ├── SConscript
   ├── ui/
   ├── callbacks/
   └── user/

   build/
   ├── assets/
   ├── app_romfs_<baseAddr>.bin
   ├── app_romfs.c
   └── ui_resource.h

Key files:

* ``src/{ProjectName}Entry.c``: project entry with protected regions.
* ``src/ui``: UI creation code, overwritten on every generation.
* ``src/callbacks``: event callbacks with protected regions.
* ``src/user``: user code, generated once.
* ``build/app_romfs_<baseAddr>.bin``: downloadable resource image.

Integrating into Firmware
~~~~~~~~~~~~~~~~~~~~~~~~~~

1. Generate code in the extension.
2. Add ``src/ui``, ``src/callbacks``, ``src/user``, and ``src/{ProjectName}Entry.c`` to the firmware project.
3. Merge ``src/SConscript`` or its source list into the firmware build.
4. Add the required include paths.
5. Include ``{ProjectName}Entry.c`` in the build; its ``GUI_INIT_APP_EXPORT(app_init)`` registers the startup initialization.

.. note::
   Firmware projects may use SCons, Keil, IAR, or another build system. The generated ``src/SConscript`` can be used directly by SCons projects. For other projects, add the source files and include paths using the existing build configuration.

Resource Packaging
~~~~~~~~~~~~~~~~~~

During a HoneyGUI build, the extension:

1. Scans assets referenced by HML.
2. Converts source assets into target formats.
3. Writes ``build/assets/``.
4. Packages ``build/app_romfs_<baseAddr>.bin``.

``<baseAddr>`` comes from ``romfsBaseAddr`` in ``project.json``, for example ``0x704D1000``.

See :ref:`ROMFS packaging <romfs-packaging-en>` for the full packaging flow and output files.

UART Download
~~~~~~~~~~~~~

1. Run a full build so that ``build/app_romfs_<baseAddr>.bin`` exists.
2. Click :guilabel:`Download` in the toolbar, or use :guilabel:`Download` in the **Quick** view.
3. Select the serial port, baud rate, chip type, and flash address.
4. Restart the board after the download completes.

UART settings can be stored in ``project.json``:

.. code-block:: json

   {
     "romfsBaseAddr": "0x704D1000",
     "uart": {
       "port": "COM3",
       "baudRate": 115200,
       "chipType": "RTL87X3E"
     }
   }

Supported chip types follow the current extension configuration. Common values include ``RTL87X3E``, ``RTL87X3EP``, ``RTL87X3D``, and ``RTL87X3G``.

LVGL Deployment
---------------

Generated Output
~~~~~~~~~~~~~~~~

.. code-block:: text

   src/lvgl/
   ├── {name}_lvgl_ui.c/h
   ├── {name}_lvgl_callbacks.c/h
   ├── lvgl_generated_ui.c/h
   ├── ui_resource.h
   └── lv_img_dsc_list.c/h

Resource Deployment Modes
~~~~~~~~~~~~~~~~~~~~~~~~~

Choose a deployment mode for images or folders in the Assets panel:

* ``c-array``: resources are compiled into C arrays and linked into firmware. Suitable for smaller assets.
* ``external-bin``: resources are packaged into romfs and accessed through ``ui_resource.h`` and ``lv_img_dsc_list.c/h`` at runtime. Suitable for larger asset sets or separately updatable resources.

Integration steps:

1. Set ``"targetEngine": "lvgl"`` in ``project.json``.
2. Generate code.
3. Add ``src/lvgl`` to the firmware project.
4. For ``external-bin``, include the generated romfs and resource descriptors.
5. Call ``lvgl_generated_ui_create()`` during firmware startup.

Resource Paths
--------------

* Images: ``assets/...``
* Fonts: ``/...`` relative to ``assets/``
* 3D models: relative to ``assets/``
* Do not use absolute host paths in HML.

FAQ
---

**Can I export HML XML or HML BIN for device-side parsing?**

The current extension does not provide either export path. It generates C code and resource images.

**Do UI changes require rebuilding firmware?**

Yes for the C-code workflow. The resource image can be updated separately over UART, but UI structure code remains in firmware.

**How do I switch between HoneyGUI and LVGL?**

Change ``targetEngine`` in ``project.json``, clean the old output directory, and regenerate code.

**Is OTA update secure?**

Implement signature and version verification in firmware. The extension does not provide a device-side OTA security policy.

**Why does download report that romfs is missing?**

Run a full build, verify that ``build/app_romfs_<baseAddr>.bin`` exists, and confirm that ``romfsBaseAddr`` matches.

Recommendations
---------------

* Use PC simulation for rapid iteration.
* Before release, verify that every ``callFunction`` callback is implemented in ``custom_functions`` or ``src/user``.
* Use compressed formats and appropriate dimensions for large assets.
* Keep generated entry files and protected regions when integrating into firmware.

