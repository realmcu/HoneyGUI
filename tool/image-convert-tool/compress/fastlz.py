#
# Copyright (c) 2026, Realtek Semiconductor Corporation
#
# SPDX-License-Identifier: MIT
#

"""
FastLz Compression
FastLz fastlz compression algorithm
"""
try:
    from .base import CompressionAlgorithm
    from .formats.format_types import COMPRESS_FASTLZ
except ImportError:
    # Handle direct execution
    try:
        from base import CompressionAlgorithm
        from formats.format_types import COMPRESS_FASTLZ
    except ImportError:
        # Fallback for standalone testing
        class CompressionAlgorithm:
            pass
        COMPRESS_FASTLZ = 1  # Integer value matching format_types.py

# Use only RTK FastLz (Windows tool compatible)
try:
    import rtk_fastlz
    HAS_RTK_FASTLZ = True
except ImportError:
    HAS_RTK_FASTLZ = False


class FastLzCompression(CompressionAlgorithm):
    """FastLz compression algorithm"""
    
    def __init__(self):
        if not HAS_RTK_FASTLZ:
            raise ImportError(
                "RTK FastLz module not found.\n"
                "Please install: cd compress/rtk_fastlz_binding && build_and_install.bat"
            )
        self.use_rtk = True
    
    def compress(self, pixel_data, width, height, pixel_bytes):
        """Compress using FastLz algorithm
        
        Format matches Windows tool:
        - Each compressed line starts with 8-byte metadata (last two line offsets)
        - Followed by FastLz compressed data (without the 4-byte size header)
        """
        import struct
        
        # First pass: compress all lines to calculate offsets
        compressed_lines = []
        bytes_per_line = width * pixel_bytes
        
        for line in range(height):
            line_start = line * bytes_per_line
            line_end = line_start + bytes_per_line
            line_data = pixel_data[line_start:line_end]
            
            # Compress line using RTK FastLz
            pure_fastlz_data = rtk_fastlz.compress(bytes(line_data))
            
            compressed_lines.append(pure_fastlz_data)
        
        # Build compressed data
        # Note: The first 8 bytes will overlap with the offset table
        # (last 8 bytes of offset table = metadata for first line)
        # So we include them here, but converter.py will skip them when writing
        compressed_data = bytearray()
        line_offsets = []
        
        # Calculate what the final offsets will be
        # (after the 8-byte metadata is added to offset table)
        temp_size = sum(len(line) for line in compressed_lines)
        imdc_offset = 12 + (height + 1) * 4
        
        # Metadata = last two offsets
        last_line_offset = imdc_offset + temp_size - len(compressed_lines[-1])
        end_offset = imdc_offset + temp_size
        
        # Build compressed data with metadata at the start
        for i, compressed_line in enumerate(compressed_lines):
            line_offsets.append(len(compressed_data))
            
            if i == 0:
                # Add 8-byte metadata for first line
                # This will overlap with offset table, so converter.py skips it
                compressed_data.extend(struct.pack('<I', last_line_offset))
                compressed_data.extend(struct.pack('<I', end_offset))
            
            compressed_data.extend(compressed_line)
        
        return compressed_data, line_offsets, {
            'feature_1': 0,
            'feature_2': 0
        }
    
    def get_algorithm_type(self):
        return COMPRESS_FASTLZ
