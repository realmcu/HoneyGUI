"""AVI 解析器模块 - 解析 AVI 文件结构"""

import os
import struct
from typing import List, Tuple, Optional
from dataclasses import dataclass

from .exceptions import VideoFormatError


@dataclass
class AviFrameInfo:
    """AVI 帧信息"""
    index: int
    chunk_id: str
    offset: int  # chunk 头的偏移
    data_offset: int  # 数据的偏移
    size: int  # 数据大小


class AviParser:
    """
    AVI 解析器 - 解析 AVI 文件结构
    
    用于解析 AVI 文件中的帧信息并验证 MJPEG 帧的 8 字节对齐。
    """
    
    def parse(self, file_path: str) -> List[AviFrameInfo]:
        """
        解析 AVI 文件，提取所有帧信息
        
        Args:
            file_path: AVI 文件路径
            
        Returns:
            AviFrameInfo 对象列表
            
        Raises:
            FileNotFoundError: 文件不存在
            VideoFormatError: 文件格式无效
        """
        if not os.path.exists(file_path):
            raise FileNotFoundError(f"文件不存在: {file_path}")
        
        frames = self._find_movi_frames(file_path)
        return [
            AviFrameInfo(
                index=i,
                chunk_id=chunk_id,
                offset=offset,
                data_offset=data_offset,
                size=size
            )
            for i, (chunk_id, offset, size, data_offset) in enumerate(frames)
        ]

    def _find_movi_frames(self, avi_path: str) -> List[Tuple[str, int, int, int]]:
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
                raise VideoFormatError(f"无效的 AVI 文件: 缺少 RIFF 头")
            
            riff_size = struct.unpack('<I', f.read(4))[0]
            riff_type = f.read(4)
            
            if riff_type != b'AVI ':
                raise VideoFormatError(f"无效的 AVI 文件: 类型不是 AVI")
            
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
                raise VideoFormatError("AVI 文件中未找到 movi 块")
            
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

    def verify_alignment(self, file_path: str, alignment: int = 8) -> Tuple[bool, List[int]]:
        """
        验证 AVI 文件中所有帧数据的 8 字节对齐
        
        Args:
            file_path: AVI 文件路径
            alignment: 对齐字节数，默认 8
            
        Returns:
            (是否全部对齐, 未对齐的帧索引列表)
        """
        frames = self.parse(file_path)
        unaligned = []
        
        for frame in frames:
            if frame.data_offset % alignment != 0:
                unaligned.append(frame.index)
        
        return (len(unaligned) == 0, unaligned)
    
    def get_frame_offsets(self, file_path: str) -> List[Tuple[int, str, int, int]]:
        """
        获取所有帧的偏移信息
        
        Args:
            file_path: AVI 文件路径
            
        Returns:
            列表，每项为 (帧索引, chunk_id, 数据偏移, 大小)
        """
        frames = self.parse(file_path)
        return [(f.index, f.chunk_id, f.data_offset, f.size) for f in frames]
    
    def get_frame_data(self, file_path: str, frame_index: int) -> bytes:
        """
        获取指定帧的数据
        
        Args:
            file_path: AVI 文件路径
            frame_index: 帧索引
            
        Returns:
            帧数据字节
            
        Raises:
            IndexError: 帧索引超出范围
        """
        frames = self.parse(file_path)
        
        if frame_index < 0 or frame_index >= len(frames):
            raise IndexError(f"帧索引超出范围: {frame_index}")
        
        frame = frames[frame_index]
        
        with open(file_path, 'rb') as f:
            f.seek(frame.data_offset)
            return f.read(frame.size)
