"""数据模型定义"""

from dataclasses import dataclass
from enum import Enum
from typing import Optional


class OutputFormat(Enum):
    """输出格式枚举"""
    MJPEG = "mjpeg"
    AVI_MJPEG = "avi_mjpeg"
    H264 = "h264"


@dataclass
class VideoInfo:
    """视频信息"""
    width: int
    height: int
    frame_rate: float
    frame_count: int
    duration: float  # 秒
    codec: str
    file_path: str


@dataclass
class ConversionResult:
    """转换结果"""
    success: bool
    input_path: str
    output_path: str
    output_format: OutputFormat
    frame_count: int
    frame_rate: float
    quality: int
    error_message: Optional[str] = None


@dataclass
class MjpegFrame:
    """MJPEG 帧数据"""
    index: int
    offset: int
    size: int
    data: bytes
