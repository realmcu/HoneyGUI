"""
Vector font generator implementation.

This module provides the VectorFontGenerator class for generating
vector font files from TrueType fonts.
"""

from pathlib import Path
from typing import Dict, List, Tuple, Any, BinaryIO, Optional
from dataclasses import dataclass

from .font_generator import FontGenerator
from .config import FontConfig
from .codepage_processor import CodePageProcessor
from .binary_writer import VectorFontHeader
from .constants import (
    CST_FILE_EXTENSION, NOT_SUPPORTED_CHARS_FILE, FONT_FILE_EXTENSION,
    INDEX_METHOD_ADDRESS, INDEX_METHOD_OFFSET, MAX_UNICODE_COUNT
)
from .errors import ErrorCode


@dataclass
class GlyphOutline:
    """
    Glyph outline data structure.
    
    Attributes:
        bbox_x0: Bounding box left
        bbox_y0: Bounding box top
        bbox_x1: Bounding box right
        bbox_y1: Bounding box bottom
        advance: Horizontal advance width
        winding_count: Number of contour rings
        winding_lengths: List of point counts for each ring
        points: List of all contour point coordinates (x, y)
    """
    bbox_x0: int
    bbox_y0: int
    bbox_x1: int
    bbox_y1: int
    advance: int
    winding_count: int
    winding_lengths: List[int]
    points: List[Tuple[int, int]]


class VectorFontGenerator(FontGenerator):
    """
    Vector font generator.
    
    This class generates vector font files from TrueType fonts with
    glyph outline data for vector rendering on devices.
    """
    
    def __init__(self, config: FontConfig):
        """
        Initialize vector font generator.
        
        Args:
            config: Font configuration object
        """
        super().__init__(config)
    
    def generate(self, exe_dir: Path, out_dir: Path) -> int:
        """
        Generate vector font file.
        
        This is the main entry point for vector font generation. It:
        1. Generates the character set from CodePage files
        2. Loads the TTF font file
        3. Generates the .cst character set file
        4. Generates the binary vector font file
        5. Generates the NotSupportedChars.txt file if needed
        
        Args:
            exe_dir: Executable directory (contains CodePage folder, etc.)
            out_dir: Output directory for generated files
            
        Returns:
            Error code (0 for success, non-zero for error)
            
        Requirements: 13.1
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
            
            # Load TTF font
            print(f"Loading font: {font_path}")
            ttf_buffer, font_info = self._load_ttf_font(font_path)
            
            # Get font metrics
            ascent, descent, line_gap = self._get_font_metrics(font_info)
            print(f"Font metrics: ascent={ascent}, descent={descent}, line_gap={line_gap}")
            
            # Create font header (use base font_name, not output_font_name)
            # The header should contain just the font name like "HarmonyOS", not "HarmonyOS_32_r2"
            header = VectorFontHeader(
                font_name,
                self.config,
                len(characters),
                ascent,
                descent,
                line_gap
            )
            
            # Generate font file
            print(f"Generating vector font file...")
            font_file_path = out_dir / f"{output_font_name}{FONT_FILE_EXTENSION}"
            result = self._generate_vector_font_file(
                font_file_path,
                header,
                characters,
                index_map,
                font_info
            )
            
            if result != ErrorCode.SUCCESS.value:
                return result
            
            print(f"Vector font file generated: {font_file_path}")
            
            # Generate NotSupportedChars.txt if there are failed characters
            if self.failed_characters:
                print(f"Generating NotSupportedChars.txt ({len(self.failed_characters)} failed characters)...")
                self._generate_not_supported_chars_file(out_dir, output_font_name)
            
            print("Vector font generation completed successfully")
            return ErrorCode.SUCCESS.value
            
        except Exception as e:
            print(f"Error during vector font generation: {e}")
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
        """
        cst_file_path = out_dir / f"{font_name}{CST_FILE_EXTENSION}"
        
        try:
            with open(cst_file_path, 'w', encoding='utf-8') as f:
                for unicode_value in characters:
                    # Write in hexadecimal format with 0x prefix and 4 digits
                    f.write(f"0x{unicode_value:04X}\n")
        except Exception as e:
            print(f"Warning: Failed to write .cst file: {e}")
    
    def _load_ttf_font(self, font_path: str) -> Tuple[bytes, Any]:
        """
        Load TTF font file.
        
        Uses fonttools to load and parse the TrueType font file.
        
        Args:
            font_path: Path to the TTF font file
            
        Returns:
            Tuple of (ttf_buffer, font_info) where:
                - ttf_buffer: TTF file bytes
                - font_info: fontTools TTFont object
                
        Requirements: 13.2
        """
        from fontTools.ttLib import TTFont
        
        # Read the font file into a buffer
        with open(font_path, 'rb') as f:
            ttf_buffer = f.read()
        
        # Load the font using fontTools
        font_info = TTFont(font_path)
        
        return ttf_buffer, font_info
    
    def _get_font_metrics(self, font_info: Any) -> Tuple[int, int, int]:
        """
        Get font metrics from font info.
        
        Args:
            font_info: fontTools TTFont object
            
        Returns:
            Tuple of (ascent, descent, line_gap)
        """
        # Get the 'hhea' table which contains font metrics
        hhea = font_info['hhea']
        
        # Extract metrics
        ascent = hhea.ascent
        descent = hhea.descent
        line_gap = hhea.lineGap
        
        return ascent, descent, line_gap
    
    def _extract_glyph_outline(self, font_info: Any, unicode_value: int) -> Optional[GlyphOutline]:
        """
        Extract glyph outline data.
        
        Extracts the outline data for a glyph including bounding box,
        advance width, and contour points.
        
        Args:
            font_info: fontTools TTFont object
            unicode_value: Unicode code point
            
        Returns:
            GlyphOutline object or None if glyph doesn't exist
            
        Requirements: 13.2, 13.6, 13.7
        """
        from fontTools.pens.recordingPen import RecordingPen
        
        try:
            # Get the glyph set
            glyph_set = font_info.getGlyphSet()
            
            # Get the cmap table to map Unicode to glyph name
            cmap = font_info.getBestCmap()
            if not cmap or unicode_value not in cmap:
                return None
            
            glyph_name = cmap[unicode_value]
            
            # Check if glyph exists
            if glyph_name not in glyph_set:
                return None
            
            # Get the glyph
            glyph = glyph_set[glyph_name]
            
            # Get advance width
            advance = int(glyph.width)
            
            # Use a recording pen to capture the outline
            pen = RecordingPen()
            glyph.draw(pen)
            
            # Get bounding box
            # Try to get from glyf table first (for TrueType outlines)
            try:
                glyf_table = font_info['glyf']
                glyf_glyph = glyf_table[glyph_name]
                if hasattr(glyf_glyph, 'xMin'):
                    bbox_x0 = glyf_glyph.xMin
                    bbox_y0 = glyf_glyph.yMin
                    bbox_x1 = glyf_glyph.xMax
                    bbox_y1 = glyf_glyph.yMax
                else:
                    # Empty glyph
                    bbox_x0 = bbox_y0 = bbox_x1 = bbox_y1 = 0
            except:
                # Fallback: calculate from pen recording
                bbox_x0 = bbox_y0 = bbox_x1 = bbox_y1 = 0
                if pen.value:
                    all_points = []
                    for op, args in pen.value:
                        if op in ('moveTo', 'lineTo'):
                            all_points.append(args[0])
                        elif op == 'qCurveTo':
                            all_points.extend(args)
                        elif op == 'curveTo':
                            all_points.extend(args)
                    
                    if all_points:
                        xs = [p[0] for p in all_points]
                        ys = [p[1] for p in all_points]
                        bbox_x0 = int(min(xs))
                        bbox_y0 = int(min(ys))
                        bbox_x1 = int(max(xs))
                        bbox_y1 = int(max(ys))
            
            # Process the recording to extract contours
            contours = []
            current_contour = []
            
            for op, args in pen.value:
                if op == 'moveTo':
                    # Start a new contour
                    if current_contour:
                        contours.append(current_contour)
                    current_contour = [args[0]]
                elif op == 'lineTo':
                    current_contour.append(args[0])
                elif op == 'qCurveTo':
                    # Quadratic Bezier curve - will be flattened later
                    current_contour.extend(args)
                elif op == 'curveTo':
                    # Cubic Bezier curve - will be flattened later
                    current_contour.extend(args)
                elif op == 'closePath':
                    # Close the contour
                    if current_contour:
                        contours.append(current_contour)
                        current_contour = []
            
            # Add any remaining contour
            if current_contour:
                contours.append(current_contour)
            
            # Flatten curves to get polygon points
            flattened_contours = self._flatten_curves(contours)
            
            # Build winding data
            winding_count = len(flattened_contours)
            winding_lengths = [len(contour) for contour in flattened_contours]
            
            # Flatten all points into a single list
            all_points = []
            for contour in flattened_contours:
                all_points.extend(contour)
            
            return GlyphOutline(
                bbox_x0=int(bbox_x0),
                bbox_y0=int(bbox_y0),
                bbox_x1=int(bbox_x1),
                bbox_y1=int(bbox_y1),
                advance=advance,
                winding_count=winding_count,
                winding_lengths=winding_lengths,
                points=all_points
            )
            
        except Exception as e:
            print(f"Error extracting glyph outline for U+{unicode_value:04X}: {e}")
            return None
    
    def _flatten_curves(
        self,
        contours: List[List[Tuple[float, float]]],
        tolerance: float = 1.0
    ) -> List[List[Tuple[int, int]]]:
        """
        Flatten Bezier curves to polygon point sequences.
        
        Converts quadratic and cubic Bezier curves in the glyph outline
        to a series of line segments (polygon approximation).
        
        For TrueType fonts, the curves are already decomposed by fontTools
        into line segments, so we just need to convert to integer coordinates
        and ensure proper formatting.
        
        Args:
            contours: List of contours, each containing curve points
            tolerance: Maximum deviation tolerance for curve flattening
            
        Returns:
            List of contours with flattened points (integer coordinates)
            
        Requirements: 13.5
        """
        flattened_contours = []
        
        for contour in contours:
            if not contour:
                continue
            
            # Convert float coordinates to integers
            flattened_contour = []
            for point in contour:
                x, y = point
                flattened_contour.append((int(round(x)), int(round(y))))
            
            # Remove duplicate consecutive points
            if len(flattened_contour) > 1:
                deduplicated = [flattened_contour[0]]
                for i in range(1, len(flattened_contour)):
                    if flattened_contour[i] != flattened_contour[i-1]:
                        deduplicated.append(flattened_contour[i])
                flattened_contour = deduplicated
            
            if flattened_contour:
                flattened_contours.append(flattened_contour)
        
        return flattened_contours
    
    def _generate_vector_font_file(
        self,
        font_file_path: Path,
        header: VectorFontHeader,
        characters: List[int],
        index_map: Dict[int, int],
        font_info: Any
    ) -> int:
        """
        Generate the binary vector font file.
        
        This method:
        1. Creates the binary font file with header
        2. Writes the index area
        3. Extracts each glyph outline and writes its data
        4. Updates indices as needed
        
        Args:
            font_file_path: Path to output font file
            header: Vector font file header
            characters: List of Unicode code points
            index_map: Unicode to index mapping
            font_info: fontTools TTFont object
            
        Returns:
            Error code (0 for success)
            
        Requirements: 13.10, 13.11
        """
        import struct
        
        try:
            with open(font_file_path, 'wb') as f:
                # Write header
                header_bytes = header.to_bytes()
                f.write(header_bytes)
                
                # Record index area offset
                index_area_offset = f.tell()
                
                # Prepare index area based on index method
                if self.config.index_method == INDEX_METHOD_ADDRESS:
                    # Address mode: 65536 entries of 32-bit addresses
                    # Initialize all to 0
                    index_bytes = bytearray(MAX_UNICODE_COUNT * 4)
                    f.write(index_bytes)
                else:
                    # Offset mode: Will write (unicode, offset) pairs
                    # Reserve space for N entries (6 bytes each: 2 bytes unicode + 4 bytes offset)
                    offset_entries = []
                    # We'll write this later after processing glyphs
                
                # Record data area offset
                data_area_offset = f.tell()
                
                # Process each character
                glyph_offsets = {}
                
                for unicode_value in characters:
                    try:
                        # Extract glyph outline
                        outline = self._extract_glyph_outline(font_info, unicode_value)
                        
                        if outline is None:
                            # Glyph doesn't exist, skip it
                            self.failed_characters.append(
                                (unicode_value, "Glyph not in font")
                            )
                            continue
                        
                        # Record current offset
                        current_offset = f.tell()
                        glyph_offsets[unicode_value] = current_offset
                        
                        # Write glyph data
                        success = self._write_single_glyph(f, outline)
                        
                        if not success:
                            self.failed_characters.append(
                                (unicode_value, "Failed to write glyph data")
                            )
                            continue
                        
                    except Exception as e:
                        self.failed_characters.append(
                            (unicode_value, f"Error processing glyph: {str(e)}")
                        )
                        continue
                
                # Update index area
                f.seek(index_area_offset)
                
                if self.config.index_method == INDEX_METHOD_ADDRESS:
                    # Address mode: Write 32-bit offsets for each unicode value
                    index_bytes = bytearray(MAX_UNICODE_COUNT * 4)
                    for unicode_val, offset in glyph_offsets.items():
                        if 0 <= unicode_val < MAX_UNICODE_COUNT:
                            struct.pack_into('<I', index_bytes, unicode_val * 4, offset)
                    f.write(index_bytes)
                else:
                    # Offset mode: Write (unicode, offset) pairs
                    for unicode_val in sorted(glyph_offsets.keys()):
                        offset = glyph_offsets[unicode_val]
                        # Write 2 bytes unicode + 4 bytes offset
                        f.write(struct.pack('<HI', unicode_val, offset))
            
            return ErrorCode.SUCCESS.value
            
        except Exception as e:
            print(f"Error generating vector font file: {e}")
            import traceback
            traceback.print_exc()
            return ErrorCode.FILE_WRITE_ERROR.value
    
    def _write_single_glyph(self, file: BinaryIO, glyph_outline: GlyphOutline) -> bool:
        """
        Write single glyph data to file.
        
        Writes the glyph outline data including bounding box, advance width,
        winding count, and all contour points.
        
        The glyph data format:
        - bbox_x0 (2 bytes, signed short)
        - bbox_y0 (2 bytes, signed short)
        - bbox_x1 (2 bytes, signed short)
        - bbox_y1 (2 bytes, signed short)
        - advance (2 bytes, unsigned short)
        - winding_count (2 bytes, unsigned short)
        - For each winding:
            - point_count (2 bytes, unsigned short)
        - For each point in all windings:
            - x (2 bytes, signed short)
            - y (2 bytes, signed short)
        
        Args:
            file: Binary file object
            glyph_outline: Glyph outline data
            
        Returns:
            True if successful, False otherwise
            
        Requirements: 13.6, 13.7, 13.8, 13.9
        """
        import struct
        
        try:
            # Write bounding box (4 signed shorts)
            file.write(struct.pack('<hhhh',
                glyph_outline.bbox_x0,
                glyph_outline.bbox_y0,
                glyph_outline.bbox_x1,
                glyph_outline.bbox_y1
            ))
            
            # Write advance width (unsigned short)
            file.write(struct.pack('<H', glyph_outline.advance))
            
            # Write winding count (unsigned short)
            file.write(struct.pack('<H', glyph_outline.winding_count))
            
            # Write point count for each winding (unsigned shorts)
            for point_count in glyph_outline.winding_lengths:
                file.write(struct.pack('<H', point_count))
            
            # Write all points (signed shorts for x and y)
            for x, y in glyph_outline.points:
                file.write(struct.pack('<hh', x, y))
            
            return True
            
        except Exception as e:
            print(f"Error writing glyph data: {e}")
            return False
    
    def _generate_not_supported_chars_file(self, out_dir: Path, font_name: str) -> None:
        """
        Generate NotSupportedChars.txt file.
        
        Writes a text file containing all Unicode code points that failed
        to render, one per line, in hexadecimal format (0xXXXX).
        
        Args:
            out_dir: Output directory
            font_name: Font name
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
