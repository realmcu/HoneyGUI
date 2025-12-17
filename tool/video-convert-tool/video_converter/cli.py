"""命令行接口模块"""

import argparse
import sys
from typing import Optional

from .models import OutputFormat
from .converter import VideoConverter
from .exceptions import VideoConverterError


def create_parser() -> argparse.ArgumentParser:
    """创建命令行参数解析器"""
    parser = argparse.ArgumentParser(
        prog="video_converter",
        description="视频转换工具 - 将视频转换为 MJPEG、AVI-MJPEG 或 H264 格式",
        formatter_class=argparse.RawDescriptionHelpFormatter,
        epilog="""
示例:
  # 转换为 MJPEG
  python -m video_converter -i input.mp4 -o output.mjpeg -f mjpeg
  
  # 转换为 AVI-MJPEG，指定帧率和质量
  python -m video_converter -i input.mp4 -o output.avi -f avi_mjpeg -r 25 -q 1
  
  # 转换为 H264
  python -m video_converter -i input.mp4 -o output.h264 -f h264 -r 30 -q 23
  
  # 查看视频信息
  python -m video_converter -i input.mp4 --info
"""
    )
    
    parser.add_argument(
        "-i", "--input",
        required=True,
        help="输入视频文件路径"
    )
    
    parser.add_argument(
        "-o", "--output",
        help="输出文件路径（使用 --info 时可省略）"
    )
    
    parser.add_argument(
        "-f", "--format",
        choices=["mjpeg", "avi_mjpeg", "h264"],
        help="输出格式: mjpeg, avi_mjpeg, h264"
    )
    
    parser.add_argument(
        "-r", "--framerate",
        type=float,
        help="目标帧率（默认保持原帧率）"
    )
    
    parser.add_argument(
        "-q", "--quality",
        type=int,
        default=1,
        help="编码质量: MJPEG/AVI 为 1-31（1最高质量），H264 为 CRF 值 0-51（默认: 1）"
    )
    
    parser.add_argument(
        "--info",
        action="store_true",
        help="仅显示输入视频信息，不进行转换"
    )
    
    parser.add_argument(
        "-v", "--verbose",
        action="store_true",
        help="显示详细输出"
    )
    
    return parser


def print_video_info(converter: VideoConverter, input_path: str) -> None:
    """打印视频信息"""
    info = converter.get_video_info(input_path)
    
    print(f"\n视频信息: {input_path}")
    print("-" * 40)
    print(f"  分辨率: {info.width} x {info.height}")
    print(f"  帧率: {info.frame_rate:.2f} fps")
    print(f"  帧数: {info.frame_count}")
    print(f"  时长: {info.duration:.2f} 秒")
    print(f"  编码: {info.codec}")
    print()


def print_progress(current: int, total: int) -> None:
    """打印进度"""
    percent = (current / total) * 100 if total > 0 else 0
    bar_length = 40
    filled = int(bar_length * current / total) if total > 0 else 0
    bar = "=" * filled + "-" * (bar_length - filled)
    print(f"\r进度: [{bar}] {percent:.1f}% ({current}/{total})", end="", flush=True)


def print_result(result) -> None:
    """打印转换结果"""
    print("\n")
    print("转换完成!")
    print("-" * 40)
    print(f"  输入文件: {result.input_path}")
    print(f"  输出文件: {result.output_path}")
    print(f"  输出格式: {result.output_format.value}")
    print(f"  帧数: {result.frame_count}")
    print(f"  帧率: {result.frame_rate:.2f} fps")
    print(f"  质量: {result.quality}")
    print()


def main(args: Optional[list] = None) -> int:
    """主函数"""
    parser = create_parser()
    parsed_args = parser.parse_args(args)
    
    # 验证参数
    if not parsed_args.info:
        if not parsed_args.output:
            parser.error("需要指定输出文件路径 (-o/--output)")
        if not parsed_args.format:
            parser.error("需要指定输出格式 (-f/--format)")
    
    # 创建转换器
    progress_callback = print_progress if parsed_args.verbose else None
    converter = VideoConverter(progress_callback=progress_callback)
    
    try:
        # 仅显示信息
        if parsed_args.info:
            print_video_info(converter, parsed_args.input)
            return 0
        
        # 解析输出格式
        format_map = {
            "mjpeg": OutputFormat.MJPEG,
            "avi_mjpeg": OutputFormat.AVI_MJPEG,
            "h264": OutputFormat.H264,
        }
        output_format = format_map[parsed_args.format]
        
        # 显示开始信息
        if parsed_args.verbose:
            print_video_info(converter, parsed_args.input)
            print(f"开始转换...")
            print(f"  输出格式: {output_format.value}")
            print(f"  目标帧率: {parsed_args.framerate or '保持原帧率'}")
            print(f"  质量: {parsed_args.quality}")
            print()
        
        # 执行转换
        result = converter.convert(
            input_path=parsed_args.input,
            output_path=parsed_args.output,
            output_format=output_format,
            frame_rate=parsed_args.framerate,
            quality=parsed_args.quality
        )
        
        # 显示结果
        print_result(result)
        return 0
        
    except FileNotFoundError as e:
        print(f"错误: {e}", file=sys.stderr)
        return 1
    except VideoConverterError as e:
        print(f"转换错误: {e}", file=sys.stderr)
        return 1
    except Exception as e:
        print(f"未知错误: {e}", file=sys.stderr)
        return 1


if __name__ == "__main__":
    sys.exit(main())
