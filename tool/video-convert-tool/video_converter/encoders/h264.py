"""H264 编码器模块"""

import os
import cv2
import numpy as np
from typing import Optional


class H264Encoder:
    """
    H264 编码器 - 将帧数据编码为 H264 格式
    
    使用 OpenCV VideoWriter 进行编码，确保尺寸 16 像素对齐。
    """
    
    # H264 FourCC 编码
    FOURCC_H264 = cv2.VideoWriter_fourcc('H', '2', '6', '4')
    FOURCC_AVC1 = cv2.VideoWriter_fourcc('a', 'v', 'c', '1')
    FOURCC_X264 = cv2.VideoWriter_fourcc('X', '2', '6', '4')
    
    def __init__(
        self, 
        output_path: str,
        width: int, 
        height: int, 
        fps: float, 
        crf: int = 23
    ):
        """
        初始化编码器
        
        Args:
            output_path: 输出文件路径
            width: 视频宽度（将对齐到 16）
            height: 视频高度（将对齐到 16）
            fps: 帧率
            crf: CRF 质量值 (0-51)，默认 23
        """
        self.original_width = width
        self.original_height = height
        self.fps = fps
        self.crf = crf
        self.output_path = output_path
        
        # 计算 16 对齐后的尺寸
        self.aligned_width = self._align_to_16(width)
        self.aligned_height = self._align_to_16(height)
        
        # 计算填充量
        self.pad_right = self.aligned_width - width
        self.pad_bottom = self.aligned_height - height
        
        self._frame_count = 0
        self._writer: Optional[cv2.VideoWriter] = None
        
        # 创建输出目录
        output_dir = os.path.dirname(output_path)
        if output_dir:
            os.makedirs(output_dir, exist_ok=True)
        
        # 尝试不同的 FourCC 编码
        self._init_writer()
    
    def _init_writer(self) -> None:
        """初始化 VideoWriter"""
        fourcc_options = [self.FOURCC_H264, self.FOURCC_AVC1, self.FOURCC_X264]
        
        for fourcc in fourcc_options:
            self._writer = cv2.VideoWriter(
                self.output_path,
                fourcc,
                self.fps,
                (self.aligned_width, self.aligned_height),
                isColor=True
            )
            
            if self._writer.isOpened():
                return
        
        raise IOError(f"无法创建 H264 编码器，请确保系统支持 H264 编码")
    
    @staticmethod
    def _align_to_16(value: int) -> int:
        """将值对齐到 16 的倍数"""
        if value % 16 == 0:
            return value
        return ((value // 16) + 1) * 16
    
    def encode_frame(self, frame: np.ndarray) -> None:
        """
        编码单帧
        
        Args:
            frame: BGR 格式的帧数据
        """
        if self._writer is None:
            raise RuntimeError("编码器未初始化")
        
        # 如果需要填充
        if self.pad_right > 0 or self.pad_bottom > 0:
            frame = self._pad_frame(frame)
        
        self._writer.write(frame)
        self._frame_count += 1
    
    def _pad_frame(self, frame: np.ndarray) -> np.ndarray:
        """
        填充帧到对齐尺寸（黑色填充）
        
        Args:
            frame: 原始帧
            
        Returns:
            填充后的帧
        """
        if frame.shape[1] != self.original_width or frame.shape[0] != self.original_height:
            # 先调整到原始尺寸
            frame = cv2.resize(frame, (self.original_width, self.original_height))
        
        # 创建黑色背景
        padded = np.zeros(
            (self.aligned_height, self.aligned_width, 3), 
            dtype=np.uint8
        )
        
        # 将原始帧复制到左上角
        padded[:self.original_height, :self.original_width] = frame
        
        return padded
    
    def flush(self) -> None:
        """刷新编码器缓冲区"""
        # OpenCV VideoWriter 在 release 时自动刷新
        pass
    
    def close(self) -> None:
        """关闭编码器"""
        if self._writer:
            self._writer.release()
            self._writer = None
    
    def get_frame_count(self) -> int:
        """获取已编码的帧数"""
        return self._frame_count
    
    def get_aligned_size(self) -> tuple:
        """获取对齐后的尺寸"""
        return self.aligned_width, self.aligned_height
    
    def get_original_size(self) -> tuple:
        """获取原始尺寸"""
        return self.original_width, self.original_height
    
    def __enter__(self):
        return self
    
    def __exit__(self, exc_type, exc_val, exc_tb):
        self.close()
        return False
