"""
Python Font Converter - A tool for converting TrueType fonts to custom binary formats.

This package provides functionality to convert TrueType fonts into custom binary
font formats suitable for embedded systems and wristband devices.
"""

__version__ = "1.0.0"
__author__ = "Font Converter Team"

from .errors import ErrorCode, FontConverterError
from .config import FontConfig, ConfigManager
from .cli import FontConverterCLI
from .codepage_processor import CodePageProcessor
from .image_processor import ImageProcessor, CropInfo
from .font_generator import FontGenerator
from .bitmap_generator import BitmapFontGenerator

__all__ = [
    "ErrorCode",
    "FontConverterError",
    "FontConfig",
    "ConfigManager",
    "FontConverterCLI",
    "CodePageProcessor",
    "ImageProcessor",
    "CropInfo",
    "FontGenerator",
    "BitmapFontGenerator",
]
