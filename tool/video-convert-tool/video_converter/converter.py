"""视频转换器主模块 - 使用 FFmpeg"""

import os
import tempfile
import shutil
from typing import Optional, Callable

from .models import VideoInfo, ConversionResult, OutputFormat
from .parser import VideoParser
from .ffmpeg_builder import FFmpegBuilder
from .ffmpeg_executor import FFmpegExecutor
from .postprocess import PostProcessor
from .exceptions import VideoConverterError, FFmpegNotFoundError


class VideoConverter:
    """
    视频转换器主类
    
    使用 FFmpeg 将视频转换为 MJPEG、AVI-MJPEG 和 H264 格式。
    """
    
    def __init__(
        self, 
        progress_callback: Optional[Callable[[int, int], None]] = None
    ):
        """
        初始化转换器
        
        Args:
            progress_callback: 进度回调函数，接收 (current_frame, total_frames)
        """
        self.progress_callback = progress_callback
        self._parser = VideoParser()
        self._builder = FFmpegBuilder()
        self._executor = FFmpegExecutor(progress_callback=progress_callback)
    
    def get_video_info(self, input_path: str) -> VideoInfo:
        """
        获取视频信息
        
        Args:
            input_path: 输入视频文件路径
            
        Returns:
            VideoInfo 对象
        """
        return self._parser.parse(input_path)

    def convert(
        self, 
        input_path: str, 
        output_path: str, 
        output_format: OutputFormat,
        frame_rate: Optional[float] = None,
        quality: int = 1
    ) -> ConversionResult:
        """
        执行视频转换
        
        Args:
            input_path: 输入视频文件路径
            output_path: 输出文件路径
            output_format: 输出格式
            frame_rate: 目标帧率，None 表示保持原帧率
            quality: MJPEG/AVI 质量 (1-31，1最高，默认1) 或 H264 CRF 值
            
        Returns:
            ConversionResult 对象
            
        Raises:
            FileNotFoundError: 输入文件不存在
            VideoConverterError: 转换失败
            FFmpegNotFoundError: FFmpeg 未安装
        """
        try:
            # 获取视频信息
            video_info = self.get_video_info(input_path)
            
            # 确定输出帧率
            target_fps = frame_rate if frame_rate else video_info.frame_rate
            
            # 根据格式选择转换方法
            if output_format == OutputFormat.MJPEG:
                return self._convert_to_mjpeg(
                    input_path, output_path, video_info, target_fps, quality
                )
            elif output_format == OutputFormat.AVI_MJPEG:
                return self._convert_to_avi_mjpeg(
                    input_path, output_path, video_info, target_fps, quality
                )
            elif output_format == OutputFormat.H264:
                return self._convert_to_h264(
                    input_path, output_path, video_info, target_fps, quality
                )
            else:
                raise ValueError(f"不支持的输出格式: {output_format}")
                
        except (VideoConverterError, FileNotFoundError, FFmpegNotFoundError):
            raise
        except Exception as e:
            raise VideoConverterError(f"转换失败: {e}")

    def _convert_to_mjpeg(
        self,
        input_path: str,
        output_path: str,
        video_info: VideoInfo,
        target_fps: float,
        quality: int
    ) -> ConversionResult:
        """转换为 MJPEG 格式"""
        # 创建临时目录存放 JPEG 帧
        temp_dir = tempfile.mkdtemp(prefix="mjpeg_frames_")
        
        try:
            # 构建 ffmpeg 命令
            cmd = self._builder.build_mjpeg_frames_cmd(
                input_path=input_path,
                output_dir=temp_dir,
                frame_rate=target_fps,
                quality=quality
            )
            
            # 执行 ffmpeg 命令
            self._executor.execute(cmd, total_frames=video_info.frame_count)
            
            # 计算实际帧数
            frame_files = [f for f in os.listdir(temp_dir) if f.endswith('.jpg')]
            frame_count = len(frame_files)
            
            # 调用后处理脚本打包
            PostProcessor.process_mjpeg(temp_dir, output_path)
            
            return ConversionResult(
                success=True,
                input_path=input_path,
                output_path=output_path,
                output_format=OutputFormat.MJPEG,
                frame_count=frame_count,
                frame_rate=target_fps,
                quality=quality
            )
            
        finally:
            # 清理临时目录
            if os.path.exists(temp_dir):
                shutil.rmtree(temp_dir)

    def _convert_to_avi_mjpeg(
        self,
        input_path: str,
        output_path: str,
        video_info: VideoInfo,
        target_fps: float,
        quality: int
    ) -> ConversionResult:
        """转换为 AVI-MJPEG 格式"""
        # 创建临时 AVI 文件
        temp_avi = output_path + ".temp.avi"
        
        try:
            # 构建 ffmpeg 命令
            cmd = self._builder.build_avi_cmd(
                input_path=input_path,
                output_path=temp_avi,
                frame_rate=target_fps,
                quality=quality
            )
            
            # 执行 ffmpeg 命令
            self._executor.execute(cmd, total_frames=video_info.frame_count)
            
            # 调用后处理脚本进行 8 字节对齐
            PostProcessor.process_avi(temp_avi, output_path)
            
            return ConversionResult(
                success=True,
                input_path=input_path,
                output_path=output_path,
                output_format=OutputFormat.AVI_MJPEG,
                frame_count=video_info.frame_count,
                frame_rate=target_fps,
                quality=quality
            )
            
        finally:
            # 清理临时文件
            if os.path.exists(temp_avi):
                os.remove(temp_avi)

    def _convert_to_h264(
        self,
        input_path: str,
        output_path: str,
        video_info: VideoInfo,
        target_fps: float,
        crf: int
    ) -> ConversionResult:
        """转换为 H264 格式"""
        # 创建临时 H264 文件
        temp_h264 = output_path + ".temp.h264"
        
        try:
            # 构建 ffmpeg 命令
            cmd = self._builder.build_h264_cmd(
                input_path=input_path,
                output_path=temp_h264,
                frame_rate=target_fps,
                crf=crf
            )
            
            # 执行 ffmpeg 命令
            self._executor.execute(cmd, total_frames=video_info.frame_count)
            
            # 调用后处理脚本添加头部
            PostProcessor.process_h264(temp_h264, output_path, target_fps)
            
            return ConversionResult(
                success=True,
                input_path=input_path,
                output_path=output_path,
                output_format=OutputFormat.H264,
                frame_count=video_info.frame_count,
                frame_rate=target_fps,
                quality=crf
            )
            
        finally:
            # 清理临时文件
            if os.path.exists(temp_h264):
                os.remove(temp_h264)
