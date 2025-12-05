"""
Image processing utilities for font rendering.

This module provides image processing operations including gamma correction,
rotation, bit packing, and character cropping.
"""

import numpy as np
from dataclasses import dataclass
from typing import Tuple

from .constants import (
    ROTATION_0, ROTATION_90, ROTATION_270, ROTATION_180,
    BYTE_ALIGNMENT,
    PIXELS_PER_BYTE,
    RENDER_MODE_1BIT, RENDER_MODE_2BIT, RENDER_MODE_4BIT, RENDER_MODE_8BIT
)


@dataclass
class CropInfo:
    """Information about character cropping."""
    top_skip: int  # Number of rows skipped from top
    left_skip: int  # Number of columns skipped from left
    valid_width: int  # Width of valid pixel region
    valid_height: int  # Height of valid pixel region


class ImageProcessor:
    """Image processing operations for font rendering."""
    
    @staticmethod
    def apply_gamma(image: np.ndarray, gamma: float) -> np.ndarray:
        """
        Apply gamma correction to an image.
        
        Uses the formula: output = 255 * (input/255)^gamma
        
        Args:
            image: Input image as numpy array with values in range [0, 255]
            gamma: Gamma correction value (typically 0.1 to 5.0)
        
        Returns:
            Gamma-corrected image as numpy array with values in range [0, 255]
        
        Requirements: 4.6, 11.2
        """
        # Normalize to [0, 1]
        normalized = image.astype(np.float64) / 255.0
        
        # Apply gamma correction
        corrected = np.power(normalized, gamma)
        
        # Scale back to [0, 255]
        result = (corrected * 255.0).astype(np.uint8)
        
        return result
    
    @staticmethod
    def rotate_image(image: np.ndarray, rotation: int) -> np.ndarray:
        """
        Rotate an image by the specified rotation value.
        
        Args:
            image: Input image as numpy array
            rotation: Rotation value:
                     0 = no rotation (0°)
                     1 = 90° clockwise
                     2 = 90° counter-clockwise (270° clockwise)
                     3 = 180°
        
        Returns:
            Rotated image as numpy array
        
        Requirements: 5.1, 5.2, 5.3, 5.4
        """
        if rotation == ROTATION_0:
            # No rotation
            return image.copy()
        elif rotation == ROTATION_90:
            # 90° clockwise: transpose then flip horizontally
            return np.transpose(image)[:, ::-1]
        elif rotation == ROTATION_270:
            # 90° counter-clockwise (270° clockwise): transpose then flip vertically
            return np.transpose(image)[::-1, :]
        elif rotation == ROTATION_180:
            # 180°: flip both horizontally and vertically
            return image[::-1, ::-1]
        else:
            raise ValueError(f"Invalid rotation value: {rotation}. Must be 0, 1, 2, or 3.")
    
    @staticmethod
    def adjust_dimensions_for_byte_alignment(height: int, width: int) -> Tuple[int, int]:
        """
        Adjust bitmap dimensions to ensure byte alignment.
        
        Dimensions are adjusted to be multiples of 8 to ensure proper
        byte alignment for bit-packed data.
        
        Args:
            height: Original height
            width: Original width
        
        Returns:
            Tuple of (adjusted_height, adjusted_width), both multiples of 8
        
        Requirements: 5.5
        """
        # Round up to nearest multiple of BYTE_ALIGNMENT (8)
        aligned_height = ((height + BYTE_ALIGNMENT - 1) // BYTE_ALIGNMENT) * BYTE_ALIGNMENT
        aligned_width = ((width + BYTE_ALIGNMENT - 1) // BYTE_ALIGNMENT) * BYTE_ALIGNMENT
        
        return aligned_height, aligned_width
    
    @staticmethod
    def pad_image_for_alignment(image: np.ndarray) -> np.ndarray:
        """
        Pad an image to ensure dimensions are multiples of 8.
        
        Args:
            image: Input image as numpy array
        
        Returns:
            Padded image with dimensions that are multiples of 8
        
        Requirements: 5.5
        """
        height, width = image.shape
        aligned_height, aligned_width = ImageProcessor.adjust_dimensions_for_byte_alignment(height, width)
        
        # If already aligned, return copy
        if height == aligned_height and width == aligned_width:
            return image.copy()
        
        # Create padded image filled with zeros (black)
        padded = np.zeros((aligned_height, aligned_width), dtype=image.dtype)
        
        # Copy original image into top-left corner
        padded[:height, :width] = image
        
        return padded
    
    @staticmethod
    def pack_to_1bit(image: np.ndarray) -> bytes:
        """
        Pack image to 1-bit format (monochrome).
        
        Each byte contains 8 pixels. Pixels >= 128 are considered white (1),
        pixels < 128 are considered black (0).
        
        Bit order matches C++ implementation:
        - Pixel 0 at bit 0 (LSB)
        - Pixel 7 at bit 7 (MSB)
        
        Args:
            image: Input image as numpy array with values in range [0, 255]
        
        Returns:
            Packed bytes with 8 pixels per byte
        
        Requirements: 4.1
        """
        height, width = image.shape
        
        # Threshold: >= 128 is white (1), < 128 is black (0)
        binary = (image >= 128).astype(np.uint8)
        
        # Pack 8 pixels per byte (matching C++ bit order)
        packed = []
        for row in binary:
            for col_start in range(0, width, 8):
                byte_val = 0
                for bit_pos in range(8):
                    col = col_start + bit_pos
                    if col < width:
                        if row[col]:
                            # C++ uses: data[pos/8] |= (value << (pos%8))
                            # So pixel 0 is at LSB, pixel 7 is at MSB
                            byte_val |= (1 << bit_pos)
                packed.append(byte_val)
        
        return bytes(packed)
    
    @staticmethod
    def pack_to_2bit(image: np.ndarray) -> bytes:
        """
        Pack image to 2-bit format (4 gray levels).
        
        Each byte contains 4 pixels. Pixel values are quantized to 4 levels:
        0-63 -> 0, 64-127 -> 1, 128-191 -> 2, 192-255 -> 3
        
        Bit order matches C++ implementation:
        - Pixel 0 at bits 0-1 (LSB)
        - Pixel 1 at bits 2-3
        - Pixel 2 at bits 4-5
        - Pixel 3 at bits 6-7 (MSB)
        
        Args:
            image: Input image as numpy array with values in range [0, 255]
        
        Returns:
            Packed bytes with 4 pixels per byte
        
        Requirements: 4.2
        """
        height, width = image.shape
        
        # Quantize to 4 levels (2 bits)
        quantized = (image >> 6).astype(np.uint8)  # Divide by 64, gives 0-3
        
        # Pack 4 pixels per byte (matching C++ bit order)
        packed = []
        for row in quantized:
            for col_start in range(0, width, 4):
                byte_val = 0
                for pixel_pos in range(4):
                    col = col_start + pixel_pos
                    if col < width:
                        pixel_val = row[col]
                        # C++ uses: data[pos/4] |= (value << ((pos%4) * 2))
                        # So pixel 0 is at LSB, pixel 3 is at MSB
                        byte_val |= (pixel_val << (pixel_pos * 2))
                packed.append(byte_val)
        
        return bytes(packed)
    
    @staticmethod
    def pack_to_4bit(image: np.ndarray) -> bytes:
        """
        Pack image to 4-bit format (16 gray levels).
        
        Each byte contains 2 pixels. Pixel values are quantized to 16 levels:
        0-15 -> 0, 16-31 -> 1, ..., 240-255 -> 15
        
        Bit order matches C++ implementation:
        - Pixel 0 at bits 0-3 (low nibble)
        - Pixel 1 at bits 4-7 (high nibble)
        
        Args:
            image: Input image as numpy array with values in range [0, 255]
        
        Returns:
            Packed bytes with 2 pixels per byte
        
        Requirements: 4.3
        """
        height, width = image.shape
        
        # Quantize to 16 levels (4 bits)
        quantized = (image >> 4).astype(np.uint8)  # Divide by 16, gives 0-15
        
        # Pack 2 pixels per byte (matching C++ bit order)
        packed = []
        for row in quantized:
            for col_start in range(0, width, 2):
                byte_val = 0
                # C++ uses: data[pos/2] |= (value << ((pos%2) * 4))
                # So pixel 0 is at low nibble, pixel 1 is at high nibble
                byte_val |= row[col_start]  # First pixel in low nibble
                if col_start + 1 < width:
                    byte_val |= (row[col_start + 1] << 4)  # Second pixel in high nibble
                packed.append(byte_val)
        
        return bytes(packed)
    
    @staticmethod
    def pack_to_8bit(image: np.ndarray) -> bytes:
        """
        Pack image to 8-bit format (256 gray levels).
        
        Each byte contains 1 pixel. No quantization is performed.
        
        Args:
            image: Input image as numpy array with values in range [0, 255]
        
        Returns:
            Packed bytes with 1 pixel per byte
        
        Requirements: 4.4
        """
        # Simply flatten the array and convert to bytes
        return image.flatten().tobytes()
    
    @staticmethod
    def crop_character(image: np.ndarray) -> Tuple[np.ndarray, CropInfo]:
        """
        Crop character to remove leading and trailing whitespace.
        
        Detects the valid pixel boundary and removes:
        - Leading empty rows from the top
        - Trailing empty columns from the right
        
        Args:
            image: Input image as numpy array
        
        Returns:
            Tuple of (cropped_image, crop_info)
        
        Requirements: 6.1, 6.2, 6.3
        """
        height, width = image.shape
        
        # Find first non-zero row (top boundary)
        top_skip = 0
        for i in range(height):
            if np.any(image[i, :] > 0):
                top_skip = i
                break
        else:
            # All pixels are zero - empty character
            return np.zeros((0, 0), dtype=image.dtype), CropInfo(
                top_skip=height,
                left_skip=0,
                valid_width=0,
                valid_height=0
            )
        
        # Find last non-zero row (bottom boundary)
        bottom = height - 1
        for i in range(height - 1, -1, -1):
            if np.any(image[i, :] > 0):
                bottom = i
                break
        
        # Find first non-zero column (left boundary)
        left_skip = 0
        for j in range(width):
            if np.any(image[:, j] > 0):
                left_skip = j
                break
        
        # Find last non-zero column (right boundary)
        right = width - 1
        for j in range(width - 1, -1, -1):
            if np.any(image[:, j] > 0):
                right = j
                break
        
        # Calculate valid dimensions
        valid_height = bottom - top_skip + 1
        valid_width = right - left_skip + 1
        
        # Crop the image
        cropped = image[top_skip:bottom + 1, left_skip:right + 1].copy()
        
        # Create crop info
        crop_info = CropInfo(
            top_skip=top_skip,
            left_skip=left_skip,
            valid_width=valid_width,
            valid_height=valid_height
        )
        
        return cropped, crop_info
