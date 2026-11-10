#
# Copyright (c) 2026, Realtek Semiconductor Corporation
#
# SPDX-License-Identifier: MIT
#

"""
YUV Sampling + Blur Compression
YUV 降采样 + 模糊压缩

YUV 格式（参考 acc_sw_idu.c）:
- YUV444: [Y U V] 每像素3字节
- YUV422: [Y0 U0 Y1 V0 Y2 U1 Y3 V1] 每4像素8字节
- YUV411: [Y0 U0 Y1 Y2 V0 Y3] 每4像素6字节
"""
import struct
from .base import CompressionAlgorithm
from formats.format_types import COMPRESS_YUV, COMPRESS_YUV_FASTLZ

# Only use RTK FastLz (standard fastlz removed)
try:
    import rtk_fastlz
    HAS_FASTLZ = True
except ImportError:
    HAS_FASTLZ = False


class YUVCompression(CompressionAlgorithm):
    """YUV sampling + blur compression"""
    
    def __init__(self, sample_mode='yuv444', blur_bits=0, use_fastlz=False):
        """
        Args:
            sample_mode: 'yuv444', 'yuv422', 'yuv411'
            blur_bits: Number of low bits to discard (0, 1, 2, 4)
            use_fastlz: Apply FastLz after YUV sampling
        """
        self.sample_mode = sample_mode
        self.blur_bits = blur_bits
        self.use_fastlz = use_fastlz
        
        if use_fastlz and not HAS_FASTLZ:
            raise ImportError("fastlz module required for YUV+FastLz")
    
    def compress(self, pixel_data, width, height, pixel_bytes):
        """Compress using YUV sampling"""
        compressed_data = bytearray()
        line_offsets = []
        
        bytes_per_line = width * pixel_bytes
        
        for line in range(height):
            line_start = line * bytes_per_line
            line_end = line_start + bytes_per_line
            line_data = pixel_data[line_start:line_end]
            
            # Record offset
            line_offsets.append(len(compressed_data))
            
            # Convert to YUV and apply sampling
            yuv_data = self._rgb_to_yuv_line(line_data, width, pixel_bytes)
            sampled_data = self._apply_sampling(yuv_data, width)
            blurred_data = self._apply_blur(sampled_data, width)
            
            # Optional FastLz compression
            if self.use_fastlz:
                compressed_line = rtk_fastlz.compress(bytes(blurred_data))
                compressed_data.extend(compressed_line)
            else:
                compressed_data.extend(blurred_data)
        
        # Map sample_mode to feature_1 value
        sample_mode_map = {
            'yuv444': 0,
            'yuv422': 1,
            'yuv411': 2
        }
        
        # Map blur_bits to feature_2 value
        blur_bits_map = {
            0: 0,
            1: 1,
            2: 2,
            4: 3
        }
        
        return compressed_data, line_offsets, {
            'feature_1': sample_mode_map.get(self.sample_mode, 0),
            'feature_2': blur_bits_map.get(self.blur_bits, 0)
        }
    
    def _rgb_to_yuv_line(self, data, width, pixel_bytes):
        """Convert RGB line to YUV"""
        yuv_data = []
        
        for i in range(0, len(data), pixel_bytes):
            if pixel_bytes == 2:  # RGB565
                pixel = struct.unpack('<H', data[i:i+2])[0]
                r = ((pixel >> 11) & 0x1F) << 3
                g = ((pixel >> 5) & 0x3F) << 2
                b = (pixel & 0x1F) << 3
            elif pixel_bytes == 3:  # RGB888 or ARGB8565
                b, g, r = data[i:i+3]
            elif pixel_bytes == 4:  # ARGB8888
                b, g, r, a = data[i:i+4]
            
            # RGB to YUV conversion (ITU-R BT.601)
            y = int(0.299 * r + 0.587 * g + 0.114 * b)
            u = int(-0.169 * r - 0.331 * g + 0.500 * b + 128)
            v = int(0.500 * r - 0.419 * g - 0.081 * b + 128)
            
            # Clamp to 0-255
            y = max(0, min(255, y))
            u = max(0, min(255, u))
            v = max(0, min(255, v))
            
            yuv_data.append((y, u, v))
        
        return yuv_data
    
    def _apply_sampling(self, yuv_data, width):
        """Apply YUV subsampling"""
        result = bytearray()
        
        if self.sample_mode == 'yuv444':
            # 4:4:4 - No subsampling: [Y U V] per pixel
            for y, u, v in yuv_data:
                result.extend([y, u, v])
        
        elif self.sample_mode == 'yuv422':
            # 4:2:2 - Format: [Y0 U0 Y1 V0 Y2 U1 Y3 V1]
            # U/V sampled every 2 pixels horizontally
            for i in range(0, len(yuv_data), 4):
                # Get 4 pixels
                pixels = yuv_data[i:i+4]
                if len(pixels) < 4:
                    # Handle remainder
                    for y, u, v in pixels:
                        result.extend([y, u, v])
                    break
                
                y0, u0, v0 = pixels[0]
                y1, u1, v1 = pixels[1]
                y2, u2, v2 = pixels[2]
                y3, u3, v3 = pixels[3]
                
                # Average U/V for pairs
                u_avg_0 = (u0 + u1) // 2
                v_avg_0 = (v0 + v1) // 2
                u_avg_1 = (u2 + u3) // 2
                v_avg_1 = (v2 + v3) // 2
                
                result.extend([y0, u_avg_0, y1, v_avg_0, y2, u_avg_1, y3, v_avg_1])
        
        elif self.sample_mode == 'yuv411':
            # 4:1:1 - Format: [Y0 U0 Y1 Y2 V0 Y3]
            # U/V sampled every 4 pixels
            for i in range(0, len(yuv_data), 4):
                pixels = yuv_data[i:i+4]
                if len(pixels) < 4:
                    # Handle remainder
                    for y, u, v in pixels:
                        result.extend([y, u, v])
                    break
                
                # Average U/V for all 4 pixels
                u_avg = sum(p[1] for p in pixels) // 4
                v_avg = sum(p[2] for p in pixels) // 4
                
                result.extend([
                    pixels[0][0], u_avg,
                    pixels[1][0],
                    pixels[2][0], v_avg,
                    pixels[3][0]
                ])
        
        return result
    
    def _apply_blur(self, data, width):
        """Apply blur by discarding low bits and bit packing
        
        Blur works by:
        1. Masking off the low bits (blur_bits)
        2. Packing the remaining high bits together
        
        For example, with 1-bit blur:
        - Original: 8 bits per byte
        - After blur: 7 bits per byte
        - Pack 8 bytes into 7 bytes
        """
        if self.blur_bits == 0:
            return data
        
        # Calculate bits remaining after blur
        bits_per_byte = 8 - self.blur_bits
        
        # Bit packing: pack multiple values into fewer bytes
        result = bytearray()
        bit_buffer = 0
        bits_in_buffer = 0
        
        for byte_val in data:
            # Shift right to discard low bits
            value = byte_val >> self.blur_bits
            
            # Add to bit buffer
            bit_buffer = (bit_buffer << bits_per_byte) | value
            bits_in_buffer += bits_per_byte
            
            # Output complete bytes
            while bits_in_buffer >= 8:
                bits_in_buffer -= 8
                output_byte = (bit_buffer >> bits_in_buffer) & 0xFF
                result.append(output_byte)
        
        # Output remaining bits if any
        if bits_in_buffer > 0:
            output_byte = (bit_buffer << (8 - bits_in_buffer)) & 0xFF
            result.append(output_byte)
        
        return result
    
    def get_algorithm_type(self):
        return COMPRESS_YUV_FASTLZ if self.use_fastlz else COMPRESS_YUV
