"""
Font generator base class and implementations.

This module provides the abstract base class for font generators and
common utility methods for font generation.
"""

from abc import ABC, abstractmethod
from pathlib import Path
from typing import List, Tuple

from .config import FontConfig


class FontGenerator(ABC):
    """
    Abstract base class for font generators.
    
    This class defines the interface for font generation and provides
    common utility methods for finding font files and generating output names.
    """
    
    def __init__(self, config: FontConfig):
        """
        Initialize the font generator.
        
        Args:
            config: Font configuration object
        """
        self.config = config
        self.failed_characters: List[Tuple[int, str]] = []
    
    @abstractmethod
    def generate(self, exe_dir: Path, out_dir: Path) -> int:
        """
        Generate font file.
        
        This method must be implemented by subclasses to perform the
        actual font generation.
        
        Args:
            exe_dir: Executable directory (contains CodePage folder, etc.)
            out_dir: Output directory for generated files
            
        Returns:
            Error code (0 for success, non-zero for error)
        """
        pass
    
    def get_failed_characters(self) -> List[Tuple[int, str]]:
        """
        Get the list of characters that failed to render.
        
        Returns:
            List of (unicode_value, reason) tuples
        """
        return self.failed_characters
    
    def _find_font_path(self, exe_dir: Path) -> str:
        """
        Find the font file path.
        
        If the configured font path is absolute or exists as-is, use it.
        Otherwise, try to find it relative to the executable directory.
        
        Args:
            exe_dir: Executable directory
            
        Returns:
            Resolved font file path
        """
        font_path = Path(self.config.font)
        
        # If it's an absolute path or exists as-is, use it
        if font_path.is_absolute() or font_path.exists():
            return str(font_path)
        
        # Try relative to executable directory
        relative_path = exe_dir / font_path
        if relative_path.exists():
            return str(relative_path)
        
        # Return original path (will fail later with appropriate error)
        return self.config.font
    
    def _get_font_name(self) -> str:
        """
        Get the font name without extension.
        
        Extracts the base name from the font file path, removing the extension.
        
        Returns:
            Font name without extension
        """
        font_path = Path(self.config.font)
        return font_path.stem
    
    def _get_output_font_name(self, font_name: str) -> str:
        """
        Generate the output font file name.
        
        Creates a descriptive file name based on font name and configuration
        parameters (size, render mode, bold, italic, rotation).
        
        Args:
            font_name: Base font name (without extension)
            
        Returns:
            Output file name with configuration suffix
        """
        # Start with base font name
        output_name = font_name
        
        # Add size
        output_name += f"_{self.config.font_size}"
        
        # Add render mode
        output_name += f"_r{self.config.render_mode}"
        
        # Add bold if enabled
        if self.config.bold:
            output_name += "_b"
        
        # Add italic if enabled
        if self.config.italic:
            output_name += "_i"
        
        # Add rotation if not 0
        if self.config.rotation != 0:
            output_name += f"_rot{self.config.rotation}"
        
        return output_name
