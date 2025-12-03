"""
FastLz Compression
FastLz 快速压缩
"""
from .base import CompressionAlgorithm
from formats.format_types import COMPRESS_FASTLZ

try:
    import fastlz
    HAS_FASTLZ = True
except ImportError:
    HAS_FASTLZ = False


class FastLzCompression(CompressionAlgorithm):
    """FastLz compression algorithm"""
    
    def __init__(self):
        if not HAS_FASTLZ:
            raise ImportError("fastlz module not found. Install: pip install fastlz")
    
    def compress(self, pixel_data, width, height, pixel_bytes):
        """Compress using FastLz algorithm"""
        compressed_data = bytearray()
        line_offsets = []
        
        bytes_per_line = width * pixel_bytes
        
        for line in range(height):
            line_start = line * bytes_per_line
            line_end = line_start + bytes_per_line
            line_data = pixel_data[line_start:line_end]
            
            # Record offset for this line
            line_offsets.append(len(compressed_data))
            
            # Compress line using FastLz
            compressed_line = fastlz.compress(bytes(line_data))
            compressed_data.extend(compressed_line)
        
        return compressed_data, line_offsets, {
            'feature_1': 0,
            'feature_2': 0
        }
    
    def get_algorithm_type(self):
        return COMPRESS_FASTLZ
