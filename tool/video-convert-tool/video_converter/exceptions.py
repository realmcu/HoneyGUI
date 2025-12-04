"""异常定义"""


class VideoConverterError(Exception):
    """视频转换器基础异常"""
    pass


class VideoFormatError(VideoConverterError):
    """不支持的视频格式"""
    pass


class EncodingError(VideoConverterError):
    """编码错误"""
    pass


class PostProcessError(VideoConverterError):
    """后处理脚本执行错误"""
    pass
