"""FFmpeg 命令执行器"""

import os
import re
import subprocess
import sys
from typing import Callable, List, Optional

from .exceptions import FFmpegError, FFmpegNotFoundError


class FFmpegExecutor:
    """FFmpeg 命令执行器 - 执行 ffmpeg 命令并处理输出"""
    
    def __init__(self, progress_callback: Optional[Callable[[int, int], None]] = None):
        """
        初始化执行器
        
        Args:
            progress_callback: 进度回调函数，接收 (current_frame, total_frames)
        """
        self.progress_callback = progress_callback
    
    @staticmethod
    def check_ffmpeg_available() -> bool:
        """
        检查 ffmpeg 是否可用
        
        Returns:
            True 如果 ffmpeg 可用，否则 False
        """
        ffmpeg_cmd = "ffmpeg.exe" if sys.platform == "win32" else "ffmpeg"
        try:
            result = subprocess.run(
                [ffmpeg_cmd, "-version"],
                capture_output=True,
                text=True,
                timeout=10
            )
            return result.returncode == 0
        except (FileNotFoundError, subprocess.TimeoutExpired):
            return False
    
    @staticmethod
    def check_ffprobe_available() -> bool:
        """
        检查 ffprobe 是否可用
        
        Returns:
            True 如果 ffprobe 可用，否则 False
        """
        ffprobe_cmd = "ffprobe.exe" if sys.platform == "win32" else "ffprobe"
        try:
            result = subprocess.run(
                [ffprobe_cmd, "-version"],
                capture_output=True,
                text=True,
                timeout=10
            )
            return result.returncode == 0
        except (FileNotFoundError, subprocess.TimeoutExpired):
            return False

    
    @staticmethod
    def ensure_ffmpeg_available() -> None:
        """
        确保 ffmpeg 可用，否则抛出异常
        
        Raises:
            FFmpegNotFoundError: ffmpeg 未安装或不在 PATH 中
        """
        if not FFmpegExecutor.check_ffmpeg_available():
            raise FFmpegNotFoundError(
                "ffmpeg 未安装或不在系统 PATH 中。"
                "请安装 ffmpeg 并确保其在系统 PATH 中可访问。"
            )
    
    @staticmethod
    def ensure_ffprobe_available() -> None:
        """
        确保 ffprobe 可用，否则抛出异常
        
        Raises:
            FFmpegNotFoundError: ffprobe 未安装或不在 PATH 中
        """
        if not FFmpegExecutor.check_ffprobe_available():
            raise FFmpegNotFoundError(
                "ffprobe 未安装或不在系统 PATH 中。"
                "请安装 ffmpeg（包含 ffprobe）并确保其在系统 PATH 中可访问。"
            )
    
    def execute(
        self, 
        cmd: List[str], 
        total_frames: int = 0
    ) -> subprocess.CompletedProcess:
        """
        执行 ffmpeg 命令
        
        Args:
            cmd: 命令参数列表
            total_frames: 总帧数（用于进度计算）
            
        Returns:
            执行结果
            
        Raises:
            FFmpegNotFoundError: ffmpeg 未安装
            FFmpegError: ffmpeg 执行失败
        """
        # 确保 ffmpeg 可用
        self.ensure_ffmpeg_available()
        
        # 添加覆盖输出文件的参数（避免交互式提示）
        if cmd[0] in ("ffmpeg", "ffmpeg.exe") and "-y" not in cmd:
            cmd = [cmd[0], "-y"] + cmd[1:]
        
        try:
            if self.progress_callback and total_frames > 0:
                return self._execute_with_progress(cmd, total_frames)
            else:
                return self._execute_simple(cmd)
        except FileNotFoundError:
            raise FFmpegNotFoundError(
                "ffmpeg 未安装或不在系统 PATH 中。"
                "请安装 ffmpeg 并确保其在系统 PATH 中可访问。"
            )

    
    def _execute_simple(self, cmd: List[str]) -> subprocess.CompletedProcess:
        """
        简单执行命令（无进度回调）
        
        Args:
            cmd: 命令参数列表
            
        Returns:
            执行结果
            
        Raises:
            FFmpegError: ffmpeg 执行失败
        """
        result = subprocess.run(
            cmd,
            capture_output=True,
            text=True
        )
        
        if result.returncode != 0:
            raise FFmpegError(
                f"ffmpeg 执行失败 (返回码: {result.returncode})\n"
                f"命令: {' '.join(cmd)}\n"
                f"错误输出: {result.stderr}"
            )
        
        return result
    
    def _execute_with_progress(
        self, 
        cmd: List[str], 
        total_frames: int
    ) -> subprocess.CompletedProcess:
        """
        执行命令并报告进度
        
        Args:
            cmd: 命令参数列表
            total_frames: 总帧数
            
        Returns:
            执行结果
            
        Raises:
            FFmpegError: ffmpeg 执行失败
        """
        # 使用 Popen 以便实时读取输出
        process = subprocess.Popen(
            cmd,
            stdout=subprocess.PIPE,
            stderr=subprocess.PIPE,
            text=True
        )
        
        stderr_output = []
        last_frame = 0
        
        # ffmpeg 将进度信息输出到 stderr
        for line in process.stderr:
            stderr_output.append(line)
            
            # 解析帧数进度 (格式: frame=  123 ...)
            frame_match = re.search(r'frame=\s*(\d+)', line)
            if frame_match:
                current_frame = int(frame_match.group(1))
                # 确保进度单调递增
                if current_frame > last_frame:
                    last_frame = current_frame
                    if self.progress_callback:
                        self.progress_callback(current_frame, total_frames)
        
        # 等待进程完成
        stdout, _ = process.communicate()
        
        # 最终进度回调
        if self.progress_callback and total_frames > 0:
            self.progress_callback(total_frames, total_frames)
        
        if process.returncode != 0:
            raise FFmpegError(
                f"ffmpeg 执行失败 (返回码: {process.returncode})\n"
                f"命令: {' '.join(cmd)}\n"
                f"错误输出: {''.join(stderr_output)}"
            )
        
        # 构造 CompletedProcess 对象
        return subprocess.CompletedProcess(
            args=cmd,
            returncode=process.returncode,
            stdout=stdout or "",
            stderr="".join(stderr_output)
        )

    
    def execute_ffprobe(self, cmd: List[str]) -> subprocess.CompletedProcess:
        """
        执行 ffprobe 命令
        
        Args:
            cmd: 命令参数列表
            
        Returns:
            执行结果
            
        Raises:
            FFmpegNotFoundError: ffprobe 未安装
            FFmpegError: ffprobe 执行失败
        """
        # 确保 ffprobe 可用
        self.ensure_ffprobe_available()
        
        try:
            result = subprocess.run(
                cmd,
                capture_output=True,
                text=True
            )
            
            if result.returncode != 0:
                raise FFmpegError(
                    f"ffprobe 执行失败 (返回码: {result.returncode})\n"
                    f"命令: {' '.join(cmd)}\n"
                    f"错误输出: {result.stderr}"
                )
            
            return result
            
        except FileNotFoundError:
            raise FFmpegNotFoundError(
                "ffprobe 未安装或不在系统 PATH 中。"
                "请安装 ffmpeg（包含 ffprobe）并确保其在系统 PATH 中可访问。"
            )
