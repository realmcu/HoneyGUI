"""
CodePage processor for parsing and processing character encoding files.

This module handles parsing of various CodePage file formats (NLS, CP, CST)
and generates character sets for font generation.
"""

from pathlib import Path
from typing import Set, List, Tuple, Dict
import struct

from .constants import MAX_UNICODE_COUNT, INVALID_UNICODE
from .errors import FileNotFoundError, ParseError


class CodePageProcessor:
    """Processor for CodePage files and character set generation."""
    
    @staticmethod
    def parse_nls_file(file_path: Path) -> Set[int]:
        """
        Parse NLS format CodePage file.
        
        NLS files are binary files with a specific structure:
        - Header with magic byte (0x0D)
        - CodePage ID and encoding type
        - Invalid data markers
        - Prefix buffer
        - Character mapping tables
        - Unicode to CodePage mapping
        
        Args:
            file_path: Path to the NLS file
            
        Returns:
            Set of Unicode code points found in the file
            
        Raises:
            FileNotFoundError: If the file doesn't exist
            ParseError: If the file format is invalid
        """
        if not file_path.exists():
            raise FileNotFoundError("CodePage", str(file_path))
        
        try:
            with open(file_path, 'rb') as f:
                data = f.read()
            
            if len(data) < 2:
                raise ParseError("CodePage", str(file_path), "File too short")
            
            # Check magic byte
            if data[0] != 0x0D:
                raise ParseError("CodePage", str(file_path), "Invalid magic byte")
            
            pos = 2
            
            # Read CodePage ID
            if pos + 2 > len(data):
                raise ParseError("CodePage", str(file_path), "Unexpected end of file")
            cp_id = struct.unpack('<H', data[pos:pos+2])[0]
            pos += 2
            
            # Read encoding type (1 = single byte, 2 = double byte)
            if pos + 2 > len(data):
                raise ParseError("CodePage", str(file_path), "Unexpected end of file")
            encode_type = struct.unpack('<H', data[pos:pos+2])[0]
            pos += 2
            
            # Read invalid data markers (4 uint16_t values)
            if pos + 8 > len(data):
                raise ParseError("CodePage", str(file_path), "Unexpected end of file")
            invalid_data = struct.unpack('<4H', data[pos:pos+8])
            pos += 8
            
            # Skip prefix buffer (12 bytes)
            pos += 12
            
            # Read table length
            if pos + 2 > len(data):
                raise ParseError("CodePage", str(file_path), "Unexpected end of file")
            table_len = struct.unpack('<H', data[pos:pos+2])[0]
            pos += 2
            
            # Read cp2uc table (256 entries)
            if pos + 512 > len(data):
                raise ParseError("CodePage", str(file_path), "Unexpected end of file")
            cp2uc = struct.unpack('<256H', data[pos:pos+512])
            pos += 512
            
            # Skip additional tables based on table_len
            if table_len == 0x103:
                pos += 6
            elif table_len == 0x203:
                pos += 2 + 512 + 512 + 4  # Skip oem2uc tables
            else:
                # Skip oem2uc and cp2uc2 tables
                pos += 2 + 512 + 512 + 2 + (table_len - 515) * 2 + 2
            
            # Read uc2cp mapping
            unicode_set = set()
            
            if encode_type == 1:
                # Single byte encoding
                if pos + 0x10000 > len(data):
                    raise ParseError("CodePage", str(file_path), "Unexpected end of file")
                
                for i in range(0x10000):
                    uc2cp_value = data[pos + i]
                    if uc2cp_value != invalid_data[0]:
                        unicode_set.add(i)
            
            elif encode_type == 2:
                # Double byte encoding
                if pos + 0x20000 > len(data):
                    raise ParseError("CodePage", str(file_path), "Unexpected end of file")
                
                for i in range(0x10000):
                    uc2cp_value = struct.unpack('<H', data[pos + i*2:pos + i*2 + 2])[0]
                    if uc2cp_value != invalid_data[0]:
                        unicode_set.add(i)
            
            # Add the invalid data character mapping if valid
            if invalid_data[0] < 256 and cp2uc[invalid_data[0]] < MAX_UNICODE_COUNT:
                unicode_set.add(cp2uc[invalid_data[0]])
            
            return unicode_set
            
        except struct.error as e:
            raise ParseError("CodePage", str(file_path), f"Struct unpacking error: {e}")
        except Exception as e:
            raise ParseError("CodePage", str(file_path), str(e))
    
    @staticmethod
    def parse_cp_file(file_path: Path) -> Set[int]:
        """
        Parse custom CP format CodePage file.
        
        CP files can be in two formats:
        1. HEX format: Lines starting with "0x" followed by hex values
        2. STRING format: UTF-8 encoded text with characters
        
        Args:
            file_path: Path to the CP file
            
        Returns:
            Set of Unicode code points found in the file
            
        Raises:
            FileNotFoundError: If the file doesn't exist
            ParseError: If the file format is invalid
        """
        if not file_path.exists():
            raise FileNotFoundError("CodePage", str(file_path))
        
        try:
            unicode_set = set()
            
            with open(file_path, 'r', encoding='utf-8', errors='ignore') as f:
                lines = f.readlines()
            
            if not lines:
                return unicode_set
            
            # Detect file type by checking first few non-empty lines
            file_type = None
            for line in lines[:5]:
                line = line.strip()
                if not line:
                    continue
                
                if line.startswith('0x'):
                    file_type = 'HEX'
                    break
                else:
                    file_type = 'STRING'
                    break
            
            if file_type is None:
                return unicode_set
            
            # Process based on detected type
            if file_type == 'HEX':
                # HEX format: parse hex values
                for line in lines:
                    line = line.strip()
                    if not line:
                        continue
                    
                    # Split line into tokens
                    tokens = line.split()
                    for token in tokens:
                        if token.startswith('0x') or token.startswith('0X'):
                            try:
                                code_point = int(token, 16)
                                if 0 <= code_point < MAX_UNICODE_COUNT:
                                    unicode_set.add(code_point)
                            except ValueError:
                                # Skip invalid hex values
                                pass
            
            else:  # STRING format
                # Process each line as UTF-8 text
                for line in lines:
                    # Add all characters from the line
                    for char in line:
                        code_point = ord(char)
                        if 0 <= code_point < MAX_UNICODE_COUNT:
                            unicode_set.add(code_point)
            
            return unicode_set
            
        except Exception as e:
            raise ParseError("CodePage", str(file_path), str(e))
    
    @staticmethod
    def parse_cst_file(file_path: Path) -> Set[int]:
        """
        Parse CST (Character Set) file.
        
        CST files are binary files containing a sequence of uint16_t values,
        each representing a Unicode code point.
        
        Args:
            file_path: Path to the CST file
            
        Returns:
            Set of Unicode code points found in the file
            
        Raises:
            FileNotFoundError: If the file doesn't exist
            ParseError: If the file format is invalid
        """
        if not file_path.exists():
            raise FileNotFoundError("CodePage", str(file_path))
        
        try:
            with open(file_path, 'rb') as f:
                data = f.read()
            
            if len(data) % 2 != 0:
                raise ParseError("CodePage", str(file_path), 
                               "File size is not a multiple of 2 bytes")
            
            unicode_set = set()
            num_entries = len(data) // 2
            
            for i in range(num_entries):
                code_point = struct.unpack('<H', data[i*2:i*2+2])[0]
                if code_point < MAX_UNICODE_COUNT:
                    unicode_set.add(code_point)
            
            return unicode_set
            
        except struct.error as e:
            raise ParseError("CodePage", str(file_path), f"Struct unpacking error: {e}")
        except Exception as e:
            raise ParseError("CodePage", str(file_path), str(e))
    
    @staticmethod
    def process_codepages(exe_dir: Path,
                         codepage_files: List[str],
                         mapping_files: List[str],
                         symbol_files: List[str],
                         custom_ranges: List[Tuple[int, int]]) -> Tuple[List[int], Dict[int, int]]:
        """
        Process all CodePage files and generate character set.
        
        This method:
        1. Parses all specified CodePage files (NLS, CP, CST formats)
        2. Merges character sets from all sources
        3. Adds custom Unicode ranges
        4. Generates sorted character list and index mapping
        
        Args:
            exe_dir: Executable directory containing CodePage folder
            codepage_files: List of NLS CodePage file names (in CodePage/ folder)
            mapping_files: List of mapping file paths (CP format)
            symbol_files: List of symbol file paths (CP format)
            custom_ranges: List of (start, count) tuples for custom Unicode ranges
            
        Returns:
            Tuple of:
            - characters: Sorted list of Unicode code points
            - index_map: Dictionary mapping Unicode code point to index
            
        Raises:
            FileNotFoundError: If any required file is not found
            ParseError: If any file cannot be parsed
        """
        unicode_set = set()
        
        # Parse NLS CodePage files
        codepage_dir = exe_dir / "CodePage"
        for cp_file in codepage_files:
            file_path = codepage_dir / cp_file
            chars = CodePageProcessor.parse_nls_file(file_path)
            unicode_set.update(chars)
        
        # Parse mapping files (CP format)
        for mapping_file in mapping_files:
            file_path = Path(mapping_file)
            chars = CodePageProcessor.parse_cp_file(file_path)
            unicode_set.update(chars)
        
        # Parse symbol files (CP format)
        for symbol_file in symbol_files:
            file_path = Path(symbol_file)
            chars = CodePageProcessor.parse_cp_file(file_path)
            unicode_set.update(chars)
        
        # Add custom Unicode ranges
        for start, count in custom_ranges:
            if start + count > MAX_UNICODE_COUNT:
                # Log warning but continue
                print(f"Warning: Custom range [{start}, {start + count}) exceeds maximum Unicode value")
                count = MAX_UNICODE_COUNT - start
            
            for code_point in range(start, start + count):
                if code_point < MAX_UNICODE_COUNT:
                    unicode_set.add(code_point)
        
        # Generate sorted character list
        characters = sorted(list(unicode_set))
        
        # Generate index mapping
        index_map = {char: idx for idx, char in enumerate(characters)}
        
        return characters, index_map
