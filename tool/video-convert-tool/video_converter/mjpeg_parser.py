"""MJPEG 解析器模块 - 用于解析和写入 MJPEG 文件"""

import os
import struct
from typing import List, Tuple, Optional
from dataclasses import dataclass

from .models import MjpegFrame
from .exceptions import VideoFormatError


class MjpegParser:
    """
    MJPEG 解析器 - 解析 MJPEG 文件提取帧和偏移信息
    
    MJPEG 文件是多个 JPEG 帧连续存储的格式，每个 JPEG 帧以 FFD8 开始，FFD9 结束。
    为了支持 8 字节对齐，每个帧可能在 APP1 段中包含填充字节。
    """
    
    # JPEG 标记
    JPEG_SOI = b'\xFF\xD8'  # Start of Image
    JPEG_EOI = b'\xFF\xD9'  # End of Image
    
    def parse(self, file_path: str) -> List[MjpegFrame]:
        """
        解析 MJPEG 文件，提取各个 JPEG 帧及其偏移信息
        
        Args:
            file_path: MJPEG 文件路径
            
        Returns:
            MjpegFrame 对象列表，包含每帧的索引、偏移、大小和数据
            
        Raises:
            FileNotFoundError: 文件不存在
            VideoFormatError: 文件格式无效
        """
        if not os.path.exists(file_path):
            raise FileNotFoundError(f"文件不存在: {file_path}")
        
        with open(file_path, 'rb') as f:
            data = f.read()
        
        return self._parse_frames(data)

    def _parse_frames(self, data: bytes) -> List[MjpegFrame]:
        """
        从字节数据中解析 JPEG 帧
        
        Args:
            data: MJPEG 文件的字节数据
            
        Returns:
            MjpegFrame 对象列表
        """
        frames = []
        pos = 0
        frame_index = 0
        
        while pos < len(data):
            # 查找 JPEG 开始标记 (FFD8)
            soi_pos = data.find(self.JPEG_SOI, pos)
            if soi_pos == -1:
                break
            
            # 查找 JPEG 结束标记 (FFD9)
            eoi_pos = self._find_eoi(data, soi_pos)
            if eoi_pos == -1:
                # 没有找到结束标记，可能是损坏的文件
                break
            
            # 帧数据包含 EOI 标记
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
    
    def _find_eoi(self, data: bytes, start: int) -> int:
        """
        查找 JPEG 结束标记 (FFD9)
        
        需要正确处理 JPEG 内部的 FF 字节，避免误判。
        
        Args:
            data: 字节数据
            start: 开始搜索的位置
            
        Returns:
            EOI 标记的位置，如果未找到返回 -1
        """
        pos = start + 2  # 跳过 SOI
        
        while pos < len(data) - 1:
            if data[pos] == 0xFF:
                marker = data[pos + 1]
                
                if marker == 0xD9:  # EOI
                    return pos
                elif marker == 0x00:  # 填充字节
                    pos += 2
                elif marker == 0xD8:  # 嵌套的 SOI（不应该出现）
                    pos += 2
                elif marker >= 0xD0 and marker <= 0xD7:  # RST 标记
                    pos += 2
                elif marker == 0xFF:  # 连续的 FF
                    pos += 1
                else:
                    # 其他标记，读取长度并跳过
                    if pos + 3 < len(data):
                        length = struct.unpack(">H", data[pos+2:pos+4])[0]
                        pos += 2 + length
                    else:
                        pos += 2
            else:
                pos += 1
        
        return -1

    def write(self, frames: List[MjpegFrame], output_path: str, align: int = 8) -> None:
        """
        将帧数据写入 MJPEG 文件
        
        Args:
            frames: MjpegFrame 对象列表
            output_path: 输出文件路径
            align: 对齐字节数（默认 8）
        """
        # 确保输出目录存在
        output_dir = os.path.dirname(output_path)
        if output_dir and not os.path.exists(output_dir):
            os.makedirs(output_dir)
        
        with open(output_path, 'wb') as f:
            for frame in frames:
                # 写入帧数据
                f.write(frame.data)
                
                # 计算并写入填充字节以实现对齐
                current_pos = f.tell()
                padding = self._calculate_padding(current_pos, align)
                if padding > 0:
                    f.write(b'\x00' * padding)
    
    def _calculate_padding(self, size: int, alignment: int = 8) -> int:
        """
        计算需要的填充字节数
        
        Args:
            size: 当前大小
            alignment: 对齐字节数
            
        Returns:
            需要的填充字节数
        """
        if size % alignment == 0:
            return 0
        return alignment - (size % alignment)
    
    def verify_alignment(self, file_path: str, alignment: int = 8) -> Tuple[bool, List[int]]:
        """
        验证 MJPEG 文件中每个帧的起始偏移是否对齐
        
        Args:
            file_path: MJPEG 文件路径
            alignment: 对齐字节数（默认 8）
            
        Returns:
            (是否全部对齐, 未对齐的帧索引列表)
        """
        frames = self.parse(file_path)
        unaligned = []
        
        for frame in frames:
            if frame.offset % alignment != 0:
                unaligned.append(frame.index)
        
        return (len(unaligned) == 0, unaligned)
    
    def get_frame_offsets(self, file_path: str) -> List[Tuple[int, int, int]]:
        """
        获取所有帧的偏移信息
        
        Args:
            file_path: MJPEG 文件路径
            
        Returns:
            列表，每项为 (帧索引, 偏移, 大小)
        """
        frames = self.parse(file_path)
        return [(f.index, f.offset, f.size) for f in frames]
