import os
import sys
import subprocess
from PIL import Image
import struct

class GuiRGBDataHeader:
    def __init__(self):
        # 初始化头部数据
        self.scan = 0
        self.align = 0
        self.resize = 0
        self.compress = 0
        self.rsvd = 0
        self.type = 0
        self.w = 0
        self.h = 0
        self.version = 0
        self.rsvd2 = 0

    def pack(self):
        # 打包数据到二进制格式
        first_byte = (self.scan & 0x01) | ((self.align & 0x01) << 1) | ((self.resize & 0x03) << 2) | ((self.compress & 0x01) << 4) | ((self.rsvd & 0x07) << 5)
        return struct.pack('=BBhhbb', first_byte, self.type, self.w, self.h, self.version, self.rsvd2)
    
def rgb_to_rgb565(r, g, b):
    return ((r & 0xF8) << 8) | ((g & 0xFC) << 3) | (b >> 3)

def convert_to_rgb565(pixels):
    rgb565_data = []
    for i in range(0, len(pixels), 3):
        r = pixels[i]
        g = pixels[i + 1]
        b = pixels[i + 2]
        rgb565_data.append(rgb_to_rgb565(r, g, b))
    return rgb565_data

def png_to_c_with_header(input_file, output_file):
    header = GuiRGBDataHeader()
    header.type = 0

    with Image.open(input_file) as img:
        img = img.convert("RGB")
        header.w, header.h = img.size
        pixels = img.tobytes()

    rgb565_pixels = convert_to_rgb565(pixels)
    header_bytes = header.pack()
    array_name = "_ac" + os.path.splitext(os.path.basename(input_file))[0]
    total_length = len(header_bytes) + len(rgb565_pixels)*2

    with open(output_file, 'w') as f:
        f.write(f"static const unsigned char {array_name}[{total_length}] = {{")

        # 写入头部数据
        for i, b in enumerate(header_bytes):
            if i % 40 == 0:
                f.write("\n")
            f.write(f"0x{b:02x}, ")
        # 写入图像数据
        for i, b in enumerate(rgb565_pixels):
            if (i + len(header_bytes)) % 40 == 0:
                f.write("\n")
            if i == len(rgb565_pixels) - 1:
                f.write(f"0x{b & 0xFF:02x}, 0x{(b >> 8) & 0xFF:02x}")
            else:
                f.write(f"0x{b & 0xFF:02x}, 0x{(b >> 8) & 0xFF:02x}, ")
        f.write("\n};\n")
    print(f"Convert {input_file} to {output_file} successfully!")
    
def process_png_file(filename):
    if not os.path.isfile(filename):
        print(f"Error: {filename} not found!")
        return

    # # 提取文件的基础名称和扩展名
    base, ext = os.path.splitext(filename)

    # 将重命名后的文件转换为 .txt 或 .c 文件

    output_filename = os.path.basename(filename).replace(ext, ".txt")
    png_to_c_with_header(filename, output_filename)


def process_files_in_directory():
    current_directory = os.getcwd()

    png_files = [f for f in os.listdir(current_directory) if f.endswith('.png')]

    for png_file in png_files:
        process_png_file(png_file)
    


if __name__ == "__main__":

    process_files_in_directory()
