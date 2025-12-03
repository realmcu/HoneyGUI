"""
Pixel Format Converter
像素格式转换
"""
import struct
from .format_types import *


def rgb_to_rgb565(r, g, b):
    """Convert RGB888 to RGB565"""
    return ((r >> 3) << 11) | ((g >> 2) << 5) | (b >> 3)


def convert_pixels(pixels, fmt):
    """
    Convert PIL pixels to target format
    
    Args:
        pixels: List of pixel tuples from PIL
        fmt: Target format (FORMAT_*)
    
    Returns:
        bytearray of pixel data
    """
    pixel_data = bytearray()
    
    if fmt == FORMAT_RGB565:
        for r, g, b in pixels:
            val = rgb_to_rgb565(r, g, b)
            pixel_data.extend(struct.pack('<H', val))
    
    elif fmt == FORMAT_RGB888:
        for r, g, b in pixels:
            pixel_data.extend([b, g, r])  # BGR order
    
    elif fmt == FORMAT_ARGB8888:
        for r, g, b, a in pixels:
            pixel_data.extend([b, g, r, a])  # BGRA order
    
    elif fmt == FORMAT_ARGB8565:
        for r, g, b, a in pixels:
            val = rgb_to_rgb565(r, g, b)
            pixel_data.append(a)
            pixel_data.extend(struct.pack('<H', val))
    
    elif fmt == FORMAT_A8:
        # Extract alpha channel only
        for r, g, b, a in pixels:
            pixel_data.append(a)
    
    return pixel_data
