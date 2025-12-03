"""
Adaptive Compression
自适应压缩 - 自动选择 RLE 或 FastLz
"""
from .base import CompressionAlgorithm
from .rle import RLECompression
from .fastlz import FastLzCompression, HAS_FASTLZ


class AdaptiveCompression(CompressionAlgorithm):
    """Adaptive compression - choose best between RLE and FastLz"""
    
    def __init__(self, rle_run_length_1=3, rle_run_length_2=3):
        """
        Args:
            rle_run_length_1: RLE 1阶 Run Length
            rle_run_length_2: RLE 2阶 Run Length
        """
        self.rle = RLECompression(rle_run_length_1, rle_run_length_2, level=1)
        
        if HAS_FASTLZ:
            self.fastlz = FastLzCompression()
        else:
            self.fastlz = None
        
        self.selected_algorithm = None
    
    def compress(self, pixel_data, width, height, pixel_bytes):
        """Try both algorithms and select the best"""
        # Try RLE
        rle_data, rle_offsets, rle_params = self.rle.compress(
            pixel_data, width, height, pixel_bytes
        )
        rle_size = len(rle_data)
        
        # Try FastLz if available
        if self.fastlz:
            try:
                fastlz_data, fastlz_offsets, fastlz_params = self.fastlz.compress(
                    pixel_data, width, height, pixel_bytes
                )
                fastlz_size = len(fastlz_data)
                
                # Select best
                if fastlz_size < rle_size:
                    self.selected_algorithm = 'fastlz'
                    return fastlz_data, fastlz_offsets, fastlz_params
            except Exception as e:
                print(f"FastLz failed: {e}, using RLE")
        
        # Use RLE
        self.selected_algorithm = 'rle'
        return rle_data, rle_offsets, rle_params
    
    def get_algorithm_type(self):
        """Return the algorithm type that was selected"""
        if self.selected_algorithm == 'fastlz':
            return self.fastlz.get_algorithm_type()
        else:
            return self.rle.get_algorithm_type()
