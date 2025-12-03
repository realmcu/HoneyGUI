"""
Image Converter Main Module
图片转换主模块
"""
from pathlib import Path
from PIL import Image
import struct

from formats.format_types import *
from formats.headers import RGBDataHeader, IMDCFileHeader
from formats.pixel_converter import convert_pixels


class ImageConverter:
    """Main image converter class"""
    
    def __init__(self):
        self.compressor = None
    
    def convert(self, input_path, output_path, fmt='auto', compress=None):
        """
        Convert image to HoneyGUI bin format
        
        Args:
            input_path: Source image path
            output_path: Output bin file path
            fmt: Output format ('auto', 'rgb565', 'rgb888', 'argb8888', 'argb8565', 'a8')
            compress: Compression algorithm instance (None for no compression)
        """
        img = Image.open(input_path)
        has_alpha = img.mode in ('RGBA', 'LA') or (img.mode == 'P' and 'transparency' in img.info)
        
        # Auto-detect format
        if fmt == 'auto':
            fmt = 'argb8888' if has_alpha else 'rgb565'
        
        # Map format string to constant
        format_map = {
            'rgb565': FORMAT_RGB565,
            'rgb888': FORMAT_RGB888,
            'argb8888': FORMAT_ARGB8888,
            'argb8565': FORMAT_ARGB8565,
            'a8': FORMAT_A8,
        }
        
        pixel_format = format_map[fmt]
        
        # Convert image mode
        if fmt == 'a8':
            if not has_alpha:
                raise ValueError("Image has no alpha channel for A8 format")
            img = img.convert('RGBA')
        elif fmt in ('argb8888', 'argb8565'):
            img = img.convert('RGBA')
        else:
            img = img.convert('RGB')
        
        w, h = img.size
        pixels = list(img.getdata())
        
        # Convert pixels
        pixel_data = convert_pixels(pixels, pixel_format)
        
        # Build header
        use_compress = compress is not None
        header = RGBDataHeader(w, h, pixel_format, compress=use_compress)
        
        # Write output
        output_path.parent.mkdir(parents=True, exist_ok=True)
        
        with open(output_path, 'wb') as f:
            f.write(header.pack())
            
            if use_compress:
                # Compress data
                pixel_bytes = FORMAT_TO_BPP[pixel_format]
                compressed_data, line_offsets, params = compress.compress(
                    pixel_data, w, h, pixel_bytes
                )
                
                # Write IMDC header
                imdc_header = IMDCFileHeader(
                    algorithm=compress.get_algorithm_type(),
                    feature_1=params.get('feature_1', 0),
                    feature_2=params.get('feature_2', 0),
                    pixel_bytes=FORMAT_TO_PIXEL_BYTES[pixel_format],
                    width=w,
                    height=h
                )
                f.write(imdc_header.pack())
                
                # Write line offset table
                for offset in line_offsets:
                    f.write(struct.pack('<I', offset))
                
                # Write compressed data
                f.write(compressed_data)
            else:
                # Write uncompressed data
                f.write(pixel_data)
        
        return True
