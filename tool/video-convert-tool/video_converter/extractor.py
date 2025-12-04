"""帧提取器模块"""

import os
import math
from typing import Iterator, Tuple, Optional, Callable
import cv2
import numpy as np
from .exceptions import VideoFormatError


class FrameExtractor:
    """
    帧提取器 - 从视频中提取帧数据
    
    支持帧率重采样，实现迭代器接口。
    """
    
    def __init__(
        self, 
        video_path: str, 
        target_fps: Optional[float] = None,
        progress_callback: Optional[Callable[[int, int], None]] = None
    ):
        """
        初始化帧提取器
        
        Args:
            video_path: 视频文件路径
            target_fps: 目标帧率，None 表示保持原帧率
            progress_callback: 进度回调函数，接收 (current_frame, total_frames)
            
        Raises:
            FileNotFoundError: 文件不存在
            VideoFormatError: 无法打开视频
        """
        if not os.path.exists(video_path):
            raise FileNotFoundError(f"文件不存在: {video_path}")
        
        self.video_path = video_path
        self.target_fps = target_fps
        self.progress_callback = progress_callback
        
        # 打开视频获取信息
        cap = cv2.VideoCapture(video_path)
        if not cap.isOpened():
            raise VideoFormatError(f"无法打开视频: {video_path}")
        
        try:
            self.source_fps = cap.get(cv2.CAP_PROP_FPS)
            self.source_frame_count = int(cap.get(cv2.CAP_PROP_FRAME_COUNT))
            self.width = int(cap.get(cv2.CAP_PROP_FRAME_WIDTH))
            self.height = int(cap.get(cv2.CAP_PROP_FRAME_HEIGHT))
            self.duration = self.source_frame_count / self.source_fps if self.source_fps > 0 else 0
        finally:
            cap.release()
        
        # 计算目标帧数
        if target_fps is not None and target_fps > 0:
            self.output_fps = target_fps
            self.output_frame_count = math.ceil(self.duration * target_fps)
        else:
            self.output_fps = self.source_fps
            self.output_frame_count = self.source_frame_count
    
    def get_frame_count(self) -> int:
        """获取输出总帧数"""
        return self.output_frame_count
    
    def get_output_fps(self) -> float:
        """获取输出帧率"""
        return self.output_fps
    
    def __iter__(self) -> Iterator[Tuple[int, np.ndarray]]:
        """
        迭代返回 (帧索引, 帧数据) 元组
        
        帧数据为 BGR 格式的 numpy 数组
        """
        cap = cv2.VideoCapture(self.video_path)
        if not cap.isOpened():
            raise VideoFormatError(f"无法打开视频: {self.video_path}")
        
        try:
            if self.target_fps is None or abs(self.target_fps - self.source_fps) < 0.01:
                # 不需要重采样，直接读取所有帧
                yield from self._extract_all_frames(cap)
            else:
                # 需要重采样
                yield from self._extract_resampled_frames(cap)
        finally:
            cap.release()
    
    def _extract_all_frames(self, cap: cv2.VideoCapture) -> Iterator[Tuple[int, np.ndarray]]:
        """提取所有帧（不重采样）"""
        frame_index = 0
        
        while True:
            ret, frame = cap.read()
            if not ret:
                break
            
            if self.progress_callback:
                self.progress_callback(frame_index + 1, self.output_frame_count)
            
            yield frame_index, frame
            frame_index += 1
    
    def _extract_resampled_frames(self, cap: cv2.VideoCapture) -> Iterator[Tuple[int, np.ndarray]]:
        """提取重采样后的帧"""
        # 计算每个输出帧对应的源帧时间
        for out_idx in range(self.output_frame_count):
            # 计算当前输出帧对应的时间点
            out_time = out_idx / self.output_fps
            
            # 计算对应的源帧索引
            source_frame_idx = int(out_time * self.source_fps)
            source_frame_idx = min(source_frame_idx, self.source_frame_count - 1)
            
            # 定位到源帧
            cap.set(cv2.CAP_PROP_POS_FRAMES, source_frame_idx)
            ret, frame = cap.read()
            
            if not ret:
                break
            
            if self.progress_callback:
                self.progress_callback(out_idx + 1, self.output_frame_count)
            
            yield out_idx, frame
