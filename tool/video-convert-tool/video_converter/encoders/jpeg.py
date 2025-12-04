"""JPEG 编码器模块"""

import cv2
import numpy as np
from ..exceptions import EncodingError


class JpegEncoder:
    """
    JPEG 编码器 - 将帧数据编码为 YUV420 Baseline JPEG 格式
    
    使用 OpenCV 进行编码，确保输出为 Baseline DCT (SOF0) 格式。
    """
    
    def __init__(self, quality: int = 85):
        """
        初始化编码器
        
        Args:
            quality: JPEG 质量 (1-100)，默认 85
            
        Raises:
            ValueError: 质量值超出范围
        """
        if not 1 <= quality <= 100:
            raise ValueError(f"质量值必须在 1-100 之间，当前值: {quality}")
        
        self.quality = quality
        # OpenCV JPEG 编码参数
        self._encode_params = [
            cv2.IMWRITE_JPEG_QUALITY, quality,
            cv2.IMWRITE_JPEG_OPTIMIZE, 0,  # 不优化，保持 Baseline
        ]
    
    def encode(self, frame: np.ndarray) -> bytes:
        """
        编码单帧为 JPEG
        
        Args:
            frame: BGR 格式的帧数据 (numpy array)
            
        Returns:
            JPEG 字节数据（YUV420 Baseline 编码）
            
        Raises:
            EncodingError: 编码失败
        """
        if frame is None or frame.size == 0:
            raise EncodingError("无效的帧数据")
        
        # 转换为 YUV420 (YCrCb) 色彩空间
        # OpenCV 的 JPEG 编码器默认使用 YUV420 子采样
        try:
            # 编码为 JPEG
            success, encoded = cv2.imencode('.jpg', frame, self._encode_params)
            
            if not success:
                raise EncodingError("JPEG 编码失败")
            
            jpeg_bytes = encoded.tobytes()
            
            # 验证是否为 Baseline JPEG
            if not self._is_baseline_jpeg(jpeg_bytes):
                raise EncodingError("编码结果不是 Baseline JPEG")
            
            return jpeg_bytes
            
        except cv2.error as e:
            raise EncodingError(f"OpenCV 编码错误: {e}")
    
    @staticmethod
    def _is_baseline_jpeg(jpeg_bytes: bytes) -> bool:
        """
        检查 JPEG 是否为 Baseline 编码
        
        Baseline DCT 标记为 0xFFC0 (SOF0)
        Progressive DCT 标记为 0xFFC2 (SOF2)
        
        Args:
            jpeg_bytes: JPEG 字节数据
            
        Returns:
            True 如果是 Baseline JPEG，否则 False
        """
        pos = 0
        length = len(jpeg_bytes)
        
        while pos < length - 1:
            if jpeg_bytes[pos] == 0xFF:
                marker = jpeg_bytes[pos + 1]
                
                if marker == 0xC0:  # SOF0 - Baseline DCT
                    return True
                elif marker == 0xC2:  # SOF2 - Progressive DCT
                    return False
                
                # 跳过有长度字段的段
                if marker not in (0xD8, 0xD9, 0x00):  # SOI, EOI, 填充
                    if pos + 3 < length:
                        seg_len = (jpeg_bytes[pos + 2] << 8) | jpeg_bytes[pos + 3]
                        pos += 2 + seg_len
                        continue
            
            pos += 1
        
        return False
    
    def set_quality(self, quality: int) -> None:
        """
        设置编码质量
        
        Args:
            quality: JPEG 质量 (1-100)
        """
        if not 1 <= quality <= 100:
            raise ValueError(f"质量值必须在 1-100 之间，当前值: {quality}")
        
        self.quality = quality
        self._encode_params = [
            cv2.IMWRITE_JPEG_QUALITY, quality,
            cv2.IMWRITE_JPEG_OPTIMIZE, 0,
        ]
