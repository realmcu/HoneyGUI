"""测试视频转换功能"""

import os
import sys

# 添加项目路径
sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))

from video_converter import VideoConverter, OutputFormat

def test_video_info():
    """测试视频信息解析"""
    print("=" * 50)
    print("测试 1: 视频信息解析")
    print("=" * 50)
    
    converter = VideoConverter()
    info = converter.get_video_info("test_video/birds.mp4")
    
    print(f"  文件: {info.file_path}")
    print(f"  分辨率: {info.width} x {info.height}")
    print(f"  帧率: {info.frame_rate:.2f} fps")
    print(f"  帧数: {info.frame_count}")
    print(f"  时长: {info.duration:.2f} 秒")
    print(f"  编码: {info.codec}")
    print("  ✓ 视频信息解析成功\n")
    return info

def progress_callback(current, total):
    """进度回调"""
    percent = (current / total) * 100 if total > 0 else 0
    print(f"\r  进度: {percent:.1f}% ({current}/{total})", end="", flush=True)

def test_mjpeg_conversion():
    """测试 MJPEG 转换"""
    print("=" * 50)
    print("测试 2: 转换为 MJPEG")
    print("=" * 50)
    
    input_path = "test_video/birds.mp4"
    output_path = "test_video/output_birds.mjpeg"
    
    converter = VideoConverter(progress_callback=progress_callback)
    
    try:
        result = converter.convert(
            input_path=input_path,
            output_path=output_path,
            output_format=OutputFormat.MJPEG,
            frame_rate=25,
            quality=85
        )
        print()  # 换行
        print(f"  输出文件: {result.output_path}")
        print(f"  帧数: {result.frame_count}")
        print(f"  帧率: {result.frame_rate} fps")
        
        if os.path.exists(output_path):
            size = os.path.getsize(output_path)
            print(f"  文件大小: {size / 1024:.1f} KB")
            print("  ✓ MJPEG 转换成功\n")
        else:
            print("  ✗ 输出文件不存在\n")
    except Exception as e:
        print(f"\n  ✗ MJPEG 转换失败: {e}\n")

def test_avi_conversion():
    """测试 AVI-MJPEG 转换"""
    print("=" * 50)
    print("测试 3: 转换为 AVI-MJPEG")
    print("=" * 50)
    
    input_path = "test_video/birds.mp4"
    output_path = "test_video/output_birds.avi"
    
    converter = VideoConverter(progress_callback=progress_callback)
    
    try:
        result = converter.convert(
            input_path=input_path,
            output_path=output_path,
            output_format=OutputFormat.AVI_MJPEG,
            frame_rate=25,
            quality=90
        )
        print()  # 换行
        print(f"  输出文件: {result.output_path}")
        print(f"  帧数: {result.frame_count}")
        print(f"  帧率: {result.frame_rate} fps")
        
        if os.path.exists(output_path):
            size = os.path.getsize(output_path)
            print(f"  文件大小: {size / 1024:.1f} KB")
            print("  ✓ AVI-MJPEG 转换成功\n")
        else:
            print("  ✗ 输出文件不存在\n")
    except Exception as e:
        print(f"\n  ✗ AVI-MJPEG 转换失败: {e}\n")

def test_h264_conversion():
    """测试 H264 转换"""
    print("=" * 50)
    print("测试 4: 转换为 H264")
    print("=" * 50)
    
    input_path = "test_video/birds.mp4"
    output_path = "test_video/output_birds.h264"
    
    converter = VideoConverter(progress_callback=progress_callback)
    
    try:
        result = converter.convert(
            input_path=input_path,
            output_path=output_path,
            output_format=OutputFormat.H264,
            frame_rate=30,
            quality=23  # CRF 值
        )
        print()  # 换行
        print(f"  输出文件: {result.output_path}")
        print(f"  帧数: {result.frame_count}")
        print(f"  帧率: {result.frame_rate} fps")
        
        if os.path.exists(output_path):
            size = os.path.getsize(output_path)
            print(f"  文件大小: {size / 1024:.1f} KB")
            print("  ✓ H264 转换成功\n")
        else:
            print("  ✗ 输出文件不存在\n")
    except Exception as e:
        print(f"\n  ✗ H264 转换失败: {e}\n")

def main():
    print("\n视频转换工具测试\n")
    
    # 检查测试视频是否存在
    if not os.path.exists("test_video/birds.mp4"):
        print("错误: 测试视频 test_video/birds.mp4 不存在")
        return
    
    # 运行测试
    test_video_info()
    test_mjpeg_conversion()
    test_avi_conversion()
    test_h264_conversion()
    
    print("=" * 50)
    print("测试完成")
    print("=" * 50)

if __name__ == "__main__":
    main()
