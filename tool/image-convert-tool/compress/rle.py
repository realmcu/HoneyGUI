"""
RLE (Run-Length Encoding) Compression
RLE 行程编码压缩

RLE 节点格式:
- RGB565:    [len:1] [pixel:2]
- ARGB8565:  [len:1] [pixel:2] [alpha:1]
- RGB888:    [len:1] [b:1] [g:1] [r:1]
- ARGB8888:  [len:1] [pixel:4]
"""
import struct
from .base import CompressionAlgorithm
from formats.format_types import *


class RLECompression(CompressionAlgorithm):
    """RLE compression algorithm"""
    
    def __init__(self, run_length_1=1, run_length_2=0, level=1):
        """
        Args:
            run_length_1: 1阶 RLE Run Length Size (0-3)
                         对应algorithm_type的bit[3:2]
            run_length_2: 2阶 RLE Run Length Size (0-3)
                         对应algorithm_type的bit[5:4]
            level: RLE level (1 or 2)
        
        Note: 默认值 run_length_1=1, run_length_2=0 对应 algorithm_type=0x04
        """
        self.run_length_1 = run_length_1 & 0x03
        self.run_length_2 = run_length_2 & 0x03
        self.level = level
        self.min_run = self.run_length_1 + 1
    
    def compress(self, pixel_data, width, height, pixel_bytes):
        """Compress using RLE algorithm"""
        compressed_data = bytearray()
        line_offsets = []
        
        bytes_per_line = width * pixel_bytes
        
        for line in range(height):
            line_start = line * bytes_per_line
            line_end = line_start + bytes_per_line
            line_data = pixel_data[line_start:line_end]
            
            # Record offset for this line
            line_offsets.append(len(compressed_data))
            
            # Compress line
            compressed_line = self._compress_line(line_data, pixel_bytes)
            compressed_data.extend(compressed_line)
        
        return compressed_data, line_offsets, {
            'feature_1': self.run_length_1,
            'feature_2': self.run_length_2
        }
    
    def _compress_line(self, data, pixel_bytes):
        """压缩一行数据"""
        result = bytearray()
        i = 0
        
        while i < len(data):
            # 获取当前像素
            pixel = data[i:i+pixel_bytes]
            
            # 查找连续重复的像素
            run_length = 1
            j = i + pixel_bytes
            while j < len(data) and run_length < 255:
                if data[j:j+pixel_bytes] == pixel:
                    run_length += 1
                    j += pixel_bytes
                else:
                    break
            
            # 写入 RLE 节点: [len] [pixel_data]
            result.append(run_length)
            result.extend(pixel)
            
            i = j
        
        return result
    
    def get_algorithm_type(self):
        return COMPRESS_RLE
