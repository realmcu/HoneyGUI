"""
Binary font file writer for bitmap fonts.

This module provides classes for writing binary font files in the format
compatible with the C++ fontDictionary tool.
"""

import struct
from dataclasses import dataclass
from pathlib import Path
from typing import BinaryIO, Dict, List, Optional, Tuple, Union

from .config import FontConfig
from .constants import (
    FILE_FLAG_BITMAP,
    FILE_FLAG_VECTOR,
    VERSION_MAJOR,
    VERSION_MINOR,
    VERSION_REVISION,
    INDEX_METHOD_ADDRESS,
    INDEX_METHOD_OFFSET,
    MAX_UNICODE_COUNT,
)


@dataclass
class GlyphMetrics:
    """
    Glyph metrics information.
    
    Attributes:
        advance: Horizontal advance width
        bearing_x: Horizontal bearing (left side bearing)
        bearing_y: Vertical bearing (top side bearing)
        width: Glyph width in pixels
        height: Glyph height in pixels
        baseline_offset: Offset from baseline
    """
    advance: int
    bearing_x: int
    bearing_y: int
    width: int
    height: int
    baseline_offset: int = 0


@dataclass
class CropInfo:
    """
    Character cropping information.
    
    Attributes:
        top_skip: Number of rows skipped from top
        left_skip: Number of columns skipped from left
        valid_width: Width of valid pixel area
        valid_height: Height of valid pixel area
    """
    top_skip: int
    left_skip: int
    valid_width: int
    valid_height: int


class BitmapFontHeader:
    """
    Bitmap font file header.
    
    This class represents the header structure of a bitmap font file,
    compatible with the C++ fontDictionary format.
    """
    
    def __init__(self, font_name: str, config: FontConfig, char_count: int):
        """
        Initialize bitmap font header.
        
        Args:
            font_name: Name of the font (without extension)
            config: Font configuration
            char_count: Number of characters in the font
        """
        self.file_flag: int = FILE_FLAG_BITMAP
        self.version_major: int = VERSION_MAJOR
        self.version_minor: int = VERSION_MINOR
        self.version_revision: int = VERSION_REVISION
        # Note: C++ header field mapping (confusing but must match exactly):
        # C++ 'size' field gets the recalculated fontSize value
        # C++ 'fontSize' field gets the backSize value
        # See BitmapFontGenerator.h SetupFontHeader() for reference
        self.size: int = config.font_size  # C++ 'size' field = recalculated fontSize
        self.font_size: int = config.back_size  # C++ 'fontSize' field = backSize
        self.render_mode: int = config.render_mode
        self.bold: bool = config.bold
        self.italic: bool = config.italic
        self.rvd: bool = config.rvd
        self.index_method: int = config.index_method
        self.crop: bool = config.crop
        self.index_area_size: int = self._calculate_index_size(char_count, config)
        self.font_name: str = font_name
    
    def _calculate_index_size(self, char_count: int, config: FontConfig) -> int:
        """
        Calculate the size of the index area in bytes.
        
        Args:
            char_count: Number of characters in the font
            config: Font configuration
            
        Returns:
            Size of index area in bytes
        """
        if config.crop:
            # Crop mode uses 32-bit addresses
            return MAX_UNICODE_COUNT * 4
        elif config.index_method == INDEX_METHOD_ADDRESS:
            # Address mode: 65536 entries of 16-bit values
            return MAX_UNICODE_COUNT * 2
        else:
            # Offset mode: N entries of 16-bit values (Unicode + offset pairs)
            # Each entry is 4 bytes (2 bytes Unicode + 2 bytes offset)
            return char_count * 4
    
    def to_bytes(self) -> bytes:
        """
        Serialize the header to bytes in C++ compatible packed format.
        
        The header structure matches C++ #pragma pack(1) format:
        - length (1 byte): Total header length
        - fileFlag (1 byte): File type flag (1 for bitmap)
        - version_major (1 byte)
        - version_minor (1 byte)
        - version_revision (1 byte)
        - size (1 byte): Recalculated font size (C++ fontSize after metric calculation)
        - fontSize (1 byte): Back size (C++ backSize, original fontSize)
        - renderMode (1 byte)
        - bit_fields (1 byte): bold|italic|rvd|indexMethod|crop (packed into bits)
        - indexAreaSize (4 bytes, little-endian)
        - fontNameLength (1 byte): Length of font name string (including null terminator)
        - fontName (variable): ASCII encoded font name (null-terminated)
        
        Returns:
            Serialized header as bytes
        """
        # Encode font name as ASCII (C++ uses char*)
        font_name_bytes = self.font_name.encode('ascii')
        # Add null terminator
        font_name_bytes += b'\x00'
        font_name_length = len(font_name_bytes)
        
        # Calculate total header length
        # Fixed fields: 1 + 8 + 4 + 1 = 14 bytes
        # Variable: font name length
        header_length = 14 + font_name_length
        
        # Pack bit fields into a single byte
        # Bit 0: bold
        # Bit 1: italic
        # Bit 2: rvd
        # Bit 3: indexMethod
        # Bit 4: crop
        # Bits 5-7: reserved (0)
        bit_fields = 0
        if self.bold:
            bit_fields |= 0x01
        if self.italic:
            bit_fields |= 0x02
        if self.rvd:
            bit_fields |= 0x04
        if self.index_method:
            bit_fields |= 0x08
        if self.crop:
            bit_fields |= 0x10
        
        # Pack fixed fields (C++ packed structure format)
        header_data = struct.pack(
            '<9BI',  # 9 bytes + 1 unsigned int (4 bytes)
            header_length,           # length (1 byte)
            self.file_flag,          # fileFlag (1 byte)
            self.version_major,      # version_major (1 byte)
            self.version_minor,      # version_minor (1 byte)
            self.version_revision,   # version_revision (1 byte)
            self.size,               # size (1 byte)
            self.font_size,          # fontSize (1 byte)
            self.render_mode,        # renderMode (1 byte)
            bit_fields,              # bit fields (1 byte)
            self.index_area_size     # indexAreaSize (4 bytes, little-endian)
        )
        
        # Add font name length
        header_data += struct.pack('<B', font_name_length)
        
        # Append font name (with null terminator)
        return header_data + font_name_bytes


class BinaryFontWriter:
    """
    Binary font file writer.
    
    This class provides methods for writing bitmap font files in the
    format compatible with the C++ fontDictionary tool.
    """
    
    def __init__(self, file_path: Path, header: BitmapFontHeader):
        """
        Initialize binary font writer.
        
        Args:
            file_path: Path to the output font file
            header: Bitmap font header
        """
        self.file_path = file_path
        self.header = header
        self.file: Optional[BinaryIO] = None
        self.index_area_offset: int = 0
        self.data_area_offset: int = 0
    
    def __enter__(self):
        """Enter context manager - open file and write header."""
        self.file = open(self.file_path, 'wb')
        self._write_header()
        return self
    
    def __exit__(self, exc_type, exc_val, exc_tb):
        """Exit context manager - close file."""
        if self.file:
            self.file.close()
    
    def _write_header(self):
        """Write the font file header."""
        if not self.file:
            raise RuntimeError("File not opened")
        
        header_bytes = self.header.to_bytes()
        self.file.write(header_bytes)
        
        # Record the offset where index area starts
        self.index_area_offset = self.file.tell()
    
    def write_index_area(self, index_data: Union[List[int], Dict[int, int], List[Tuple[int, int]]]):
        """
        Write the index area.
        
        Args:
            index_data: Index data in one of the following formats:
                - List[int]: For address mode (65536 entries)
                - Dict[int, int]: For offset mode (unicode -> offset mapping)
                - List[Tuple[int, int]]: For offset mode (list of unicode, offset pairs)
        """
        if not self.file:
            raise RuntimeError("File not opened")
        
        # Ensure we're at the correct position
        self.file.seek(self.index_area_offset)
        
        if self.header.crop:
            # Crop mode: 65536 entries of 32-bit addresses
            # Initialize all to 0xFFFFFFFF (C++ compatibility - indicates character not present)
            index_bytes = bytearray([0xFF] * (MAX_UNICODE_COUNT * 4))
            
            # Fill in the addresses for characters that exist
            if isinstance(index_data, dict):
                for unicode_val, offset in index_data.items():
                    if 0 <= unicode_val < MAX_UNICODE_COUNT:
                        # Write 32-bit offset at the appropriate position
                        struct.pack_into('<I', index_bytes, unicode_val * 4, offset)
            elif isinstance(index_data, list) and len(index_data) > 0:
                if isinstance(index_data[0], tuple):
                    # List of (unicode, offset) pairs
                    for unicode_val, offset in index_data:
                        if 0 <= unicode_val < MAX_UNICODE_COUNT:
                            struct.pack_into('<I', index_bytes, unicode_val * 4, offset)
                else:
                    # List of offsets (assume sequential unicode values)
                    for i, offset in enumerate(index_data):
                        if i < MAX_UNICODE_COUNT:
                            struct.pack_into('<I', index_bytes, i * 4, offset)
            
            self.file.write(index_bytes)
            
        elif self.header.index_method == INDEX_METHOD_ADDRESS:
            # Address mode: 65536 entries of 16-bit offsets
            # Initialize all to 0xFFFF (C++ compatibility - indicates character not present)
            index_bytes = bytearray([0xFF] * (MAX_UNICODE_COUNT * 2))
            
            # Fill in the offsets for characters that exist
            if isinstance(index_data, dict):
                for unicode_val, offset in index_data.items():
                    if 0 <= unicode_val < MAX_UNICODE_COUNT:
                        # Write 16-bit offset at the appropriate position
                        struct.pack_into('<H', index_bytes, unicode_val * 2, offset)
            elif isinstance(index_data, list) and len(index_data) > 0:
                if isinstance(index_data[0], tuple):
                    # List of (unicode, offset) pairs
                    for unicode_val, offset in index_data:
                        if 0 <= unicode_val < MAX_UNICODE_COUNT:
                            struct.pack_into('<H', index_bytes, unicode_val * 2, offset)
                else:
                    # List of offsets (assume sequential unicode values)
                    for i, offset in enumerate(index_data):
                        if i < MAX_UNICODE_COUNT:
                            struct.pack_into('<H', index_bytes, i * 2, offset)
            
            self.file.write(index_bytes)
            
        else:
            # Offset mode: N entries of (unicode, offset) pairs
            # Each entry is 4 bytes: 2 bytes unicode + 2 bytes offset
            if isinstance(index_data, dict):
                # Convert dict to list of tuples
                pairs = sorted(index_data.items())
            elif isinstance(index_data, list) and len(index_data) > 0:
                if isinstance(index_data[0], tuple):
                    pairs = index_data
                else:
                    # Assume sequential unicode values
                    pairs = [(i, offset) for i, offset in enumerate(index_data)]
            else:
                pairs = []
            
            # Write each pair
            for unicode_val, offset in pairs:
                self.file.write(struct.pack('<HH', unicode_val, offset))
        
        # Record where data area starts
        self.data_area_offset = self.file.tell()
    
    def write_character_data(
        self,
        unicode_value: int,
        bitmap: bytes,
        metrics: GlyphMetrics,
        crop_info: Optional[CropInfo] = None
    ) -> int:
        """
        Write character data to the file.
        
        Args:
            unicode_value: Unicode value of the character
            bitmap: Packed bitmap data
            metrics: Glyph metrics
            crop_info: Optional cropping information (for crop mode)
            
        Returns:
            Offset where the character data was written
        """
        if not self.file:
            raise RuntimeError("File not opened")
        
        # Get current position (this is the offset for this character)
        offset = self.file.tell()
        
        if self.header.crop and crop_info:
            # Crop mode: write position info (4 bytes) + bitmap data
            # Position info: top_skip (1 byte) + left_skip (1 byte) + width (1 byte) + height (1 byte)
            position_info = struct.pack(
                '<BBBB',
                crop_info.top_skip & 0xFF,
                crop_info.left_skip & 0xFF,
                crop_info.valid_width & 0xFF,
                crop_info.valid_height & 0xFF
            )
            self.file.write(position_info)
        
        # Write bitmap data
        self.file.write(bitmap)
        
        return offset
    
    def update_crop_index(self, unicode_value: int, offset: int):
        """
        Update the index entry for a character in crop mode.
        
        This method seeks back to the index area and updates the offset
        for the specified character.
        
        Args:
            unicode_value: Unicode value of the character
            offset: Offset where the character data is located
        """
        if not self.file:
            raise RuntimeError("File not opened")
        
        if not self.header.crop:
            raise RuntimeError("update_crop_index only valid in crop mode")
        
        if not (0 <= unicode_value < MAX_UNICODE_COUNT):
            raise ValueError(f"Invalid unicode value: {unicode_value}")
        
        # Save current position
        current_pos = self.file.tell()
        
        # Seek to the index entry for this character
        index_entry_offset = self.index_area_offset + (unicode_value * 4)
        self.file.seek(index_entry_offset)
        
        # Write the 32-bit offset
        self.file.write(struct.pack('<I', offset))
        
        # Restore position
        self.file.seek(current_pos)


class VectorFontHeader:
    """
    Vector font file header.
    
    This class represents the header structure of a vector font file,
    compatible with the C++ fontDictionary format.
    """
    
    def __init__(self, font_name: str, config: FontConfig, char_count: int,
                 ascent: int, descent: int, line_gap: int):
        """
        Initialize vector font header.
        
        Args:
            font_name: Name of the font (without extension)
            config: Font configuration
            char_count: Number of characters in the font
            ascent: Font ascent value
            descent: Font descent value
            line_gap: Font line gap value
        """
        self.file_flag: int = FILE_FLAG_VECTOR
        self.version_major: int = 0
        self.version_minor: int = 0
        self.version_revision: int = 0
        self.version_buildnum: int = 1
        self.font_size: int = config.font_size
        self.render_mode: int = config.render_mode
        self.bold: bool = config.bold
        self.italic: bool = config.italic
        self.rvd: bool = config.rvd
        self.index_method: int = config.index_method
        self.index_area_size: int = self._calculate_index_size(char_count, config)
        self.ascent: int = ascent
        self.descent: int = descent
        self.line_gap: int = line_gap
        self.font_name: str = font_name
    
    def _calculate_index_size(self, char_count: int, config: FontConfig) -> int:
        """
        Calculate the size of the index area in bytes for vector fonts.
        
        Args:
            char_count: Number of characters in the font
            config: Font configuration
            
        Returns:
            Size of index area in bytes
        """
        if config.index_method == INDEX_METHOD_ADDRESS:
            # Address mode: 65536 entries of 32-bit addresses
            return MAX_UNICODE_COUNT * 4
        else:
            # Offset mode: N entries of (Unicode + 32-bit offset) pairs
            # Each entry is 6 bytes (2 bytes Unicode + 4 bytes offset)
            return char_count * 6
    
    def to_bytes(self) -> bytes:
        """
        Serialize the header to bytes.
        
        The header structure (little-endian):
        - length (4 bytes): Total header length
        - fileFlag (4 bytes): File type flag (2 for vector)
        - version_major (4 bytes)
        - version_minor (4 bytes)
        - version_revision (4 bytes)
        - version_buildnum (4 bytes)
        - fontSize (4 bytes)
        - renderMode (4 bytes)
        - bold (4 bytes): 1 if true, 0 if false
        - italic (4 bytes): 1 if true, 0 if false
        - rvd (4 bytes): 1 if true, 0 if false
        - indexMethod (4 bytes)
        - indexAreaSize (4 bytes)
        - fontNameLength (4 bytes): Length of font name string
        - ascent (4 bytes): Font ascent
        - descent (4 bytes): Font descent
        - lineGap (4 bytes): Font line gap
        - fontName (variable): UTF-8 encoded font name
        
        Returns:
            Serialized header as bytes
        """
        # Encode font name as UTF-8
        font_name_bytes = self.font_name.encode('utf-8')
        font_name_length = len(font_name_bytes)
        
        # Calculate total header length
        # Fixed fields: 17 * 4 = 68 bytes
        # Variable: font name length
        header_length = 68 + font_name_length
        
        # Pack fixed fields (all little-endian)
        # Note: descent is signed, others are unsigned
        header_data = struct.pack(
            '<15IiI',  # 15 unsigned, 1 signed (descent), 1 unsigned
            header_length,           # length
            self.file_flag,          # fileFlag (2 for vector)
            self.version_major,      # version_major
            self.version_minor,      # version_minor
            self.version_revision,   # version_revision
            self.version_buildnum,   # version_buildnum
            self.font_size,          # fontSize
            self.render_mode,        # renderMode
            1 if self.bold else 0,   # bold
            1 if self.italic else 0, # italic
            1 if self.rvd else 0,    # rvd
            self.index_method,       # indexMethod
            self.index_area_size,    # indexAreaSize
            font_name_length,        # fontNameLength
            self.ascent,             # ascent (unsigned)
            self.descent,            # descent (signed)
            self.line_gap            # lineGap (unsigned)
        )
        
        # Append font name
        return header_data + font_name_bytes
