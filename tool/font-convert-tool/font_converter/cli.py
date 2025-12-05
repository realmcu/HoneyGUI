"""
Command-line interface for the font converter.

This module provides the CLI for parsing command-line arguments and
orchestrating the font generation process.
"""

import argparse
import sys
from pathlib import Path
from typing import Optional

from .config import ConfigManager, FontConfig
from .constants import (
    DEFAULT_JSON_CONFIG,
    DEFAULT_INI_CONFIG,
    VALID_RENDER_MODES,
    VALID_ROTATIONS,
)
from .errors import ErrorCode, FontConverterError


class FontConverterCLI:
    """Command-line interface for the font converter."""
    
    def __init__(self):
        """Initialize the CLI with argument parser."""
        self.parser = argparse.ArgumentParser(
            prog='font_converter',
            description='Convert TrueType fonts to custom binary formats for embedded systems',
            formatter_class=argparse.RawDescriptionHelpFormatter,
            epilog="""
Examples:
  # Use default FontConfig.json in current directory
  font_converter
  
  # Use specific JSON config file
  font_converter path/to/config.json
  
  # Override JSON config with command-line arguments
  font_converter --font Arial.ttf --size 16 --render-mode 8
  
  # Use command-line arguments only (no JSON)
  font_converter --font Arial.ttf --size 24 --render-mode 4 --output ./output
            """
        )
        self._setup_arguments()
    
    def _setup_arguments(self):
        """Set up command-line arguments."""
        # Positional argument for JSON config file (optional)
        self.parser.add_argument(
            'config',
            nargs='?',
            default=None,
            help=f'Path to JSON configuration file (default: {DEFAULT_JSON_CONFIG})'
        )
        
        # Font configuration arguments
        self.parser.add_argument(
            '--font',
            type=str,
            help='Path to TrueType font file (.ttf or .ttc)'
        )
        
        self.parser.add_argument(
            '--size',
            type=int,
            help='Font size in pixels'
        )
        
        self.parser.add_argument(
            '--render-mode',
            type=int,
            choices=sorted(VALID_RENDER_MODES),
            help='Render mode: 1 (1-bit), 2 (2-bit), 4 (4-bit), 8 (8-bit)'
        )
        
        self.parser.add_argument(
            '--output',
            type=str,
            help='Output directory path'
        )
        
        # Character set arguments
        self.parser.add_argument(
            '--codepage',
            type=str,
            nargs='+',
            help='CodePage file paths (space-separated)'
        )
        
        # Rendering options
        self.parser.add_argument(
            '--rotation',
            type=int,
            choices=sorted(VALID_ROTATIONS),
            help='Rotation: 0 (0°), 1 (90° CW), 2 (90° CCW), 3 (180°)'
        )
        
        self.parser.add_argument(
            '--gamma',
            type=float,
            help='Gamma correction value (default: 1.0)'
        )
        
        self.parser.add_argument(
            '--bold',
            action='store_true',
            help='Enable bold rendering'
        )
        
        self.parser.add_argument(
            '--italic',
            action='store_true',
            help='Enable italic rendering'
        )
        
        self.parser.add_argument(
            '--crop',
            action='store_true',
            help='Enable character cropping mode'
        )
        
        self.parser.add_argument(
            '--rvd',
            action='store_true',
            help='Use reverse vertical direction mode for font metrics calculation'
        )
    
    def parse_args(self, args=None) -> argparse.Namespace:
        """
        Parse command-line arguments.
        
        Args:
            args: List of argument strings (default: sys.argv)
            
        Returns:
            Parsed arguments as argparse.Namespace
        """
        return self.parser.parse_args(args)
    
    def run(self, args=None) -> int:
        """
        Execute the font converter with given arguments.
        
        This method:
        1. Parses command-line arguments
        2. Loads and merges configurations (JSON + INI + CLI)
        3. Validates the configuration
        4. Creates the appropriate font generator (bitmap or vector)
        5. Executes font generation
        6. Handles errors and returns appropriate exit codes
        
        Args:
            args: List of argument strings (default: sys.argv)
            
        Returns:
            Exit code (0 for success, non-zero for error)
            
        Requirements: 10.1-10.15, 12.1-12.5
        """
        try:
            # Parse command-line arguments
            parsed_args = self.parse_args(args)
            
            # Determine JSON config file path
            json_path = parsed_args.config
            if json_path is None:
                # Check if default config exists
                if Path(DEFAULT_JSON_CONFIG).exists():
                    json_path = DEFAULT_JSON_CONFIG
                elif not self._has_required_cli_args(parsed_args):
                    # No JSON and no required CLI args
                    print(f"Error: No configuration file found and required arguments not provided.")
                    print(f"Either provide {DEFAULT_JSON_CONFIG} or use --font, --size, and --render-mode")
                    return ErrorCode.JSON_FILE_NOT_EXIST.value
            
            # Load configurations
            if json_path:
                print(f"Loading configuration from: {json_path}")
                json_config = ConfigManager.load_json_config(json_path)
            else:
                # Create minimal config from CLI args
                print("Using command-line arguments only")
                json_config = FontConfig(
                    font=parsed_args.font,
                    font_size=parsed_args.size,
                    render_mode=parsed_args.render_mode
                )
            
            # Load INI config if it exists
            ini_config = ConfigManager.load_ini_config(DEFAULT_INI_CONFIG)
            if Path(DEFAULT_INI_CONFIG).exists():
                print(f"Loading INI settings from: {DEFAULT_INI_CONFIG}")
            
            # Merge configurations (CLI args have highest priority)
            final_config = ConfigManager.merge_configs(json_config, ini_config, parsed_args)
            
            # Validate configuration
            print("Validating configuration...")
            ConfigManager.validate_config(final_config)
            print("Configuration validated successfully")
            
            # Display configuration summary
            print("\n" + "="*60)
            print("Font Converter Configuration")
            print("="*60)
            print(f"Font File:      {final_config.font}")
            print(f"Font Size:      {final_config.font_size}")
            print(f"Render Mode:    {final_config.render_mode}-bit")
            print(f"Output Format:  {'Vector' if final_config.output_format == 1 else 'Bitmap'}")
            print(f"Output Folder:  {final_config.output_folder or 'current directory'}")
            print(f"Bold:           {final_config.bold}")
            print(f"Italic:         {final_config.italic}")
            print(f"Rotation:       {final_config.rotation}")
            print(f"Gamma:          {final_config.gamma}")
            print(f"Crop:           {final_config.crop}")
            print(f"RVD Mode:       {final_config.rvd}")
            print(f"Index Method:   {'Address' if final_config.index_method == 0 else 'Offset'}")
            print("="*60 + "\n")
            
            # Determine executable directory (current directory or fontDirctionary subdirectory)
            exe_dir = Path.cwd()
            # Check if fontDirctionary subdirectory exists (for CodePage files)
            if (exe_dir / "fontDirctionary").exists():
                exe_dir = exe_dir / "fontDirctionary"
            
            # Determine output directory
            if final_config.output_folder:
                out_dir = Path(final_config.output_folder)
            else:
                out_dir = Path.cwd()
            
            # Create appropriate font generator based on output format
            if final_config.output_format == 1:
                # Vector font generation
                print("Creating vector font generator...")
                from .vector_generator import VectorFontGenerator
                generator = VectorFontGenerator(final_config)
            else:
                # Bitmap font generation (default)
                print("Creating bitmap font generator...")
                from .bitmap_generator import BitmapFontGenerator
                generator = BitmapFontGenerator(final_config)
            
            # Execute font generation
            print("\nStarting font generation...\n")
            result = generator.generate(exe_dir, out_dir)
            
            # Check result
            if result == ErrorCode.SUCCESS.value:
                print("\n" + "="*60)
                print("Font generation completed successfully!")
                print("="*60)
                
                # Display failed characters summary if any
                failed_chars = generator.get_failed_characters()
                if failed_chars:
                    print(f"\nNote: {len(failed_chars)} character(s) could not be rendered")
                    print(f"See NotSupportedChars.txt for details")
            else:
                print("\n" + "="*60)
                print(f"Font generation failed with error code: {result}")
                print("="*60)
            
            return result
            
        except FontConverterError as e:
            print(f"\nError: {e}", file=sys.stderr)
            return e.error_code.value
        except Exception as e:
            print(f"\nUnexpected error: {e}", file=sys.stderr)
            import traceback
            traceback.print_exc()
            return ErrorCode.ARGUMENT_ERROR.value
    
    def _has_required_cli_args(self, args: argparse.Namespace) -> bool:
        """
        Check if required CLI arguments are provided.
        
        Args:
            args: Parsed command-line arguments
            
        Returns:
            True if all required arguments are present
        """
        return (
            args.font is not None and
            args.size is not None and
            args.render_mode is not None
        )


def main():
    """Main entry point for the command-line interface."""
    cli = FontConverterCLI()
    sys.exit(cli.run())


if __name__ == '__main__':
    main()
