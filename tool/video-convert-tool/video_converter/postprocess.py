"""后处理器模块 - 调用现有的后处理脚本"""

import os
import sys
import subprocess
import shutil
from pathlib import Path
from .exceptions import PostProcessError


class PostProcessor:
    """
    后处理器 - 调用现有的后处理脚本
    
    支持调用：
    - mkMJPEG.py: 打包 JPEG 帧为 MJPEG 流
    - procAVI_no_audio_first.py: AVI 第一步处理
    - procAVI_no_audio_second.py: AVI 第二步处理
    - h264_pack.py: H264 添加头部
    
    跨平台兼容：Linux 和 Windows
    """
    
    # 脚本目录（相对于项目根目录）
    SCRIPT_DIR = "script"
    
    @classmethod
    def _get_script_path(cls, script_name: str) -> str:
        """
        获取脚本完整路径
        
        使用 pathlib 确保跨平台路径兼容性
        """
        # 尝试多个可能的位置
        possible_paths = [
            Path(cls.SCRIPT_DIR) / script_name,
            Path(__file__).parent.parent / cls.SCRIPT_DIR / script_name,
            Path.cwd() / cls.SCRIPT_DIR / script_name,
        ]
        
        for path in possible_paths:
            abs_path = path.resolve()
            if abs_path.exists():
                return str(abs_path)
        
        raise PostProcessError(f"找不到脚本: {script_name}")
    
    @classmethod
    def _run_script(cls, script_path: str, args: list) -> None:
        """运行 Python 脚本"""
        cmd = [sys.executable, script_path] + args
        
        try:
            result = subprocess.run(
                cmd,
                capture_output=True,
                text=True,
                check=True
            )
            print(result.stdout)
        except subprocess.CalledProcessError as e:
            raise PostProcessError(
                f"脚本执行失败: {script_path}\n"
                f"错误输出: {e.stderr}"
            )
    
    @classmethod
    def process_mjpeg(cls, input_dir: str, output_path: str) -> None:
        """
        调用 mkMJPEG.py 处理 MJPEG
        
        将目录中的 JPEG 帧打包为 MJPEG 流文件。
        
        Args:
            input_dir: 包含 JPEG 帧的目录
            output_path: 输出 MJPEG 文件路径
        """
        script_path = cls._get_script_path("mkMJPEG.py")
        
        # mkMJPEG.py 使用硬编码的路径，需要直接导入并调用
        # 这里我们导入并调用其 main 函数
        import importlib.util
        spec = importlib.util.spec_from_file_location("mkMJPEG", script_path)
        module = importlib.util.module_from_spec(spec)
        spec.loader.exec_module(module)
        
        # 调用 main 函数
        module.main(input_dir, output_path)
    
    @classmethod
    def process_avi(cls, input_path: str, output_path: str) -> None:
        """
        依次调用 procAVI_no_audio_first.py 和 procAVI_no_audio_second.py
        
        Args:
            input_path: 输入 AVI 文件路径
            output_path: 最终输出 AVI 文件路径
        """
        # 第一步处理
        first_script = cls._get_script_path("procAVI_no_audio_first.py")
        temp_path = input_path + ".temp1.avi"
        
        cls._run_script(first_script, ["-i", input_path, "-o", temp_path])
        
        # 第二步处理
        second_script = cls._get_script_path("procAVI_no_audio_second.py")
        
        try:
            cls._run_script(second_script, ["-i", temp_path, "-o", output_path])
        except PostProcessError as e:
            # 如果第二步失败，使用第一步的结果
            shutil.copy(temp_path, output_path)
            print(f"警告: 第二步处理失败，使用第一步结果: {e}")
        
        # 清理临时文件
        if os.path.exists(temp_path):
            os.remove(temp_path)
    
    @classmethod
    def process_h264(cls, input_path: str, output_path: str, fps: float) -> None:
        """
        调用 h264_pack.py 添加头部
        
        Args:
            input_path: 输入 H264 文件路径
            output_path: 输出文件路径
            fps: 帧率
        """
        script_path = cls._get_script_path("h264_pack.py")
        
        # h264_pack.py 使用硬编码的路径，需要直接导入并调用
        import importlib.util
        spec = importlib.util.spec_from_file_location("h264_pack", script_path)
        module = importlib.util.module_from_spec(spec)
        spec.loader.exec_module(module)
        
        # 调用函数
        module.add_header_to_h264(input_path, output_path, fps)
