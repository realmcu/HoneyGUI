"""视频解析器模块 - 使用 ffprobe"""

import json
import os
import subprocess
import sys
from typing import Any, Dict

from .models import VideoInfo
from .exceptions import VideoFormatError, FFmpegNotFoundError, FFmpegError


class VideoParser:
    """视频解析器 - 使用 ffprobe 解析视频文件的元数据"""
    
    def __init__(self):
        """初始化解析器"""
        self._ffprobe_cmd = "ffprobe.exe" if sys.platform == "win32" else "ffprobe"
    
    def _check_ffprobe_available(self) -> bool:
        """
        检查 ffprobe 是否可用
        
        Returns:
            True 如果 ffprobe 可用，否则 False
        """
        try:
            result = subprocess.run(
                [self._ffprobe_cmd, "-version"],
                capture_output=True,
                text=True,
                timeout=10
            )
            return result.returncode == 0
        except (FileNotFoundError, subprocess.TimeoutExpired):
            return False
    
    def _ensure_ffprobe_available(self) -> None:
        """
        确保 ffprobe 可用，否则抛出异常
        
        Raises:
            FFmpegNotFoundError: ffprobe 未安装或不在 PATH 中
        """
        if not self._check_ffprobe_available():
            raise FFmpegNotFoundError(
                "ffprobe 未安装或不在系统 PATH 中。"
                "请安装 ffmpeg（包含 ffprobe）并确保其在系统 PATH 中可访问。"
            )

    def _run_ffprobe(self, input_path: str) -> Dict[str, Any]:
        """
        运行 ffprobe 命令并返回 JSON 结果
        
        Args:
            input_path: 输入视频文件路径
            
        Returns:
            ffprobe 输出的 JSON 数据
            
        Raises:
            FFmpegNotFoundError: ffprobe 未安装
            FFmpegError: ffprobe 执行失败
            VideoFormatError: 无法解析视频格式
        """
        cmd = [
            self._ffprobe_cmd,
            "-v", "quiet",
            "-print_format", "json",
            "-show_streams",
            "-show_format",
            input_path
        ]
        
        try:
            result = subprocess.run(
                cmd,
                capture_output=True,
                text=True,
                timeout=30
            )
            
            if result.returncode != 0:
                # ffprobe 返回非零状态码通常表示无法解析文件
                # 这可能是因为文件格式不支持或文件损坏
                raise VideoFormatError(f"不支持的视频格式或无法解析: {input_path}")
            
            # 解析 JSON 输出
            try:
                data = json.loads(result.stdout)
            except json.JSONDecodeError as e:
                raise VideoFormatError(f"无法解析 ffprobe 输出: {e}")
            
            return data
            
        except FileNotFoundError:
            raise FFmpegNotFoundError(
                "ffprobe 未安装或不在系统 PATH 中。"
                "请安装 ffmpeg（包含 ffprobe）并确保其在系统 PATH 中可访问。"
            )
        except subprocess.TimeoutExpired:
            raise FFmpegError(f"ffprobe 执行超时: {input_path}")

    def _find_video_stream(self, data: Dict[str, Any]) -> Dict[str, Any]:
        """
        从 ffprobe 输出中找到视频流
        
        Args:
            data: ffprobe 输出的 JSON 数据
            
        Returns:
            视频流信息字典
            
        Raises:
            VideoFormatError: 未找到视频流
        """
        streams = data.get("streams", [])
        
        for stream in streams:
            if stream.get("codec_type") == "video":
                return stream
        
        raise VideoFormatError("文件中未找到视频流")
    
    def _parse_frame_rate(self, stream: Dict[str, Any]) -> float:
        """
        解析帧率
        
        Args:
            stream: 视频流信息
            
        Returns:
            帧率（FPS）
        """
        # 优先使用 avg_frame_rate
        frame_rate_str = stream.get("avg_frame_rate", "0/1")
        
        # 如果 avg_frame_rate 无效，尝试 r_frame_rate
        if frame_rate_str == "0/0" or frame_rate_str == "0/1":
            frame_rate_str = stream.get("r_frame_rate", "0/1")
        
        # 解析分数格式 (如 "30/1" 或 "30000/1001")
        try:
            if "/" in frame_rate_str:
                num, den = frame_rate_str.split("/")
                num, den = float(num), float(den)
                if den > 0:
                    return num / den
            else:
                return float(frame_rate_str)
        except (ValueError, ZeroDivisionError):
            pass
        
        return 0.0
    
    def _parse_frame_count(self, stream: Dict[str, Any], format_info: Dict[str, Any], 
                           frame_rate: float, duration: float) -> int:
        """
        解析帧数
        
        Args:
            stream: 视频流信息
            format_info: 格式信息
            frame_rate: 帧率
            duration: 时长
            
        Returns:
            帧数
        """
        # 优先使用 nb_frames
        nb_frames = stream.get("nb_frames")
        if nb_frames is not None:
            try:
                count = int(nb_frames)
                if count > 0:
                    return count
            except (ValueError, TypeError):
                pass
        
        # 如果没有 nb_frames，通过时长和帧率计算
        if frame_rate > 0 and duration > 0:
            return int(frame_rate * duration)
        
        return 0

    def parse(self, input_path: str) -> VideoInfo:
        """
        解析视频文件，返回视频信息
        
        使用 ffprobe 命令:
        ffprobe -v quiet -print_format json -show_streams -show_format <input>
        
        Args:
            input_path: 输入视频文件路径
            
        Returns:
            VideoInfo 对象，包含视频的宽度、高度、帧率、帧数等信息
            
        Raises:
            FileNotFoundError: 文件不存在
            VideoFormatError: 不支持的视频格式或无法打开
            FFmpegNotFoundError: ffprobe 未安装或不在 PATH 中
        """
        # 检查文件是否存在
        if not os.path.exists(input_path):
            raise FileNotFoundError(f"文件不存在: {input_path}")
        
        # 确保 ffprobe 可用
        self._ensure_ffprobe_available()
        
        # 运行 ffprobe
        data = self._run_ffprobe(input_path)
        
        # 检查是否有有效数据
        if not data or (not data.get("streams") and not data.get("format")):
            raise VideoFormatError(f"不支持的视频格式或无法解析: {input_path}")
        
        # 找到视频流
        video_stream = self._find_video_stream(data)
        format_info = data.get("format", {})
        
        # 提取视频属性
        width = video_stream.get("width", 0)
        height = video_stream.get("height", 0)
        codec = video_stream.get("codec_name", "unknown")
        
        # 解析帧率
        frame_rate = self._parse_frame_rate(video_stream)
        
        # 解析时长
        duration = 0.0
        duration_str = video_stream.get("duration") or format_info.get("duration")
        if duration_str:
            try:
                duration = float(duration_str)
            except (ValueError, TypeError):
                pass
        
        # 解析帧数
        frame_count = self._parse_frame_count(video_stream, format_info, frame_rate, duration)
        
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
            codec=codec,
            file_path=os.path.abspath(input_path)
        )
