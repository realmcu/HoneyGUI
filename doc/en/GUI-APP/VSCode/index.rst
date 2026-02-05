Using VSCode Plugin to Develop Applications
============================================

HoneyGUI Visual Designer is a powerful VSCode extension that provides a visual design environment, code generation, and real-time simulation tools for embedded GUI application development.

Key Features
-------------

* **Fully Offline Development**: No network dependency, all features run locally
* **Visual Design**: Drag-and-drop design interface with WYSIWYG experience
* **HML File Format**: XML-based interface description language, version control friendly
* **Code Protection Mechanism**: User code is never overwritten, supports multiple redesigns
* **One-Stop Workflow**: Integrated design, coding, compilation, and simulation workflow
* **Cross-Platform Support**: Full support for Windows/Linux/macOS

Development Workflow
---------------------

Typical workflow for developing HoneyGUI applications with VSCode plugin:

1. **Create Project** - Using templates or from scratch
2. **Visual Interface Design** - Drag components, configure properties
3. **Generate C Code** - Automatically scan all HML files
4. **Build and Simulate** - SCons + GCC + SDL2
5. **Deploy to Embedded Devices** - Choose appropriate deployment strategy

.. toctree::
    :maxdepth: 2

    Quick Start <quick_start>
    Designer Usage <designer>
    HML File Format <hml_format>
    Code Generation <code_generation>
    Event System <event_system>
    Resource Management <resource_management>
    Build and Simulation <build_simulation>
    Embedded Deployment <deployment>
