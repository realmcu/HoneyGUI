#
# Copyright (c) 2026, Realtek Semiconductor Corporation
#
# SPDX-License-Identifier: MIT
#

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
                
                # Calculate offsets relative to imdc_file_t start
                # imdc_file_t starts at offset 8 (after gui_rgb_data_head_t)
                # Compressed data starts after: imdc_header(12) + offset_table((h+1)*4)
                # Note: We need h+1 offsets (one per line + one end marker)
                # So offset is relative to imdc_file_t, which means:
                # offset = 12 + (h+1)*4 + relative_offset_in_compressed_data
                imdc_offset = 12 + (h + 1) * 4
                
                # Write line offset table (relative to imdc_file_t start)
                # Write h offsets for each line
                for offset in line_offsets:
                    # offset relative to imdc_file_t start
                    relative_offset = imdc_offset + offset
                    f.write(struct.pack('<I', relative_offset))
                
                # Write the final offset (end of compressed data)
                end_offset = imdc_offset + len(compressed_data)
                f.write(struct.pack('<I', end_offset))
                
                # Write compressed data
                # Note: For FastLz, the first 8 bytes (metadata) overlap with
                # the last 8 bytes of the offset table, so skip them
                from compress.fastlz import FastLzCompression
                if isinstance(compress, FastLzCompression):
                    f.write(compressed_data[8:])
                else:
                    f.write(compressed_data)
            else:
                # Write uncompressed data
                f.write(pixel_data)
        
        return True
