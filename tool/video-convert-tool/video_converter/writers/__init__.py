"""写入器模块"""

from .mjpeg import MjpegWriter
from .avi import AviWriter
from .h264 import H264Writer

__all__ = ["MjpegWriter", "AviWriter", "H264Writer"]
