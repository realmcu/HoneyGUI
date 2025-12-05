"""
Bitmap font generator implementation.

This module provides the BitmapFontGenerator class for generating
bitmap font files from TrueType fonts.
"""

from pathlib import Path
from typing import Dict, List, Tuple

from .font_generator import FontGenerator
from .config import FontConfig
from .codepage_processor import CodePageProcessor
from .font_renderer import FontRenderer, GlyphMetrics
from .image_processor import ImageProcessor, CropInfo
from .binary_writer import BitmapFontHeader, BinaryFontWriter
from .constants import (
    RENDER_MODE_1BIT, RENDER_MODE_2BIT, RENDER_MODE_4BIT, RENDER_MODE_8BIT,
    CST_FILE_EXTENSION, NOT_SUPPORTED_CHARS_FILE, FONT_FILE_EXTENSION,
    INDEX_METHOD_ADDRESS, INDEX_METHOD_OFFSET
)
from .errors import ErrorCode


class BitmapFontGenerator(FontGenerator):
    """
    Bitmap font generator.
    
    This class generates bitmap font files from TrueType fonts with
    configurable rendering options, character sets, and output formats.
    """
    
    def __init__(self, config: FontConfig):
        """
        Initialize bitmap font generator.
        
        Args:
            config: Font configuration object
        """
        super().__init__(config)
    
    def generate(self, exe_dir: Path, out_dir: Path) -> int:
        """
        Generate bitmap font file.
        
        This is the main entry point for font generation. It:
        1. Generates the character set from CodePage files
        2. Creates the font renderer
        3. Generates the .cst character set file
        4. Generates the binary font file
        5. Generates the NotSupportedChars.txt file if needed
        
        Args:
            exe_dir: Executable directory (contains CodePage folder, etc.)
            out_dir: Output directory for generated files
            
        Returns:
            Error code (0 for success, non-zero for error)
        """
        try:
            # Ensure output directory exists
            out_dir.mkdir(parents=True, exist_ok=True)
            
            # Find font file path
            font_path = self._find_font_path(exe_dir)
            
            # Get font name for output files
            font_name = self._get_font_name()
            output_font_name = self._get_output_font_name(font_name)
            
            # Generate character set
            print(f"Generating character set...")
            characters, index_map = self._generate_character_set(exe_dir)
            
            if not characters:
                print("Warning: No characters in character set")
                return ErrorCode.SUCCESS.value
            
            print(f"Character set contains {len(characters)} characters")
            
            # Generate .cst file
            print(f"Generating .cst file...")
            self._generate_cst_file(out_dir, output_font_name, characters)
            
            # Create font renderer
            print(f"Loading font: {font_path}")
            font_renderer = FontRenderer(font_path, self.config)
            
            # Calculate dimensions (this may update self.config.font_size)
            rows, cols = self._calculate_dimensions(font_renderer)
            print(f"Character dimensions: {rows}x{cols}")
            
            # Update font renderer with recalculated font_size
            # This ensures glyphs are rendered at the correct size to fit in the backSize box
            font_renderer.update_font_size(self.config.font_size)
            
            # Adjust dimensions for rotation
            rows, cols = self._adjust_dimensions_for_rotation(rows, cols)
            print(f"Adjusted dimensions (after rotation): {rows}x{cols}")
            
            # Create font header (use base font_name, not output_font_name)
            # The header should contain just the font name like "HarmonyOS", not "HarmonyOS_32_r2"
            header = self._create_font_header(font_name, len(characters))
            
            # Generate font file
            print(f"Generating font file...")
            font_file_path = out_dir / f"{output_font_name}{FONT_FILE_EXTENSION}"
            result = self._generate_font_file(
                font_file_path,
                header,
                characters,
                index_map,
                font_renderer,
                rows,
                cols
            )
            
            if result != ErrorCode.SUCCESS.value:
                return result
            
            print(f"Font file generated: {font_file_path}")
            
            # Generate NotSupportedChars.txt if there are failed characters
            if self.failed_characters:
                print(f"Generating NotSupportedChars.txt ({len(self.failed_characters)} failed characters)...")
                self._generate_not_supported_chars_file(out_dir, output_font_name)
            
            print("Font generation completed successfully")
            return ErrorCode.SUCCESS.value
            
        except Exception as e:
            print(f"Error during font generation: {e}")
            import traceback
            traceback.print_exc()
            return ErrorCode.FILE_WRITE_ERROR.value
    
    def _generate_character_set(self, exe_dir: Path) -> Tuple[List[int], Dict[int, int]]:
        """
        Generate character set from CodePage files.
        
        Processes all configured CodePage files, mapping files, symbol files,
        and custom Unicode ranges to generate a complete character set.
        
        Args:
            exe_dir: Executable directory (contains CodePage folder)
            
        Returns:
            Tuple of (characters, index_map) where:
                - characters: Sorted list of Unicode code points
                - index_map: Dictionary mapping Unicode to index
        """
        # Use CodePageProcessor to process all character sources
        characters, index_map = CodePageProcessor.process_codepages(
            exe_dir=exe_dir,
            codepage_files=self.config.codepages,
            mapping_files=self.config.mappings,
            symbol_files=self.config.symbols,
            custom_ranges=self.config.custom_ranges
        )
        
        return characters, index_map
    
    def _generate_cst_file(self, out_dir: Path, font_name: str, characters: List[int]) -> None:
        """
        Generate .cst character set file.
        
        Writes a text file containing all Unicode code points in the character set,
        one per line, in hexadecimal format (0xXXXX).
        
        Args:
            out_dir: Output directory
            font_name: Font name
            characters: List of Unicode code points
            
        Requirements: 8.1, 8.2, 8.3
        """
        cst_file_path = out_dir / f"{font_name}{CST_FILE_EXTENSION}"
        
        try:
            with open(cst_file_path, 'w', encoding='utf-8') as f:
                for unicode_value in characters:
                    # Write in hexadecimal format with 0x prefix and 4 digits
                    f.write(f"0x{unicode_value:04X}\n")
        except Exception as e:
            print(f"Warning: Failed to write .cst file: {e}")
    
    def _calculate_dimensions(self, font_renderer: FontRenderer) -> Tuple[int, int]:
        """
        Calculate character bitmap dimensions.
        
        Calculates the dimensions based on font metrics and configuration.
        If rvd mode is enabled, uses proportional calculation based on
        font ascender/descender ratio.
        
        This method also updates self.config.back_size and self.config.font_size
        to match C++ behavior for header generation.
        
        Args:
            font_renderer: Font renderer instance
            
        Returns:
            Tuple of (rows, cols) representing character dimensions
            
        Requirements: 5.5, 10.12
        """
        # Get font metrics
        font_metrics = font_renderer.get_font_metrics()
        
        if self.config.rvd:
            # RVD mode: Calculate back_size based on font metrics
            # back_size = fontSize * (ascender - descender) / units_per_em
            if font_metrics.units_per_em > 0:
                ascender = abs(font_metrics.ascender)
                descender = abs(font_metrics.descender)
                total_height = ascender + descender
                
                # Calculate proportional back_size
                import math
                self.config.back_size = math.ceil(self.config.font_size * total_height / font_metrics.units_per_em)
                
                # Use back_size as height
                rows = self.config.back_size
            else:
                # Fallback to font_size if units_per_em is invalid
                rows = self.config.font_size
                self.config.back_size = self.config.font_size
        else:
            # Normal mode (non-RVD): Match C++ behavior
            # C++ does: backSize = fontSize; fontSize = fontSize * units_per_em / (ascender - descender)
            if font_metrics.units_per_em > 0:
                ascender = abs(font_metrics.ascender)
                descender = abs(font_metrics.descender)
                total_height = ascender + descender
                
                if total_height > 0:
                    # Store original fontSize as backSize
                    original_font_size = self.config.font_size
                    self.config.back_size = original_font_size
                    
                    # Recalculate fontSize based on font metrics (this becomes the 'size' field in header)
                    # C++ formula: fontSize = fontSize * units_per_em / (ascender - descender)
                    self.config.font_size = int(original_font_size * font_metrics.units_per_em / total_height)
                    
                    # Use back_size as height
                    rows = self.config.back_size
                else:
                    # Fallback
                    rows = self.config.font_size
                    self.config.back_size = self.config.font_size
            else:
                # Fallback
                rows = self.config.font_size
                self.config.back_size = self.config.font_size
        
        # Width is typically the same as height for square characters
        # But we use back_size as the base width (matching C++ behavior)
        cols = self.config.back_size
        
        return rows, cols
    
    def _adjust_dimensions_for_rotation(self, rows: int, cols: int) -> Tuple[int, int]:
        """
        Adjust dimensions for rotation and byte alignment.
        
        Swaps dimensions if rotation is 90° or 270°, then ensures both
        dimensions are multiples of 8 for proper byte alignment.
        
        Args:
            rows: Original row count
            cols: Original column count
            
        Returns:
            Tuple of (adjusted_rows, adjusted_cols) with byte alignment
            
        Requirements: 5.5
        """
        from .constants import ROTATION_90, ROTATION_270
        
        # Swap dimensions for 90° and 270° rotations
        if self.config.rotation in (ROTATION_90, ROTATION_270):
            rows, cols = cols, rows
        
        # Adjust for byte alignment (multiples of 8)
        adjusted_rows, adjusted_cols = ImageProcessor.adjust_dimensions_for_byte_alignment(rows, cols)
        
        return adjusted_rows, adjusted_cols
    
    def _create_font_header(self, font_name: str, char_count: int) -> BitmapFontHeader:
        """
        Create bitmap font header.
        
        Args:
            font_name: Font name
            char_count: Number of characters in the font
            
        Returns:
            BitmapFontHeader instance
        """
        return BitmapFontHeader(font_name, self.config, char_count)
    
    def _generate_font_file(
        self,
        font_file_path: Path,
        header: BitmapFontHeader,
        characters: List[int],
        index_map: Dict[int, int],
        font_renderer: FontRenderer,
        rows: int,
        cols: int
    ) -> int:
        """
        Generate the binary font file.
        
        This method:
        1. Creates the binary font file with header
        2. Writes the index area
        3. Renders each character and writes its data
        4. Updates crop mode indices if needed
        
        Args:
            font_file_path: Path to output font file
            header: Font file header
            characters: List of Unicode code points
            index_map: Unicode to index mapping
            font_renderer: Font renderer instance
            rows: Character height
            cols: Character width
            
        Returns:
            Error code (0 for success)
            
        Requirements: 7.1-7.5, 6.1-6.5
        """
        import numpy as np
        
        try:
            # Open binary font file for writing
            with BinaryFontWriter(font_file_path, header) as writer:
                
                # Prepare index data structure
                if self.config.crop:
                    # Crop mode: Will update indices as we write characters
                    index_data = {}
                elif self.config.index_method == INDEX_METHOD_ADDRESS:
                    # Address mode: Store character indices (not file offsets)
                    # The index maps Unicode value -> character index in sequence
                    index_data = {}
                else:
                    # Offset mode: Will build list of (unicode, offset) pairs
                    index_data = []
                
                # Write initial index area (will be updated for crop mode)
                writer.write_index_area(index_data)
                
                # Track character index for address mode (non-crop)
                char_index = 0
                
                # Process each character
                for unicode_value in characters:
                    try:
                        # Check if character exists in font
                        if not font_renderer.check_character_valid(unicode_value):
                            self.failed_characters.append(
                                (unicode_value, "Character not in font")
                            )
                            continue
                        
                        # Render the character
                        bitmap, metrics = font_renderer.render_character(unicode_value)
                        
                        # Resize bitmap to target dimensions
                        # Create a canvas of the target size
                        canvas = np.zeros((rows, cols), dtype=np.uint8)
                        
                        # Calculate position to place the glyph (matching C++ logic)
                        glyph_height, glyph_width = bitmap.shape
                        
                        # Calculate baseline height (matching C++ CvxText.cpp line 175)
                        # baseline_height = abs(ascender) * backSize / (ascender - descender)
                        font_metrics = font_renderer.get_font_metrics()
                        baseline_height = round(
                            abs(font_metrics.ascender) * self.config.back_size / 
                            (font_metrics.ascender - font_metrics.descender)
                        )
                        
                        # X position (matching C++ logic, lines 177-189)
                        # Use bearing_x (slot->left) for horizontal positioning
                        if metrics.bearing_x < 0:
                            x_offset = 0
                        elif metrics.bearing_x + glyph_width > cols:
                            x_offset = cols - glyph_width
                        else:
                            x_offset = metrics.bearing_x
                        
                        # Y position (matching C++ logic, lines 191-203)
                        # pos.y = baseline_height - slot->top + rows
                        # In numpy, we need to calculate the top-left corner
                        y_top = baseline_height - metrics.bearing_y
                        if y_top < 0:
                            y_offset = 0
                        elif y_top + glyph_height > rows:
                            y_offset = rows - glyph_height
                        else:
                            y_offset = y_top
                        
                        # Ensure we don't exceed canvas bounds
                        x_offset = max(0, min(x_offset, cols - 1))
                        y_offset = max(0, min(y_offset, rows - 1))
                        
                        copy_height = min(glyph_height, rows - y_offset)
                        copy_width = min(glyph_width, cols - x_offset)
                        
                        if copy_height > 0 and copy_width > 0:
                            canvas[y_offset:y_offset+copy_height, x_offset:x_offset+copy_width] = \
                                bitmap[:copy_height, :copy_width]
                        
                        # Apply gamma correction if configured
                        if self.config.gamma != 1.0:
                            canvas = ImageProcessor.apply_gamma(canvas, self.config.gamma)
                        
                        # Apply rotation if configured
                        if self.config.rotation != 0:
                            canvas = ImageProcessor.rotate_image(canvas, self.config.rotation)
                        
                        # Ensure dimensions match after rotation
                        # (rotation might have changed dimensions)
                        if canvas.shape != (rows, cols):
                            # Pad to target dimensions
                            canvas = ImageProcessor.pad_image_for_alignment(canvas)
                            # Crop or pad to exact dimensions
                            temp_canvas = np.zeros((rows, cols), dtype=np.uint8)
                            copy_h = min(canvas.shape[0], rows)
                            copy_w = min(canvas.shape[1], cols)
                            temp_canvas[:copy_h, :copy_w] = canvas[:copy_h, :copy_w]
                            canvas = temp_canvas
                        
                        # Handle cropping if enabled (matching C++ logic)
                        crop_info = None
                        canvas_to_pack = canvas
                        
                        if self.config.crop:
                            # Detect top empty rows (matching C++ zeroRowsCount logic)
                            top_skip = 0
                            for i in range(rows):
                                if np.any(canvas[i, :] > 0):
                                    top_skip = i
                                    break
                            else:
                                # All rows are empty - skip this character
                                continue
                            
                            # Detect bottom boundary (last non-zero row)
                            bottom_row = rows - 1
                            for i in range(rows - 1, -1, -1):
                                if np.any(canvas[i, :] > 0):
                                    bottom_row = i
                                    break
                            
                            # Calculate total glyph height (from row 0 to last non-empty row + 1)
                            # This matches C++ pos.y which is the bottom boundary position
                            glyph_height = bottom_row + 1
                            
                            # Detect actual glyph width (rightmost non-zero column)
                            glyph_width = 0
                            for j in range(cols - 1, -1, -1):
                                if np.any(canvas[:, j] > 0):
                                    glyph_width = j + 1
                                    break
                            
                            if glyph_width == 0 or glyph_height <= top_skip:
                                # No valid pixels - skip
                                continue
                            
                            # Ensure values are within valid byte range (0-255)
                            top_skip = min(top_skip, 255)
                            glyph_width = min(glyph_width, 255)
                            glyph_height = min(glyph_height, 255)
                            baseline_height_byte = min(baseline_height, 255)
                            
                            # Create crop info (matching C++ format)
                            # Byte 0: top_skip (number of leading empty rows)
                            # Byte 1: posO.y (unused, set to baseline_height for compatibility)
                            # Byte 2: glyph_width (actual width in pixels)
                            # Byte 3: glyph_height (total height from row 0 to last non-empty row)
                            crop_info = CropInfo(
                                top_skip=top_skip,
                                left_skip=baseline_height_byte,  # C++ posO.y value (unused but kept for compatibility)
                                valid_width=glyph_width,
                                valid_height=glyph_height
                            )
                            
                            # Only pack data from first non-empty row to last non-empty row
                            # and only up to glyph_width columns
                            canvas_to_pack = canvas[top_skip:glyph_height, 0:glyph_width]
                        
                        # Pack bitmap data according to render mode
                        if self.config.render_mode == RENDER_MODE_1BIT:
                            packed_data = ImageProcessor.pack_to_1bit(canvas_to_pack)
                        elif self.config.render_mode == RENDER_MODE_2BIT:
                            packed_data = ImageProcessor.pack_to_2bit(canvas_to_pack)
                        elif self.config.render_mode == RENDER_MODE_4BIT:
                            packed_data = ImageProcessor.pack_to_4bit(canvas_to_pack)
                        elif self.config.render_mode == RENDER_MODE_8BIT:
                            packed_data = ImageProcessor.pack_to_8bit(canvas_to_pack)
                        else:
                            raise ValueError(f"Invalid render mode: {self.config.render_mode}")
                        
                        # Write character data and get offset
                        offset = writer.write_character_data(
                            unicode_value,
                            packed_data,
                            metrics,
                            crop_info
                        )
                        
                        # Update index data
                        if self.config.crop:
                            # Update crop mode index with file offset
                            writer.update_crop_index(unicode_value, offset)
                        elif self.config.index_method == INDEX_METHOD_ADDRESS:
                            # Store character index (not file offset) for address mode
                            index_data[unicode_value] = char_index
                            char_index += 1
                        else:
                            # Add to offset mode list with character index
                            index_data.append((unicode_value, char_index))
                            char_index += 1
                        
                    except Exception as e:
                        # Record failed character and continue
                        self.failed_characters.append(
                            (unicode_value, f"Rendering error: {str(e)}")
                        )
                        continue
                
                # For non-crop modes, update the index area with final data
                if not self.config.crop:
                    # Seek back to index area and write final indices
                    import io
                    if hasattr(writer.file, 'seek'):
                        writer.file.seek(writer.index_area_offset)
                        
                        if self.config.index_method == INDEX_METHOD_ADDRESS:
                            # Write address mode index
                            index_bytes = bytearray(65536 * 2)
                            for unicode_val, offset in index_data.items():
                                if 0 <= unicode_val < 65536:
                                    import struct
                                    struct.pack_into('<H', index_bytes, unicode_val * 2, offset)
                            writer.file.write(index_bytes)
                        else:
                            # Write offset mode index
                            for unicode_val, offset in index_data:
                                import struct
                                writer.file.write(struct.pack('<HH', unicode_val, offset))
            
            return ErrorCode.SUCCESS.value
            
        except Exception as e:
            print(f"Error generating font file: {e}")
            import traceback
            traceback.print_exc()
            return ErrorCode.FILE_WRITE_ERROR.value
    
    def _generate_not_supported_chars_file(self, out_dir: Path, font_name: str) -> None:
        """
        Generate NotSupportedChars.txt file.
        
        Writes a text file containing all Unicode code points that failed
        to render, one per line, in hexadecimal format (0xXXXX).
        
        Args:
            out_dir: Output directory
            font_name: Font name
            
        Requirements: 9.1, 9.2, 9.3, 9.4, 9.5
        """
        if not self.failed_characters:
            # No failed characters, don't create the file
            return
        
        not_supported_file_path = out_dir / NOT_SUPPORTED_CHARS_FILE
        
        try:
            with open(not_supported_file_path, 'w', encoding='utf-8') as f:
                for unicode_value, reason in self.failed_characters:
                    # Write in hexadecimal format with 0x prefix and 4 digits
                    f.write(f"0x{unicode_value:04X}\n")
        except Exception as e:
            print(f"Warning: Failed to write NotSupportedChars.txt: {e}")
