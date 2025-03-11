import os
import struct

def read_bin_files(base_dir):
    # Traverse the directory and its subdirectories to collect all .bin files
    bin_files = []
    for root, dirs, files in os.walk(base_dir):
        for file in files:
            if file.endswith('.bin'):
                bin_files.append(os.path.join(root, file))
    return bin_files

def parse_bin_file(file_path):
    with open(file_path, 'rb') as f:
        data = f.read()

        # Check if the file length is sufficient for parsing
        if len(data) < 6:
            print(f"File {file_path} is too short to parse.")
            return None

        # Get the second byte to determine the color space
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

        # Read the 3rd and 4th bytes to determine the width W, note that the low byte comes first
        width = struct.unpack('<H', data[2:4])[0]

        # Read the 5th and 6th bytes to determine the height H, note that the low byte comes first
        height = struct.unpack('<H', data[4:6])[0]

        data_size = width * height * size

        # Extract the file name and its different formats
        file_name = os.path.basename(file_path)
        struct_name = os.path.splitext(file_name)[0].lower()
        data_name = file_name.upper().replace('.', '_')

        # Return the parsed result
        return {
            'struct_name': struct_name,
            'width': width,
            'height': height,
            'data_size': data_size,
            'color_space': color_space,
            'data_name': data_name
        }

def generate_c_file(bin_file_data, output_c_file, output_h_file):
    # Generate the .c file
    with open(output_c_file, 'w') as f_c:
        f_c.write('#include "lvgl.h"\n')
        f_c.write('#include "lv_image_dsc.h"\n')
        f_c.write('#include "lv_color.h"\n')
        f_c.write('#include "root_image_lvgl/ui_resource.h"\n\n')
        for data in bin_file_data:
            # Write the structure
            f_c.write(f"const lv_image_dsc_t {data['struct_name']} = {{\n")
            f_c.write(f"    .header.magic = LV_IMAGE_HEADER_MAGIC,\n")
            f_c.write(f"    .header.w = {data['width']},\n")
            f_c.write(f"    .header.h = {data['height']},\n")
            f_c.write(f"    .data_size = {data['data_size']},\n")
            f_c.write(f"    .header.cf = {data['color_space']},\n")
            f_c.write(f"    .data = {data['data_name']} + 8\n")
            f_c.write("};\n\n")

    # Generate the .h file
    with open(output_h_file, 'w') as f_h:
        include_guard = os.path.basename(output_h_file).replace('.', '_').upper()
        f_h.write(f"#ifndef {include_guard}\n")
        f_h.write(f"#define {include_guard}\n\n")
        for data in bin_file_data:
            f_h.write(f"extern const lv_image_dsc_t {data['struct_name']};\n")
        f_h.write(f"\n#endif // {include_guard}\n")

if __name__ == '__main__':
    # Replace with your directory path
    directory = "./root"
    output_c_file = "lv_img_dsc_list.c"  # Generated C file
    output_h_file = "lv_img_dsc_list.h"  # Generated H file

    bin_files = read_bin_files(directory)
    bin_file_data = []

    for bin_file in bin_files:
        result = parse_bin_file(bin_file)
        if result:
            bin_file_data.append(result)

    generate_c_file(bin_file_data, output_c_file, output_h_file)
    # print(f"C file generated: {output_c_file}")
    # print(f"H file generated: {output_h_file}")
