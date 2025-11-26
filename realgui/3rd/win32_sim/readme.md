# 3rd Party Libraries Test Environment

## Configuration

Before building, configure which libraries to enable:

```bash
menuconfig Kconfig
```

Configuration will be saved to `.config` file.

## Build

### For SCons
```bash
scons
```

### For VSCode
Please refer to online documentation

### For CMake
```bash
mkdir tmp
cd tmp
cmake ../ -G "MinGW Makefiles"
```

## Configuration Options

- **ThorVG**: Vector graphics library
- **GIFDEC**: GIF decoder
- **Lite3D**: 3D rendering library (default: enabled)
- **ARM2D**: Hardware acceleration (default: enabled)
- **3D Demos**: Butterfly, Dog, Face, Disc, Prism, AppList
