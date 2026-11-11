Using the VSCode Extension to Develop Applications
===================================================

HoneyDesigner is a VSCode extension for embedded :term:`GUI` application development. It combines visual design, HML editing, C code generation, resource conversion, and local simulation.

Key Features
-------------

* **Dual-engine code generation**: HoneyGUI and LVGL, selected by ``targetEngine`` in ``project.json``.
* **Visual design**: drag-and-drop canvas, component tree, property/event panels, alignment and distribution tools, and a view navigation graph.
* **HML format**: XML-based, version-control friendly, and editable in either the designer or a text editor.
* **Code protection**: UI code is regenerated on every build; callback files preserve protected regions; files under ``src/user`` are generated once.
* **Resource conversion**: images, fonts, videos, 3D models, SVG, Lottie, Glass, and TRMap assets can be configured in the Assets panel.
* **Build and simulation**: integrated build flows for HoneyGUI (SCons) and LVGL (CMake).
* **Templates and AI collaboration**: empty projects and online templates; optional ``.claude/skills`` and ``AGENTS.md`` distribution.
* **UART download**: flash a generated romfs image to supported boards.

.. note::

   * Windows and Linux/WSL are the primary supported platforms. macOS is not fully validated.
   * Design, code generation, build, and simulation run locally. Creating a project from a template requires ``git`` and network access.
   * The first build requires Python 3.9.7, SCons 4.4.0, and GCC/MinGW. Linux also requires SDL2. Video conversion requires FFmpeg.

Development Workflow
---------------------

Typical workflow for developing HoneyGUI applications with VSCode plugin:

1. Create or open a project.
2. Design the interface and configure events in the HML designer.
3. Generate C code for HoneyGUI or LVGL.
4. Build and simulate on the PC.
5. Integrate into firmware or download the romfs image over UART.

.. toctree::
    :maxdepth: 2

    Quick Start <quick_start>
    Designer Usage <designer>
    HML File Format <hml_format>
    Event System <event_system>
    Code Generation <code_generation>
    Resource Management <resource_management>
    Build and Simulation <build_simulation>
    Embedded Deployment <deployment>
