#!/usr/bin/env python3
"""
HoneyGUI Image Converter
Convert PNG/JPG images to HoneyGUI bin format.
"""

import argparse
import struct
import sys
from pathlib import Path

try:
    from PIL import Image
except ImportError:
    print("Error: Pillow not installed. Run: pip install Pillow")
    sys.exit(1)


# GUI_FormatType enum values (must match draw_img.h)
FORMAT_RGB565 = 0
FORMAT_ARGB8565 = 1
FORMAT_RGB888 = 3
FORMAT_ARGB8888 = 4
FORMAT_ALPHAMASK = 9  # Alpha channel only (8-bit grayscale)
FORMAT_A8 = FORMAT_ALPHAMASK  # Alias


def rgb_to_rgb565(r, g, b):
    """Convert RGB888 to RGB565."""
    return ((r >> 3) << 11) | ((g >> 2) << 5) | (b >> 3)


def convert_image(input_path: Path, output_path: Path, fmt: str = "auto") -> bool:
    """
    Convert image to HoneyGUI bin format.
    
    Args:
        input_path: Source image path (PNG/JPG)
        output_path: Output bin file path
        fmt: Output format - "rgb565", "rgb888", "argb8888", "a8", or "auto"
    
    Returns:
        True on success
    """
    img = Image.open(input_path)
    has_alpha = img.mode in ('RGBA', 'LA') or (img.mode == 'P' and 'transparency' in img.info)
    
    # Auto-detect format
    if fmt == "auto":
        fmt = "argb8888" if has_alpha else "rgb565"
    
    # Convert to appropriate mode
    if fmt == "a8":
        # Extract alpha channel only
        if not has_alpha:
            raise ValueError("Image has no alpha channel for A8 format")
        img = img.convert("RGBA")
    elif fmt in ("argb8888", "argb8565"):
        img = img.convert("RGBA")
    else:
        img = img.convert("RGB")
    
    w, h = img.size
    pixels = list(img.getdata())
    
    # Build header (8 bytes)
    # Byte 0: flags (all 0 for uncompressed)
    # Byte 1: type
    # Byte 2-3: width (little-endian)
    # Byte 4-5: height (little-endian)
    # Byte 6: version
    # Byte 7: reserved
    
    type_map = {
        "rgb565": FORMAT_RGB565,
        "rgb888": FORMAT_RGB888,
        "argb8888": FORMAT_ARGB8888,
        "argb8565": FORMAT_ARGB8565,
        "a8": FORMAT_A8,
    }
    pixel_type = type_map[fmt]
    
    header = struct.pack('<BBhhBB', 0, pixel_type, w, h, 0, 0)
    
    # Build pixel data
    pixel_data = bytearray()
    
    if fmt == "rgb565":
        for r, g, b in pixels:
            val = rgb_to_rgb565(r, g, b)
            pixel_data.extend(struct.pack('<H', val))
    
    elif fmt == "rgb888":
        for r, g, b in pixels:
            pixel_data.extend([b, g, r])  # BGR order
    
    elif fmt == "argb8888":
        for r, g, b, a in pixels:
            pixel_data.extend([b, g, r, a])  # BGRA order
    
    elif fmt == "argb8565":
        for r, g, b, a in pixels:
            val = rgb_to_rgb565(r, g, b)
            pixel_data.extend(struct.pack('<H', val))  # RGB565 first
            pixel_data.append(a)  # Alpha last
    
    elif fmt == "a8":
        # Extract alpha channel only (8-bit grayscale)
        for r, g, b, a in pixels:
            pixel_data.append(a)
    
    # Write output
    output_path.parent.mkdir(parents=True, exist_ok=True)
    with open(output_path, 'wb') as f:
        f.write(header)
        f.write(pixel_data)
    
    return True


def main():
    parser = argparse.ArgumentParser(description='Convert images to HoneyGUI bin format')
    parser.add_argument('-i', '--input', required=True, help='Input image path')
    parser.add_argument('-o', '--output', required=True, help='Output bin path')
    parser.add_argument('-f', '--format', default='auto',
                        choices=['auto', 'rgb565', 'rgb888', 'argb8888', 'argb8565', 'a8'],
                        help='Output pixel format (default: auto)')
    
    args = parser.parse_args()
    
    input_path = Path(args.input)
    output_path = Path(args.output)
    
    if not input_path.exists():
        print(f"Error: Input file not found: {input_path}")
        sys.exit(1)
    
    try:
        convert_image(input_path, output_path, args.format)
        print(f"Converted: {input_path} -> {output_path}")
    except Exception as e:
        print(f"Error: {e}")
        sys.exit(1)


if __name__ == '__main__':
    main()
