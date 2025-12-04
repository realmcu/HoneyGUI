"""MJPEG 写入器模块"""

import os
import struct
from typing import List, Optional
from ..models import MjpegFrame


class MjpegWriter:
    """
    MJPEG 写入器 - 将 JPEG 帧写入临时目录
    
    用于生成独立的 JPEG 帧文件，供后处理脚本打包。
    """
    
    def __init__(self, output_dir: str):
        """
        初始化写入器
        
        Args:
            output_dir: 输出目录路径
        """
        self.output_dir = output_dir
        self._frame_paths: List[str] = []
        
        # 创建输出目录
        os.makedirs(output_dir, exist_ok=True)
    
    def write_frame(self, frame_index: int, jpeg_data: bytes) -> str:
        """
        写入单帧
        
        Args:
            frame_index: 帧索引
            jpeg_data: JPEG 字节数据
            
        Returns:
            写入的文件路径
        """
        filename = f"frame_{frame_index:06d}.jpg"
        filepath = os.path.join(self.output_dir, filename)
        
        with open(filepath, 'wb') as f:
            f.write(jpeg_data)
        
        self._frame_paths.append(filepath)
        return filepath
    
    def get_frame_paths(self) -> List[str]:
        """获取所有帧文件路径"""
        return self._frame_paths.copy()
    
    def clear(self) -> None:
        """清理所有帧文件"""
        for path in self._frame_paths:
            if os.path.exists(path):
                os.remove(path)
        self._frame_paths.clear()


class MjpegParser:
    """
    MJPEG 解析器 - 解析 MJPEG 文件提取帧信息
    
    用于验证 8 字节对齐和往返测试。
    """
    
    # JPEG 起始标记
    JPEG_SOI = b'\xFF\xD8'
    # JPEG 结束标记
    JPEG_EOI = b'\xFF\xD9'
    
    @classmethod
    def parse(cls, mjpeg_path: str) -> List[MjpegFrame]:
        """
        解析 MJPEG 文件，提取所有帧
        
        Args:
            mjpeg_path: MJPEG 文件路径
            
        Returns:
            MjpegFrame 列表，包含每帧的索引、偏移、大小和数据
        """
        frames: List[MjpegFrame] = []
        
        with open(mjpeg_path, 'rb') as f:
            data = f.read()
        
        pos = 0
        frame_index = 0
        
        while pos < len(data):
            # 查找 JPEG 起始标记
            soi_pos = data.find(cls.JPEG_SOI, pos)
            if soi_pos == -1:
                break
            
            # 查找 JPEG 结束标记
            eoi_pos = data.find(cls.JPEG_EOI, soi_pos + 2)
            if eoi_pos == -1:
                break
            
            # 提取帧数据（包含 EOI）
            frame_end = eoi_pos + 2
            frame_data = data[soi_pos:frame_end]
            frame_size = len(frame_data)
            
            frames.append(MjpegFrame(
                index=frame_index,
                offset=soi_pos,
                size=frame_size,
                data=frame_data
            ))
            
            frame_index += 1
            pos = frame_end
        
        return frames
    
    @classmethod
    def verify_alignment(cls, mjpeg_path: str, alignment: int = 8) -> tuple:
        """
        验证 MJPEG 文件中所有帧的对齐
        
        Args:
            mjpeg_path: MJPEG 文件路径
            alignment: 对齐字节数，默认 8
            
        Returns:
            (is_aligned, failed_frames) 元组
            is_aligned: 是否所有帧都对齐
            failed_frames: 未对齐的帧列表 [(index, offset), ...]
        """
        frames = cls.parse(mjpeg_path)
        failed_frames = []
        
        for frame in frames:
            if frame.offset % alignment != 0:
                failed_frames.append((frame.index, frame.offset))
        
        return len(failed_frames) == 0, failed_frames


class MjpegStreamWriter:
    """
    MJPEG 流写入器 - 直接写入 MJPEG 流文件（带 8 字节对齐）
    
    用于直接生成对齐的 MJPEG 文件，不依赖后处理脚本。
    """
    
    def __init__(self, output_path: str, alignment: int = 8):
        """
        初始化写入器
        
        Args:
            output_path: 输出文件路径
            alignment: 对齐字节数，默认 8
        """
        self.output_path = output_path
        self.alignment = alignment
        self._current_offset = 0
        self._frame_count = 0
        
        # 创建输出目录
        output_dir = os.path.dirname(output_path)
        if output_dir:
            os.makedirs(output_dir, exist_ok=True)
        
        # 打开文件
        self._file = open(output_path, 'wb')
    
    def write_frame(self, jpeg_data: bytes) -> int:
        """
        写入单帧（自动添加对齐填充）
        
        Args:
            jpeg_data: JPEG 字节数据
            
        Returns:
            帧的起始偏移量
        """
        # 计算需要的填充
        padding = self._calculate_padding(self._current_offset)
        
        # 写入填充
        if padding > 0:
            self._file.write(b'\x00' * padding)
            self._current_offset += padding
        
        # 记录帧起始偏移
        frame_offset = self._current_offset
        
        # 写入帧数据
        self._file.write(jpeg_data)
        self._current_offset += len(jpeg_data)
        self._frame_count += 1
        
        return frame_offset
    
    def _calculate_padding(self, offset: int) -> int:
        """计算对齐所需的填充字节数"""
        if offset % self.alignment == 0:
            return 0
        return self.alignment - (offset % self.alignment)
    
    def close(self) -> None:
        """关闭文件"""
        if self._file:
            self._file.close()
            self._file = None
    
    def get_frame_count(self) -> int:
        """获取已写入的帧数"""
        return self._frame_count
    
    def __enter__(self):
        return self
    
    def __exit__(self, exc_type, exc_val, exc_tb):
        self.close()
        return False
