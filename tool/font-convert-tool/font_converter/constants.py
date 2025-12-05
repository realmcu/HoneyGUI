"""
Constants used throughout the font converter.
"""

# Maximum Unicode character count (full Unicode range)
MAX_UNICODE_COUNT = 65536

# Invalid Unicode value marker
INVALID_UNICODE = 0xFFFF

# Render modes
RENDER_MODE_1BIT = 1
RENDER_MODE_2BIT = 2
RENDER_MODE_4BIT = 4
RENDER_MODE_8BIT = 8

VALID_RENDER_MODES = {RENDER_MODE_1BIT, RENDER_MODE_2BIT, RENDER_MODE_4BIT, RENDER_MODE_8BIT}

# Rotation values
ROTATION_0 = 0    # No rotation
ROTATION_90 = 1   # 90 degrees clockwise
ROTATION_270 = 2  # 90 degrees counter-clockwise (270 clockwise)
ROTATION_180 = 3  # 180 degrees

VALID_ROTATIONS = {ROTATION_0, ROTATION_90, ROTATION_270, ROTATION_180}

# Index methods
INDEX_METHOD_ADDRESS = 0  # Address mode: 65536 entries
INDEX_METHOD_OFFSET = 1   # Offset mode: N entries for N characters

# Output formats
OUTPUT_FORMAT_BITMAP = 0  # Bitmap font
OUTPUT_FORMAT_VECTOR = 1  # Vector font

# File flags
FILE_FLAG_BITMAP = 1  # Bitmap font file
FILE_FLAG_VECTOR = 2  # Vector font file

# Version information
VERSION_MAJOR = 1
VERSION_MINOR = 0
VERSION_REVISION = 2

# Byte alignment
BYTE_ALIGNMENT = 8  # Dimensions must be multiples of 8 for byte alignment

# Default values
DEFAULT_GAMMA = 1.0
DEFAULT_ROTATION = ROTATION_0
DEFAULT_FONT_SIZE = 16
DEFAULT_BACK_SIZE = 0
DEFAULT_RENDER_MODE = RENDER_MODE_1BIT
DEFAULT_INDEX_METHOD = INDEX_METHOD_ADDRESS
DEFAULT_OUTPUT_FORMAT = OUTPUT_FORMAT_BITMAP

# Configuration file names
DEFAULT_JSON_CONFIG = "FontConfig.json"
DEFAULT_INI_CONFIG = "setting.ini"

# Output file extensions
CST_FILE_EXTENSION = ".cst"
FONT_FILE_EXTENSION = ".font"
NOT_SUPPORTED_CHARS_FILE = "NotSupportedChars.txt"

# Bits per pixel for each render mode
BITS_PER_PIXEL = {
    RENDER_MODE_1BIT: 1,
    RENDER_MODE_2BIT: 2,
    RENDER_MODE_4BIT: 4,
    RENDER_MODE_8BIT: 8,
}

# Pixels per byte for each render mode
PIXELS_PER_BYTE = {
    RENDER_MODE_1BIT: 8,
    RENDER_MODE_2BIT: 4,
    RENDER_MODE_4BIT: 2,
    RENDER_MODE_8BIT: 1,
}

# Gray levels for each render mode
GRAY_LEVELS = {
    RENDER_MODE_1BIT: 2,    # Black and white
    RENDER_MODE_2BIT: 4,    # 4 gray levels
    RENDER_MODE_4BIT: 16,   # 16 gray levels
    RENDER_MODE_8BIT: 256,  # 256 gray levels
}
