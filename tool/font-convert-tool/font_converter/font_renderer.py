"""
Font rendering module using FreeType library.

This module provides the FontRenderer class for loading TrueType fonts
and rendering individual characters to bitmaps.
"""

import freetype
import numpy as np
from dataclasses import dataclass
from typing import Tuple, Optional

from .config import FontConfig
from .errors import FontRenderError, ErrorCode


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
    baseline_offset: int


@dataclass
class FontMetrics:
    """
    Font-level metrics information.
    
    Attributes:
        ascender: Typographic ascender
        descender: Typographic descender
        height: Line height
        units_per_em: Units per EM square
    """
    ascender: int
    descender: int
    height: int
    units_per_em: int


class FontRenderer:
    """
    FreeType font renderer for generating character bitmaps.
    
    This class handles loading TrueType fonts, setting font parameters,
    and rendering individual characters to grayscale bitmaps.
    """
    
    def __init__(self, font_path: str, config: FontConfig):
        """
        Initialize the font renderer.
        
        Args:
            font_path: Path to the TrueType font file
            config: Font configuration object
            
        Raises:
            FontRenderError: If font loading or initialization fails
        """
        self.config = config
        self.font_path = font_path
        self._bold_strength = 0
        
        try:
            # Load the font face
            self.face = freetype.Face(font_path)
        except Exception as e:
            raise FontRenderError(
                f"Failed to load font file: {font_path}",
                str(e)
            )
        
        # Setup font parameters
        self._setup_font()
    
    def _setup_font(self):
        """
        Setup font parameters based on configuration.
        
        Sets the font size and applies bold effect if configured.
        """
        try:
            # Set character size (font_size is in pixels)
            # FreeType expects size in 1/64th of a point
            # We use 0 for width to let FreeType calculate it proportionally
            self.face.set_char_size(
                width=0,
                height=self.config.font_size * 64,
                hres=72,
                vres=72
            )
            
            # Calculate bold strength if configured
            if self.config.bold:
                # Bold strength is typically 1/24 of the font size
                # FreeType uses 26.6 fixed-point format (1/64th units)
                self._bold_strength = (self.config.font_size * 64) // 24
                
        except Exception as e:
            raise FontRenderError(
                "Failed to setup font parameters",
                str(e)
            )
    
    def update_font_size(self, new_font_size: int):
        """
        Update the font size after initial setup.
        
        This is used when the font size is recalculated based on font metrics
        to ensure glyphs fit within the target box.
        
        Args:
            new_font_size: New font size in pixels
        """
        self.config.font_size = new_font_size
        try:
            # Update FreeType font size
            self.face.set_char_size(
                width=0,
                height=new_font_size * 64,
                hres=72,
                vres=72
            )
            
            # Update bold strength if configured
            if self.config.bold:
                self._bold_strength = (new_font_size * 64) // 24
                
        except Exception as e:
            raise FontRenderError(
                "Failed to update font size",
                str(e)
            )
    
    def _apply_bold(self, bitmap_array: np.ndarray) -> np.ndarray:
        """
        Apply bold effect to a rendered bitmap.
        
        Args:
            bitmap_array: Input bitmap as numpy array
            
        Returns:
            Bitmap with bold effect applied
            
        Note:
            This simulates bold by dilating the bitmap horizontally.
        """
        if bitmap_array.shape[0] == 0 or bitmap_array.shape[1] == 0:
            return bitmap_array
        
        # Create a slightly wider bitmap
        height, width = bitmap_array.shape
        bold_width = width + 1  # Add one pixel for bold effect
        bold_bitmap = np.zeros((height, bold_width), dtype=np.uint8)
        
        # Copy original bitmap
        bold_bitmap[:, :width] = bitmap_array
        
        # Add a shifted copy to create bold effect
        for y in range(height):
            for x in range(width - 1):
                # Take maximum of current and next pixel
                bold_bitmap[y, x + 1] = max(bold_bitmap[y, x + 1], bitmap_array[y, x])
        
        return bold_bitmap
    
    def _apply_italic(self, bitmap_array: np.ndarray) -> np.ndarray:
        """
        Apply italic effect to a rendered bitmap.
        
        Args:
            bitmap_array: Input bitmap as numpy array
            
        Returns:
            Bitmap with italic effect applied
            
        Note:
            This applies a shear transformation to create an italic effect.
            The shear is approximately 12 degrees (tan(12°) ≈ 0.21).
        """
        if bitmap_array.shape[0] == 0 or bitmap_array.shape[1] == 0:
            return bitmap_array
        
        height, width = bitmap_array.shape
        
        # Calculate shear amount (12 degrees)
        shear = 0.21
        max_shift = int(height * shear) + 1
        
        # Create output bitmap with extra width for shear
        italic_width = width + max_shift
        italic_bitmap = np.zeros((height, italic_width), dtype=np.uint8)
        
        # Apply shear transformation
        for y in range(height):
            # Calculate horizontal shift for this row
            # Shift decreases from top to bottom
            shift = int((height - y - 1) * shear)
            
            # Copy row with shift
            italic_bitmap[y, shift:shift + width] = bitmap_array[y, :]
        
        return italic_bitmap
    
    def check_character_valid(self, unicode_value: int) -> bool:
        """
        Check if a character exists in the font.
        
        Args:
            unicode_value: Unicode code point of the character
            
        Returns:
            True if the character exists in the font, False otherwise
        """
        try:
            # Get the glyph index for this character
            glyph_index = self.face.get_char_index(unicode_value)
            
            # If glyph_index is 0, the character doesn't exist
            return glyph_index != 0
        except Exception:
            return False
    
    def render_character(self, unicode_value: int) -> Tuple[np.ndarray, GlyphMetrics]:
        """
        Render a single character to a grayscale bitmap.
        
        Args:
            unicode_value: Unicode code point of the character to render
            
        Returns:
            Tuple of (bitmap, metrics) where:
                - bitmap: numpy array of grayscale pixel values (0-255)
                - metrics: GlyphMetrics object with glyph measurements
                
        Raises:
            FontRenderError: If character rendering fails
        """
        try:
            # Load the glyph for this character
            self.face.load_char(unicode_value, freetype.FT_LOAD_RENDER)
            
            # Get the glyph slot
            glyph = self.face.glyph
            bitmap = glyph.bitmap
            
            # Extract bitmap data
            if bitmap.width == 0 or bitmap.rows == 0:
                # Empty glyph (e.g., space character)
                bitmap_array = np.zeros((1, 1), dtype=np.uint8)
            else:
                # Convert bitmap buffer to numpy array
                bitmap_array = np.array(
                    bitmap.buffer,
                    dtype=np.uint8
                ).reshape(bitmap.rows, bitmap.width)
            
            # Apply bold effect if configured
            if self.config.bold and bitmap_array.size > 1:
                bitmap_array = self._apply_bold(bitmap_array)
            
            # Apply italic effect if configured
            if self.config.italic and bitmap_array.size > 1:
                bitmap_array = self._apply_italic(bitmap_array)
            
            # Create metrics object
            # Note: metrics reflect the original glyph, not the modified one
            # The caller should adjust metrics based on the actual bitmap size
            metrics = GlyphMetrics(
                advance=glyph.advance.x >> 6,  # Convert from 26.6 fixed-point
                bearing_x=glyph.bitmap_left,
                bearing_y=glyph.bitmap_top,
                width=bitmap_array.shape[1],  # Use actual bitmap width
                height=bitmap_array.shape[0],  # Use actual bitmap height
                baseline_offset=glyph.bitmap_top
            )
            
            return bitmap_array, metrics
            
        except Exception as e:
            raise FontRenderError(
                f"Failed to render character U+{unicode_value:04X}",
                str(e)
            )
    
    def get_font_metrics(self) -> FontMetrics:
        """
        Get font-level metrics information.
        
        Returns raw font metrics from the font file (not scaled to current size).
        These are the values needed for calculating font size adjustments.
        
        Returns:
            FontMetrics object with font measurements
        """
        # Return raw font metrics (not scaled)
        # C++ uses face->ascender and face->descender directly for calculations
        return FontMetrics(
            ascender=self.face.ascender,  # Raw value, not scaled
            descender=self.face.descender,  # Raw value, not scaled
            height=self.face.height,  # Raw value, not scaled
            units_per_em=self.face.units_per_EM
        )
