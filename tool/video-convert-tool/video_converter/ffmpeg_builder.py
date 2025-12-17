"""FFmpeg 命令构建器"""

from typing import List, Optional


class FFmpegBuilder:
    """FFmpeg 命令构建器 - 构建不同格式转换的 ffmpeg 命令"""
    
    # H264 编码参数
    H264_X264_PARAMS = (
        "cabac=0:ref=3:deblock=1:0:0:analyse=0x1:0x111:me=hex:subme=7:"
        "psy=1:psy_rd=1.0:0.0:mixed_ref=1:me_range=16:chroma_me=1:"
        "trellis=1:8x8dct=0:deadzone-inter=21:deadzone-intra=11:"
        "fast_pskip=1:chroma_qp_offset=-2:threads=11:lookahead_threads=1:"
        "sliced_threads=0:nr=0:decimate=1:interlaced=0:bluray_compat=0:"
        "constrained_intra=0:bframes=0:weightp=0:keyint=40:min-keyint=4:"
        "scenecut=40:intra_refresh=0:rc_lookahead=40:mbtree=1:"
        "crf={crf}:qcomp=0.60:qpmin=0:qpmax=69:qpstep=4:ipratio=1.40:"
        "aq-mode=1:aq-strength=1.00"
    )
    
    def build_mjpeg_frames_cmd(
        self,
        input_path: str,
        output_dir: str,
        frame_rate: Optional[float] = None,
        quality: int = 5
    ) -> List[str]:
        """
        构建 MJPEG 帧提取命令
        
        命令格式:
        ffmpeg -i input.mp4 -r 24 -vf "format=yuvj420p" -q:v 5 output/frame_%04d.jpg
        
        Args:
            input_path: 输入视频文件路径
            output_dir: 输出目录路径
            frame_rate: 目标帧率，None 表示保持原帧率
            quality: JPEG 质量 (1-31，1最高)
            
        Returns:
            ffmpeg 命令参数列表
        """
        cmd = ["ffmpeg", "-i", input_path]
        
        # 添加帧率参数（如果指定）
        if frame_rate is not None:
            cmd.extend(["-r", str(frame_rate)])
        
        # 添加视频滤镜和质量参数
        # 注意：-vf 参数值需要正确格式化
        cmd.extend(["-vf", "format=yuvj420p"])
        # 质量参数：1 最高质量，31 最低质量
        cmd.extend(["-q:v", str(quality)])
        
        # 输出路径模式
        output_pattern = f"{output_dir}/frame_%04d.jpg"
        cmd.append(output_pattern)
        
        return cmd

    def build_avi_cmd(
        self,
        input_path: str,
        output_path: str,
        frame_rate: Optional[float] = None,
        quality: int = 5
    ) -> List[str]:
        """
        构建 AVI-MJPEG 转换命令
        
        命令格式:
        ffmpeg -i input.mp4 -an -r 25 -vcodec mjpeg -pix_fmt yuvj420p -q:v 5 output.avi
        
        Args:
            input_path: 输入视频文件路径
            output_path: 输出文件路径
            frame_rate: 目标帧率，None 表示保持原帧率
            quality: JPEG 质量 (1-31，1最高)
            
        Returns:
            ffmpeg 命令参数列表
        """
        cmd = ["ffmpeg", "-i", input_path]
        
        # 无音频
        cmd.append("-an")
        
        # 添加帧率参数（如果指定）
        if frame_rate is not None:
            cmd.extend(["-r", str(frame_rate)])
        
        # 视频编码器和像素格式
        cmd.extend(["-vcodec", "mjpeg"])
        cmd.extend(["-pix_fmt", "yuvj420p"])
        
        # 质量参数
        cmd.extend(["-q:v", str(quality)])
        
        # 输出路径
        cmd.append(output_path)
        
        return cmd
    
    def build_h264_cmd(
        self,
        input_path: str,
        output_path: str,
        frame_rate: Optional[float] = None,
        crf: int = 23
    ) -> List[str]:
        """
        构建 H264 转换命令
        
        命令格式:
        ffmpeg -r 30 -i input.mp4 -c:v libx264 -x264-params "..." -an -f rawvideo output.h264
        
        Args:
            input_path: 输入视频文件路径
            output_path: 输出文件路径
            frame_rate: 输入帧率，None 表示保持原帧率
            crf: CRF 值控制编码质量
            
        Returns:
            ffmpeg 命令参数列表
        """
        cmd = ["ffmpeg"]
        
        # 添加帧率参数（如果指定）- H264 的帧率参数在输入之前
        if frame_rate is not None:
            cmd.extend(["-r", str(frame_rate)])
        
        # 输入文件
        cmd.extend(["-i", input_path])
        
        # H264 编码器
        cmd.extend(["-c:v", "libx264"])
        
        # x264 参数（包含 CRF 值）
        x264_params = self.H264_X264_PARAMS.format(crf=crf)
        cmd.extend(["-x264-params", x264_params])
        
        # 无音频
        cmd.append("-an")
        
        # 输出格式为原始视频
        cmd.extend(["-f", "rawvideo"])
        
        # 输出路径
        cmd.append(output_path)
        
        return cmd
