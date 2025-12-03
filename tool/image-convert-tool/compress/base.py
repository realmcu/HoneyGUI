"""
Compression Algorithm Base Class
压缩算法基类
"""
from abc import ABC, abstractmethod


class CompressionAlgorithm(ABC):
    """Base class for compression algorithms"""
    
    @abstractmethod
    def compress(self, pixel_data, width, height, pixel_bytes):
        """
        Compress pixel data
        
        Args:
            pixel_data: Raw pixel data (bytearray)
            width: Image width
            height: Image height
            pixel_bytes: Bytes per pixel
        
        Returns:
            tuple: (compressed_data, line_offsets, algorithm_params)
                - compressed_data: bytearray of compressed data
                - line_offsets: list of uint32 offsets for each line
                - algorithm_params: dict with algorithm-specific params
        """
        pass
    
    @abstractmethod
    def get_algorithm_type(self):
        """Return algorithm type constant"""
        pass
