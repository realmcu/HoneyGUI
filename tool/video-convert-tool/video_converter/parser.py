"""视频解析器模块"""

import os
import cv2
from .models import VideoInfo
from .exceptions import VideoFormatError


class VideoParser:
    """视频解析器 - 解析视频文件的元数据"""
    
    # 支持的视频扩展名
    SUPPORTED_EXTENSIONS = {
        '.mp4', '.avi', '.mkv', '.mov', '.wmv', '.flv', '.webm',
        '.MP4', '.AVI', '.MKV', '.MOV', '.WMV', '.FLV', '.WEBM'
    }
    
    def parse(self, input_path: str) -> VideoInfo:
        """
        解析视频文件，返回视频信息
        
        Args:
            input_path: 输入视频文件路径
            
        Returns:
            VideoInfo 对象，包含视频的宽度、高度、帧率、帧数等信息
            
        Raises:
            FileNotFoundError: 文件不存在
            VideoFormatError: 不支持的视频格式或无法打开
        """
        # 检查文件是否存在
        if not os.path.exists(input_path):
            raise FileNotFoundError(f"文件不存在: {input_path}")
        
        # 检查文件扩展名
        _, ext = os.path.splitext(input_path)
        if ext not in self.SUPPORTED_EXTENSIONS:
            raise VideoFormatError(f"不支持的视频格式: {ext}")
        
        # 使用 OpenCV 打开视频
        cap = cv2.VideoCapture(input_path)
        
        if not cap.isOpened():
            raise VideoFormatError(f"无法打开视频文件: {input_path}")
        
        try:
            # 获取视频属性
            width = int(cap.get(cv2.CAP_PROP_FRAME_WIDTH))
            height = int(cap.get(cv2.CAP_PROP_FRAME_HEIGHT))
            frame_rate = cap.get(cv2.CAP_PROP_FPS)
            frame_count = int(cap.get(cv2.CAP_PROP_FRAME_COUNT))
            
            # 获取编码器信息
            fourcc = int(cap.get(cv2.CAP_PROP_FOURCC))
            codec = "".join([chr((fourcc >> 8 * i) & 0xFF) for i in range(4)])
            
            # 计算时长
            duration = frame_count / frame_rate if frame_rate > 0 else 0.0
            
            # 验证数据有效性
            if width <= 0 or height <= 0:
                raise VideoFormatError(f"无效的视频尺寸: {width}x{height}")
            
            if frame_rate <= 0:
                raise VideoFormatError(f"无效的帧率: {frame_rate}")
            
            if frame_count <= 0:
                raise VideoFormatError(f"无效的帧数: {frame_count}")
            
            return VideoInfo(
                width=width,
                height=height,
                frame_rate=frame_rate,
                frame_count=frame_count,
                duration=duration,
                codec=codec.strip(),
                file_path=os.path.abspath(input_path)
            )
        finally:
            cap.release()
