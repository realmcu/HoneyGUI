"""
视频转换工具 - 纯 Python 实现

支持将常见视频格式转换为 MJPEG、AVI-MJPEG 和 H264 格式。

使用示例:
    from video_converter import VideoConverter, OutputFormat
    
    converter = VideoConverter()
    
    # 获取视频信息
    info = converter.get_video_info("input.mp4")
    print(f"分辨率: {info.width}x{info.height}")
    
    # 转换视频
    result = converter.convert(
        input_path="input.mp4",
        output_path="output.mjpeg",
        output_format=OutputFormat.MJPEG,
        frame_rate=25,
        quality=85
    )
"""

from .models import VideoInfo, ConversionResult, OutputFormat, MjpegFrame
from .exceptions import (
    VideoConverterError,
    VideoFormatError,
    EncodingError,
    PostProcessError,
)
from .converter import VideoConverter
from .parser import VideoParser

__version__ = "1.0.0"
__all__ = [
    "VideoConverter",
    "VideoParser",
    "VideoInfo",
    "ConversionResult", 
    "OutputFormat",
    "MjpegFrame",
    "VideoConverterError",
    "VideoFormatError",
    "EncodingError",
    "PostProcessError",
]
