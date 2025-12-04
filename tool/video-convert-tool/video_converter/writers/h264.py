"""H264 写入器模块"""

import os
from typing import Optional


class H264Writer:
    """
    H264 写入器 - 写入原始 H264 流文件
    
    用于写入原始 H264 NAL 单元数据。
    """
    
    def __init__(self, output_path: str):
        """
        初始化写入器
        
        Args:
            output_path: 输出文件路径
        """
        self.output_path = output_path
        self._bytes_written = 0
        
        # 创建输出目录
        output_dir = os.path.dirname(output_path)
        if output_dir:
            os.makedirs(output_dir, exist_ok=True)
        
        # 打开文件
        self._file = open(output_path, 'wb')
    
    def write(self, data: bytes) -> int:
        """
        写入数据
        
        Args:
            data: 要写入的字节数据
            
        Returns:
            写入的字节数
        """
        written = self._file.write(data)
        self._bytes_written += written
        return written
    
    def get_bytes_written(self) -> int:
        """获取已写入的字节数"""
        return self._bytes_written
    
    def close(self) -> None:
        """关闭文件"""
        if self._file:
            self._file.close()
            self._file = None
    
    def __enter__(self):
        return self
    
    def __exit__(self, exc_type, exc_val, exc_tb):
        self.close()
        return False


class H264Parser:
    """
    H264 解析器 - 解析 H264 流
    
    用于验证 H264 流的有效性和提取 SPS 信息。
    """
    
    # NAL 单元类型
    NAL_TYPE_SLICE = 1
    NAL_TYPE_IDR = 5
    NAL_TYPE_SPS = 7
    NAL_TYPE_PPS = 8
    
    @staticmethod
    def find_start_code(data: bytes, start: int = 0) -> int:
        """
        查找 H264 起始码位置
        
        Args:
            data: H264 数据
            start: 起始搜索位置
            
        Returns:
            起始码位置，未找到返回 -1
        """
        # 3 字节起始码
        code_3 = b'\x00\x00\x01'
        # 4 字节起始码
        code_4 = b'\x00\x00\x00\x01'
        
        pos_3 = data.find(code_3, start)
        pos_4 = data.find(code_4, start)
        
        if pos_3 == -1 and pos_4 == -1:
            return -1
        elif pos_3 == -1:
            return pos_4
        elif pos_4 == -1:
            return pos_3
        else:
            return min(pos_3, pos_4)
    
    @classmethod
    def find_nal_units(cls, data: bytes) -> list:
        """
        查找所有 NAL 单元
        
        Args:
            data: H264 数据
            
        Returns:
            NAL 单元列表 [(type, offset, size), ...]
        """
        nal_units = []
        pos = 0
        
        while pos < len(data):
            start = cls.find_start_code(data, pos)
            if start == -1:
                break
            
            # 确定起始码长度
            if data[start:start+4] == b'\x00\x00\x00\x01':
                header_len = 4
            else:
                header_len = 3
            
            # NAL 类型
            nal_byte = data[start + header_len]
            nal_type = nal_byte & 0x1F
            
            # 查找下一个 NAL 单元
            next_start = cls.find_start_code(data, start + header_len)
            if next_start == -1:
                nal_size = len(data) - start
            else:
                nal_size = next_start - start
            
            nal_units.append((nal_type, start, nal_size))
            pos = start + nal_size
        
        return nal_units
    
    @classmethod
    def count_frames(cls, data: bytes) -> int:
        """
        统计帧数
        
        Args:
            data: H264 数据
            
        Returns:
            帧数（IDR + 非 IDR slice）
        """
        nal_units = cls.find_nal_units(data)
        frame_count = 0
        
        for nal_type, _, _ in nal_units:
            if nal_type in (cls.NAL_TYPE_SLICE, cls.NAL_TYPE_IDR):
                frame_count += 1
        
        return frame_count
    
    @classmethod
    def has_valid_structure(cls, data: bytes) -> bool:
        """
        检查 H264 流是否有有效结构
        
        Args:
            data: H264 数据
            
        Returns:
            True 如果包含 SPS 和至少一个帧
        """
        nal_units = cls.find_nal_units(data)
        
        has_sps = False
        has_frame = False
        
        for nal_type, _, _ in nal_units:
            if nal_type == cls.NAL_TYPE_SPS:
                has_sps = True
            elif nal_type in (cls.NAL_TYPE_SLICE, cls.NAL_TYPE_IDR):
                has_frame = True
        
        return has_sps and has_frame
