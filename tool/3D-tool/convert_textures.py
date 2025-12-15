#!/usr/bin/env python3
"""
Batch convert PNG textures to binary format for 3D models

This script converts all PNG files in the current directory to .bin format
using the image_converter_new.py tool.
"""

import os
import sys
import subprocess
from pathlib import Path


def find_image_converter():
    """Find the image_converter_new.py script"""
    # Try relative path from this script
    script_dir = Path(__file__).parent
    converter_path = script_dir.parent / 'image-convert-tool' / 'image_converter_new.py'
    
    if converter_path.exists():
        return str(converter_path)
    
    # Try current directory
    converter_path = Path('image_converter_new.py')
    if converter_path.exists():
        return str(converter_path)
    
    # Try image-convert-tool directory
    converter_path = Path('../image-convert-tool/image_converter_new.py')
    if converter_path.exists():
        return str(converter_path)
    
    return None


def convert_textures(directory='.', format_type='auto'):
    """
    Convert all PNG files in directory to binary format
    
    Args:
        directory: Directory to search for PNG files (default: current directory)
        format_type: Output format type (default: 'auto')
    """
    # Find converter script
    converter = find_image_converter()
    if not converter:
        print("Error: image_converter_new.py not found!")
        print("Please ensure the image-convert-tool directory exists.")
        return False
    
    print(f"Using converter: {converter}")
    print(f"Scanning directory: {directory}")
    print("=" * 60)
    
    # Find all PNG files
    png_files = list(Path(directory).glob('*.png'))
    
    if not png_files:
        print("No PNG files found in the current directory.")
        return True
    
    print(f"Found {len(png_files)} PNG file(s)")
    print()
    
    success_count = 0
    failed_count = 0
    
    for png_file in png_files:
        bin_file = png_file.with_suffix('.bin')
        
        print(f"Converting: {png_file.name} -> {bin_file.name}...", end=' ')
        
        try:
            result = subprocess.run(
                [
                    sys.executable,
                    converter,
                    '-i', str(png_file),
                    '-o', str(bin_file),
                    '-f', format_type
                ],
                capture_output=True,
                text=True,
                timeout=30
            )
            
            if result.returncode == 0:
                print("✓ OK")
                success_count += 1
            else:
                print("✗ FAILED")
                if result.stderr:
                    print(f"  Error: {result.stderr.strip()}")
                failed_count += 1
        
        except subprocess.TimeoutExpired:
            print("✗ TIMEOUT")
            failed_count += 1
        
        except Exception as e:
            print(f"✗ ERROR: {e}")
            failed_count += 1
    
    print()
    print("=" * 60)
    print(f"Conversion complete:")
    print(f"  Success: {success_count}/{len(png_files)}")
    print(f"  Failed:  {failed_count}/{len(png_files)}")
    
    return failed_count == 0


def main():
    """Main entry point"""
    import argparse
    
    parser = argparse.ArgumentParser(
        description='Batch convert PNG textures to binary format',
        formatter_class=argparse.RawDescriptionHelpFormatter,
        epilog="""
Examples:
  # Convert all PNG files in current directory
  python convert_textures.py
  
  # Convert PNG files in specific directory
  python convert_textures.py -d ./textures
  
  # Specify output format
  python convert_textures.py -f RGB565
        """
    )
    
    parser.add_argument(
        '-d', '--directory',
        default='.',
        help='Directory containing PNG files (default: current directory)'
    )
    
    parser.add_argument(
        '-f', '--format',
        default='auto',
        choices=['auto', 'RGB565', 'ARGB8565', 'ARGB8888'],
        help='Output format (default: auto)'
    )
    
    args = parser.parse_args()
    
    # Convert textures
    success = convert_textures(args.directory, args.format)
    
    sys.exit(0 if success else 1)


if __name__ == '__main__':
    main()
