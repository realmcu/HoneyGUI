#!/usr/bin/env python3
"""
HoneyGUI Image Converter (Modular Version)
Convert PNG/JPG/BMP images to HoneyGUI bin format with compression support.
"""

import argparse
import sys
from pathlib import Path

from converter import ImageConverter
from compress import RLECompression, FastLzCompression, YUVCompression, AdaptiveCompression


def main():
    parser = argparse.ArgumentParser(
        description='Convert images to HoneyGUI bin format',
        formatter_class=argparse.RawDescriptionHelpFormatter,
        epilog='''
Examples:
  # No compression
  %(prog)s -i input.png -o output.bin -f auto
  
  # RLE compression
  %(prog)s -i input.png -o output.bin -f rgb565 -c rle
  
  # FastLz compression
  %(prog)s -i input.png -o output.bin -f argb8888 -c fastlz
  
  # YUV compression
  %(prog)s -i photo.jpg -o photo.bin -c yuv --yuv-mode yuv422 --blur-bits 2
  
  # Adaptive (auto-select best)
  %(prog)s -i input.png -o output.bin -c adaptive
        '''
    )
    
    parser.add_argument('-i', '--input', required=True, help='Input image path')
    parser.add_argument('-o', '--output', required=True, help='Output bin path')
    parser.add_argument('-f', '--format', default='auto',
                        choices=['auto', 'rgb565', 'rgb888', 'argb8888', 'argb8565', 'a8'],
                        help='Output pixel format (default: auto)')
    parser.add_argument('-c', '--compress', default='none',
                        choices=['none', 'rle', 'fastlz', 'yuv', 'yuv-fastlz', 'adaptive'],
                        help='Compression algorithm (default: none)')
    
    # RLE options
    rle_group = parser.add_argument_group('RLE options')
    rle_group.add_argument('--rle-level', type=int, default=1, choices=[1, 2],
                          help='RLE compression level (default: 1)')
    rle_group.add_argument('--rle-run1', type=int, default=1, choices=[0, 1, 2, 3],
                          help='RLE 1st stage run length size (default: 1)')
    rle_group.add_argument('--rle-run2', type=int, default=0, choices=[0, 1, 2, 3],
                          help='RLE 2nd stage run length size (default: 0)')
    
    # YUV options
    yuv_group = parser.add_argument_group('YUV options')
    yuv_group.add_argument('--yuv-mode', default='yuv444',
                          choices=['yuv444', 'yuv422', 'yuv411'],
                          help='YUV sampling mode (default: yuv444)')
    yuv_group.add_argument('--blur-bits', type=int, default=0,
                          choices=[0, 1, 2, 4],
                          help='Blur mode - discard low bits (default: 0)')
    
    args = parser.parse_args()
    
    input_path = Path(args.input)
    output_path = Path(args.output)
    
    if not input_path.exists():
        print(f"Error: Input file not found: {input_path}")
        sys.exit(1)
    
    # Setup compression
    compressor = None
    try:
        if args.compress == 'rle':
            compressor = RLECompression(
                run_length_1=args.rle_run1,
                run_length_2=args.rle_run2,
                level=args.rle_level
            )
        elif args.compress == 'fastlz':
            compressor = FastLzCompression()
        elif args.compress == 'yuv':
            compressor = YUVCompression(
                sample_mode=args.yuv_mode,
                blur_bits=args.blur_bits,
                use_fastlz=False
            )
        elif args.compress == 'yuv-fastlz':
            compressor = YUVCompression(
                sample_mode=args.yuv_mode,
                blur_bits=args.blur_bits,
                use_fastlz=True
            )
        elif args.compress == 'adaptive':
            compressor = AdaptiveCompression(
                rle_run_length_1=args.rle_run1,
                rle_run_length_2=args.rle_run2
            )
    except ImportError as e:
        print(f"Error: {e}")
        sys.exit(1)
    
    # Convert
    try:
        converter = ImageConverter()
        converter.convert(input_path, output_path, args.format, compressor)
        
        compress_info = f" ({args.compress})" if compressor else ""
        print(f"Converted: {input_path} -> {output_path}{compress_info}")
        
        # Show selected algorithm for adaptive
        if args.compress == 'adaptive' and hasattr(compressor, 'selected_algorithm'):
            print(f"  Selected algorithm: {compressor.selected_algorithm}")
        
    except Exception as e:
        print(f"Error: {e}")
        import traceback
        traceback.print_exc()
        sys.exit(1)


if __name__ == '__main__':
    main()
