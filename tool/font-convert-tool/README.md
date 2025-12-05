# Font Converter - Python Edition

A Python-based font conversion tool that generates bitmap font files from TrueType fonts, compatible with embedded systems and wearable devices.

## Features

- **Multiple Render Modes**: Support for 1-bit, 2-bit, 4-bit, and 8-bit bitmap fonts
- **Character Set Customization**: Configure character sets via CodePage files, mapping files, symbol files, or custom Unicode ranges
- **Font Styling**: Bold and italic effects
- **Image Processing**: Rotation (0°, 90°, 180°, 270°) and gamma correction
- **Optimization**: Character cropping to reduce file size
- **Flexible Indexing**: Address mode or offset mode for character lookup
- **C++ Compatibility**: Generates font files compatible with the C++ fontDictionary tool

## Requirements

- Python 3.7 or higher
- Dependencies (see `requirements.txt`):
  - freetype-py >= 2.3.0
  - numpy >= 1.21.0
  - Pillow >= 9.0.0

## Installation

### Option 1: Install as a Package

```bash
pip install -r requirements.txt
python setup.py install
```

### Option 2: Run Directly

```bash
pip install -r requirements.txt
python -m font_converter <config_file.json>
```

## Quick Start

1. **Prepare a TrueType font file** (e.g., `HarmonyOS.ttf`)

2. **Create a configuration file** (e.g., `config.json`):

```json
{
  "fontFile": "HarmonyOS.ttf",
  "fontSize": 32,
  "renderMode": 2,
  "outputFormat": "bitmap",
  "outputFolder": "output",
  "codepages": ["CP437.txt"],
  "crop": true,
  "indexMethod": 0
}
```

3. **Run the converter**:

```bash
python -m font_converter config.json
```

4. **Output files**:
   - `HarmonyOS_32_r2.font` - Binary font file
   - `HarmonyOS_32_r2.cst` - Character set file
   - `NotSupportedChars.txt` - List of characters not in the font (if any)

## Configuration Reference

### Basic Settings

| Field | Type | Required | Description |
|-------|------|----------|-------------|
| `fontFile` | string | Yes | Path to TrueType font file (.ttf) |
| `fontSize` | integer | Yes | Font size in pixels (e.g., 16, 24, 32) |
| `renderMode` | integer | Yes | Bit depth: 1, 2, 4, or 8 |
| `outputFormat` | string | Yes | Output format: "bitmap" or "vector" |
| `outputFolder` | string | Yes | Output directory path |

### Character Set Configuration

| Field | Type | Required | Description |
|-------|------|----------|-------------|
| `codepages` | array | No | List of CodePage files (e.g., ["CP437.txt", "CP1252.txt"]) |
| `mappings` | array | No | List of mapping files for character substitution |
| `symbols` | array | No | List of symbol files for special characters |
| `customRanges` | array | No | Custom Unicode ranges (e.g., [[0x0020, 0x007E]]) |

### Font Styling

| Field | Type | Default | Description |
|-------|------|---------|-------------|
| `bold` | boolean | false | Apply bold effect |
| `italic` | boolean | false | Apply italic effect |

### Image Processing

| Field | Type | Default | Description |
|-------|------|---------|-------------|
| `rotation` | integer | 0 | Rotation angle: 0, 90, 180, or 270 |
| `gamma` | float | 1.0 | Gamma correction value (0.1 - 3.0) |

### Optimization

| Field | Type | Default | Description |
|-------|------|---------|-------------|
| `crop` | boolean | false | Enable character cropping to reduce file size |
| `indexMethod` | integer | 0 | Index method: 0 (address mode) or 1 (offset mode) |
| `rvd` | boolean | false | Reverse vertical direction mode |

## Examples

### Example 1: Basic Bitmap Font (2-bit)

```json
{
  "fontFile": "Arial.ttf",
  "fontSize": 24,
  "renderMode": 2,
  "outputFormat": "bitmap",
  "outputFolder": "output",
  "codepages": ["CP437.txt"],
  "crop": false,
  "indexMethod": 0
}
```

### Example 2: Bold Italic Font with Rotation

```json
{
  "fontFile": "HarmonyOS.ttf",
  "fontSize": 32,
  "renderMode": 2,
  "outputFormat": "bitmap",
  "outputFolder": "output",
  "codepages": ["CP437.txt"],
  "bold": true,
  "italic": true,
  "rotation": 90,
  "crop": true,
  "indexMethod": 0
}
```

### Example 3: Chinese Characters

```json
{
  "fontFile": "SimHei.ttf",
  "fontSize": 16,
  "renderMode": 4,
  "outputFormat": "bitmap",
  "outputFolder": "output",
  "customRanges": [
    [0x4E00, 0x9FFF]
  ],
  "crop": true,
  "indexMethod": 0
}
```

### Example 4: Cropped Font for Embedded Systems

```json
{
  "fontFile": "Roboto.ttf",
  "fontSize": 16,
  "renderMode": 1,
  "outputFormat": "bitmap",
  "outputFolder": "output",
  "codepages": ["CP437.txt"],
  "crop": true,
  "indexMethod": 0,
  "gamma": 1.2
}
```

## CodePage Files

CodePage files define character sets. Place them in a `CodePage` folder relative to your configuration file.

**Format**: One Unicode value per line in hexadecimal (0xXXXX)

Example `CP437.txt`:
```
0x0020
0x0021
0x0022
...
0x007E
```

## Output File Format

### Binary Font File (.font)

The binary font file contains:
1. **Header**: Font metadata (size, render mode, flags, etc.)
2. **Index Area**: Character lookup table
3. **Data Area**: Bitmap data for each character

### Character Set File (.cst)

A text file listing all Unicode values in the font (one per line in hexadecimal).

### Not Supported Characters File

Lists characters that couldn't be rendered (missing from the font file).

## Render Modes

| Mode | Bits per Pixel | Colors | Use Case |
|------|----------------|--------|----------|
| 1 | 1-bit | 2 (black/white) | Monochrome displays, minimal storage |
| 2 | 2-bit | 4 (grayscale) | Simple anti-aliasing, low storage |
| 4 | 4-bit | 16 (grayscale) | Good anti-aliasing, moderate storage |
| 8 | 8-bit | 256 (grayscale) | Best quality, higher storage |

## Index Methods

### Address Mode (indexMethod: 0)
- Fixed-size lookup table (65536 entries)
- Fast character lookup (O(1))
- Higher memory usage
- Best for: Fonts with many characters or random access patterns

### Offset Mode (indexMethod: 1)
- Variable-size lookup table (N entries, where N = character count)
- Slower character lookup (O(log N) with binary search)
- Lower memory usage
- Best for: Fonts with few characters or sequential access patterns

## Troubleshooting

### Issue: "Font file not found"
**Solution**: Ensure the font file path in the configuration is correct and the file exists.

### Issue: "No characters in character set"
**Solution**: Check that CodePage files exist and contain valid Unicode values, or specify `customRanges`.

### Issue: "Character not in font"
**Solution**: The font file doesn't contain glyphs for some characters. Check `NotSupportedChars.txt` for the list. Use a different font or remove those characters from your character set.

### Issue: "Output file is too large"
**Solution**: 
- Enable `crop` mode to remove empty space
- Use a lower `renderMode` (e.g., 1-bit instead of 8-bit)
- Reduce the character set size

### Issue: "Characters appear too large/small"
**Solution**: Adjust the `fontSize` parameter. The tool automatically scales glyphs to fit within the specified size box.

## Performance Tips

1. **Use crop mode** for embedded systems to minimize file size
2. **Choose appropriate render mode**: 1-bit for simple displays, 2-bit for basic anti-aliasing
3. **Limit character sets**: Only include characters you need
4. **Use address mode** for better performance if memory allows

## Compatibility

This Python implementation generates font files compatible with:
- C++ fontDictionary tool
- Embedded systems using the fontDictionary format
- Wearable devices and IoT displays

## License

This tool is provided as-is for font conversion purposes.

## Support

For issues, questions, or contributions, please refer to the project repository.

## Version History

### v1.0.0 (2025-12-04)
- Initial release
- Full C++ compatibility
- Support for bitmap fonts with 1/2/4/8-bit rendering
- Character cropping and optimization
- Bold, italic, rotation, and gamma correction support
