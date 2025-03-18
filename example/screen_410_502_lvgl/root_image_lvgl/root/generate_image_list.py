import os
import struct

def read_bin_files(base_dir):
    # 遍历目录及其子目录，收集所有 .bin 文件
    bin_files = []
    for root, dirs, files in os.walk(base_dir):
        for file in files:
            if file.endswith('.bin'):
                bin_files.append(os.path.join(root, file))
    return bin_files

def parse_bin_file(file_path):
    with open(file_path, 'rb') as f:
        data = f.read()

        # 判断文件长度足够来进行解析
        if len(data) < 6:
            print(f"File {file_path} is too short to parse.")
            return None

        # 获取第2个字节，确定颜色空间
        color_byte = data[1]
        if color_byte == 0:
            color_space = "LV_COLOR_FORMAT_RGB565"
            size = 2
        elif color_byte == 4:
            color_space = "LV_COLOR_FORMAT_ARGB8888"
            size = 4
        else:
            color_space = "LV_COLOR_FORMAT_RGB565"
            size = 2

        # 读取第3、4字节，确定宽度W，注意低位在前
        width = struct.unpack('<H', data[2:4])[0]

        # 读取第5、6字节，确定高度H，注意低位在前
        height = struct.unpack('<H', data[4:6])[0]

        data_size = width * height * size

        # 提取文件名及其不同格式
        file_name = os.path.basename(file_path)
        struct_name = os.path.splitext(file_name)[0].lower()
        data_name = file_name.upper().replace('.', '_')

        # 返回解析出的结果
        return {
            'struct_name': struct_name,
            'width': width,
            'height': height,
            'data_size': data_size,
            'color_space': color_space,
            'data_name': data_name
        }

def generate_c_file(bin_file_data, output_c_file, output_h_file):
    # 生成 .c 文件
    with open(output_c_file, 'w') as f_c:
        f_c.write('#include "lvgl.h"\n\n')
        for data in bin_file_data:
            # 写入结构体
            f_c.write(f"const lv_image_dsc_t {data['struct_name']} = {{\n")
            f_c.write(f"    .header.w = {data['width']},\n")
            f_c.write(f"    .header.h = {data['height']},\n")
            f_c.write(f"    .data_size = {data['data_size']},\n")
            f_c.write(f"    .header.cf = {data['color_space']},\n")
            f_c.write(f"    .data = {data['data_name']} + 8\n")
            f_c.write("};\n\n")

    # 生成 .h 文件
    with open(output_h_file, 'w') as f_h:
        include_guard = os.path.basename(output_h_file).replace('.', '_').upper()
        f_h.write(f"#ifndef {include_guard}\n")
        f_h.write(f"#define {include_guard}\n\n")
        for data in bin_file_data:
            f_h.write(f"LV_IMG_DECLARE({data['struct_name']});\n")
        f_h.write(f"\n#endif // {include_guard}\n")

if __name__ == '__main__':
    # 替换为你的目录路径
    directory = "./"
    output_c_file = "lv_img_dsc_list.c"  # 生成的C文件
    output_h_file = "lv_img_dsc_list.h"  # 生成的H文件

    bin_files = read_bin_files(directory)
    bin_file_data = []

    for bin_file in bin_files:
        result = parse_bin_file(bin_file)
        if result:
            bin_file_data.append(result)

    generate_c_file(bin_file_data, output_c_file, output_h_file)
    # print(f"C file generated: {output_c_file}")
    # print(f"H file generated: {output_h_file}")
