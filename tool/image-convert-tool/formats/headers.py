"""
HoneyGUI Binary File Headers
定义 bin 文件的头部结构
"""
import struct
from .format_types import *


class RGBDataHeader:
    """gui_rgb_data_head_t - 8 bytes"""
    
    def __init__(self, width, height, fmt, compress=False, resize=0):
        self.scan = 0
        self.align = 0
        self.resize = resize  # 0=no, 1=50%, 2=70%, 3=80%
        self.compress = 1 if compress else 0
        self.jpeg = 0
        self.idu = 0
        self.rsvd = 0
        self.type = fmt
        self.width = width
        self.height = height
        self.version = 0
        self.rsvd2 = 0
    
    def pack(self):
        """Pack to 8 bytes"""
        # Byte 0: flags (bitfield)
        flags = (
            (self.scan & 0x01) |
            ((self.align & 0x01) << 1) |
            ((self.resize & 0x03) << 2) |
            ((self.compress & 0x01) << 4) |
            ((self.jpeg & 0x01) << 5) |
            ((self.idu & 0x01) << 6) |
            ((self.rsvd & 0x01) << 7)
        )
        
        return struct.pack('<BBhhBB',
            flags & 0xFF,
            self.type & 0xFF,
            self.width,
            self.height,
            self.version & 0xFF,
            self.rsvd2 & 0xFF
        )


class IMDCFileHeader:
    """imdc_file_header_t - 12 bytes"""
    
    def __init__(self, algorithm, feature_1, feature_2, pixel_bytes, width, height):
        self.algorithm = algorithm & 0x03
        self.feature_1 = feature_1 & 0x03
        self.feature_2 = feature_2 & 0x03
        self.pixel_bytes = pixel_bytes & 0x03
        self.reserved = [0, 0, 0]
        self.raw_pic_width = width
        self.raw_pic_height = height
    
    def pack(self):
        """Pack to 12 bytes"""
        # Byte 0: algorithm_type (bitfield)
        algorithm_type = (
            (self.algorithm & 0x03) |
            ((self.feature_1 & 0x03) << 2) |
            ((self.feature_2 & 0x03) << 4) |
            ((self.pixel_bytes & 0x03) << 6)
        )
        
        return struct.pack('<BBBBII',
            algorithm_type,
            self.reserved[0],
            self.reserved[1],
            self.reserved[2],
            self.raw_pic_width,
            self.raw_pic_height
        )
