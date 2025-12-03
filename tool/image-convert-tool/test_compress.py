#!/usr/bin/env python3
"""
Test compression algorithms
"""
from pathlib import Path
from PIL import Image
import sys

# Add current directory to path
sys.path.insert(0, str(Path(__file__).parent))

from converter import ImageConverter
from compress import RLECompression, AdaptiveCompression

def create_test_image():
    """Create a simple test image"""
    img = Image.new('RGB', (100, 100), color='red')
    # Add some patterns
    for x in range(50):
        for y in range(50):
            img.putpixel((x, y), (0, 255, 0))  # Green square
    
    test_path = Path('/tmp/test_image.png')
    img.save(test_path)
    return test_path

def test_no_compression():
    print("Testing: No compression...")
    input_path = create_test_image()
    output_path = Path('/tmp/test_no_compress.bin')
    
    converter = ImageConverter()
    converter.convert(input_path, output_path, 'rgb565', None)
    
    size = output_path.stat().st_size
    print(f"  Output size: {size} bytes")
    return size

def test_rle_compression():
    print("Testing: RLE compression...")
    input_path = create_test_image()
    output_path = Path('/tmp/test_rle.bin')
    
    compressor = RLECompression(run_length_1=3, level=1)
    converter = ImageConverter()
    converter.convert(input_path, output_path, 'rgb565', compressor)
    
    size = output_path.stat().st_size
    print(f"  Output size: {size} bytes")
    return size

def test_adaptive_compression():
    print("Testing: Adaptive compression...")
    input_path = create_test_image()
    output_path = Path('/tmp/test_adaptive.bin')
    
    compressor = AdaptiveCompression()
    converter = ImageConverter()
    converter.convert(input_path, output_path, 'rgb565', compressor)
    
    size = output_path.stat().st_size
    print(f"  Output size: {size} bytes")
    print(f"  Selected: {compressor.selected_algorithm}")
    return size

if __name__ == '__main__':
    print("=== HoneyGUI Image Converter Test ===\n")
    
    try:
        size_none = test_no_compression()
        print()
        
        size_rle = test_rle_compression()
        ratio_rle = (1 - size_rle / size_none) * 100
        print(f"  Compression ratio: {ratio_rle:.1f}%")
        print()
        
        size_adaptive = test_adaptive_compression()
        ratio_adaptive = (1 - size_adaptive / size_none) * 100
        print(f"  Compression ratio: {ratio_adaptive:.1f}%")
        print()
        
        print("✅ All tests passed!")
        
    except Exception as e:
        print(f"❌ Test failed: {e}")
        import traceback
        traceback.print_exc()
        sys.exit(1)
