# HoneyGUI 3D Model Tool

Convert OBJ and GLTF 3D models with textures to HoneyGUI binary format.

## Usage

### Step 1: Prepare Files

Copy your model files to the `3D-tool` directory:

```
tool/3D-tool/
├── extract_desc.exe          (tool)
├── extract_desc_v2.py        (tool)
├── your_model.obj            ← Your model
├── your_model.mtl            ← Your model
├── texture1.png              ← Your textures
└── texture2.png              ← Your textures
```

### Step 2: Run the Tool

Open command prompt in the `3D-tool` directory and run:

**Windows:**
```cmd
extract_desc_v3.exe your_model.obj
```

**Or use Python version:**
```cmd
python extract_desc_v3.py your_model.obj
```

### Step 3: Choose Texture Conversion

The tool will ask if you want to convert textures:

```
========================================
HoneyGUI 3D Model Descriptor Generator
========================================

Do you want to convert PNG textures to bin format? [Y/n]: 
```

- Press **Enter** or type **Y** - Auto-convert all PNG files to .bin
- Type **N** - Skip conversion (if already converted or no textures)

### Output

After processing, you'll get:

```
tool/3D-tool/
├── your_model.obj
├── your_model.mtl
├── texture1.png
├── texture1.bin              ← Generated
├── texture2.png
├── texture2.bin              ← Generated
├── desc_your_model.bin       ← Use this in your app
└── desc_your_model.txt       ← C array version
```


## Supported Formats

- **Models:** OBJ, GLTF
- **Textures:** PNG (auto-converted to optimal format)
- **Output:** Binary descriptor + texture binaries

## Notes

- All files must be in the `3D-tool` directory
- Textures are auto-detected as ARGB8888 or RGB565
- The tool uses `../image-convert-tool/image_converter_new.py` for conversion
- Binary format is compatible with existing HoneyGUI applications
