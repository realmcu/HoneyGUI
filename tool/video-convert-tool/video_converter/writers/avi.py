"""AVI 写入器模块"""

import os
import struct
import cv2
import numpy as np
from typing import Optional


class AviWriter:
    """
    AVI 写入器 - 生成 AVI 容器（MJPEG 编码）
    
    使用 OpenCV VideoWriter 生成 AVI 文件。
    """
    
    # MJPEG FourCC 编码
    FOURCC_MJPEG = cv2.VideoWriter_fourcc('M', 'J', 'P', 'G')
    
    def __init__(
        self, 
        output_path: str, 
        width: int, 
        height: int, 
        fps: float,
        quality: int = 85
    ):
        """
        初始化 AVI 写入器
        
        Args:
            output_path: 输出文件路径
            width: 视频宽度
            height: 视频高度
            fps: 帧率
            quality: JPEG 质量 (1-100)
        """
        self.output_path = output_path
        self.width = width
        self.height = height
        self.fps = fps
        self.quality = quality
        self._frame_count = 0
        
        # 创建输出目录
        output_dir = os.path.dirname(output_path)
        if output_dir:
            os.makedirs(output_dir, exist_ok=True)
        
        # 创建 VideoWriter
        self._writer = cv2.VideoWriter(
            output_path,
            self.FOURCC_MJPEG,
            fps,
            (width, height),
            isColor=True
        )
        
        if not self._writer.isOpened():
            raise IOError(f"无法创建 AVI 文件: {output_path}")
    
    def write_frame(self, frame: np.ndarray) -> None:
        """
        写入一帧
        
        Args:
            frame: BGR 格式的帧数据
        """
        if frame.shape[1] != self.width or frame.shape[0] != self.height:
            # 调整尺寸
            frame = cv2.resize(frame, (self.width, self.height))
        
        self._writer.write(frame)
        self._frame_count += 1
    
    def write_jpeg_frame(self, jpeg_data: bytes) -> None:
        """
        写入 JPEG 帧数据
        
        Args:
            jpeg_data: JPEG 字节数据
        """
        # 解码 JPEG 为帧
        nparr = np.frombuffer(jpeg_data, np.uint8)
        frame = cv2.imdecode(nparr, cv2.IMREAD_COLOR)
        
        if frame is not None:
            self.write_frame(frame)
    
    def close(self) -> None:
        """关闭文件"""
        if self._writer:
            self._writer.release()
            self._writer = None
    
    def get_frame_count(self) -> int:
        """获取已写入的帧数"""
        return self._frame_count
    
    def __enter__(self):
        return self
    
    def __exit__(self, exc_type, exc_val, exc_tb):
        self.close()
        return False


class AviParser:
    """
    AVI 解析器 - 解析 AVI 文件结构
    
    用于验证 MJPEG 帧的 8 字节对齐。
    """
    
    @staticmethod
    def find_movi_frames(avi_path: str) -> list:
        """
        查找 AVI 文件中 movi 块的所有帧
        
        Args:
            avi_path: AVI 文件路径
            
        Returns:
            帧信息列表 [(chunk_id, offset, size, data_offset), ...]
        """
        frames = []
        
        with open(avi_path, 'rb') as f:
            # 读取 RIFF 头
            riff_id = f.read(4)
            if riff_id != b'RIFF':
                return frames
            
            riff_size = struct.unpack('<I', f.read(4))[0]
            riff_type = f.read(4)
            
            if riff_type != b'AVI ':
                return frames
            
            # 查找 movi LIST
            movi_offset = None
            movi_size = None
            
            offset = 12
            riff_end = 8 + riff_size
            
            while offset + 8 <= riff_end:
                f.seek(offset)
                chunk_id = f.read(4)
                chunk_size = struct.unpack('<I', f.read(4))[0]
                
                if chunk_id == b'LIST':
                    list_type = f.read(4)
                    if list_type == b'movi':
                        movi_offset = offset + 12  # 跳过 LIST + size + 'movi'
                        movi_size = chunk_size - 4  # 减去 'movi' 4 字节
                        break
                
                # 移动到下一个块
                pad = chunk_size % 2
                offset += 8 + chunk_size + pad
            
            if movi_offset is None:
                return frames
            
            # 解析 movi 中的帧
            pos = movi_offset
            movi_end = movi_offset + movi_size
            
            while pos + 8 <= movi_end:
                f.seek(pos)
                chunk_id = f.read(4).decode('ascii', errors='replace')
                chunk_size = struct.unpack('<I', f.read(4))[0]
                data_offset = pos + 8
                
                # 跳过 JUNK 和 LIST
                if chunk_id not in ('JUNK', 'LIST'):
                    frames.append((chunk_id, pos, chunk_size, data_offset))
                
                pad = chunk_size % 2
                pos += 8 + chunk_size + pad
        
        return frames
    
    @classmethod
    def verify_alignment(cls, avi_path: str, alignment: int = 8) -> tuple:
        """
        验证 AVI 文件中所有帧数据的对齐
        
        Args:
            avi_path: AVI 文件路径
            alignment: 对齐字节数，默认 8
            
        Returns:
            (is_aligned, failed_frames) 元组
        """
        frames = cls.find_movi_frames(avi_path)
        failed_frames = []
        
        for i, (chunk_id, offset, size, data_offset) in enumerate(frames):
            if data_offset % alignment != 0:
                failed_frames.append((i, chunk_id, data_offset))
        
        return len(failed_frames) == 0, failed_frames
