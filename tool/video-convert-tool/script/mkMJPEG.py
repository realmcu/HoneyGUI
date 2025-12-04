import os
import struct

def calculate_padding(size, alignment=8):
    if size % alignment == 0:
        return 0
    return alignment - (size % alignment)

def is_baseline_jpeg(jpeg_bytes):
    """
    检查 JPEG 是否 baseline 编码。baseline DCT 标记为 0xFFC0（SOF0）。
    """
    pos = 0
    while pos < len(jpeg_bytes) - 1:
        if jpeg_bytes[pos] == 0xFF:
            marker = jpeg_bytes[pos + 1]
            if marker == 0xC0:  # SOF0
                return True
            elif marker == 0xC2:  # SOF2, Progressive JPEG
                return False
            # 跳过有长度的段
            if marker not in (0xD8, 0xD9):  # SOI, EOI没有长度字段
                length = struct.unpack(">H", jpeg_bytes[pos+2:pos+4])[0]
                pos += 2 + length
                continue
        pos += 1
    return False

def find_app1_insert_position(jpeg_bytes):
    """
    查找 APP1 插入点和 APP1 区域的 start/end，如果没有则返回插入点。
    JPG格式： SOI (FFD8) | APPx 若干 | ... | SOS
    """
    pos = 2  # 跳过 SOI
    length = len(jpeg_bytes)
    app1_start = app1_end = -1
    insert_pos = 2

    while pos < length:
        if jpeg_bytes[pos] != 0xFF:
            # 坏图片，跳过
            break
        marker = jpeg_bytes[pos + 1]
        if marker == 0xDA:  # SOS
            break
        if marker == 0xE1:  # APP1
            app1_start = pos
            seg_len = struct.unpack(">H", jpeg_bytes[pos+2:pos+4])[0]
            app1_end = pos + 2 + seg_len
            return ('exist', app1_start, app1_end)
        else:
            if marker == 0xD8 or marker == 0xD9:
                pos += 2
            else:
                seg_len = struct.unpack(">H", jpeg_bytes[pos+2:pos+4])[0]
                pos += 2 + seg_len
        insert_pos = pos
    # APP1 不存在，在扫描段之后插入
    return ('new', insert_pos, insert_pos)

def pad_jpeg_via_app1_segment(jpeg_bytes, alignment=8):
    """
    使JPG长度为8的倍数。优先在APP1结尾补零，没有APP1则插入空APP1并补零。
    返回新的 JPEG 字节流。
    """
    orig_len = len(jpeg_bytes)
    padding = calculate_padding(orig_len, alignment)
    if padding == 0:
        return jpeg_bytes  # 已对齐
    app1info = find_app1_insert_position(jpeg_bytes)
    if app1info[0] == 'exist':
        print(f'padding {padding} byte')
        app1_start, app1_end = app1info[1], app1info[2]
        # 原有APP1，补零到 APP1 结尾前
        app1_len = app1_end - app1_start
        # APP1头2字节+段长2字节
        old_content_len = app1_len - 4
        new_content_len = old_content_len + padding
        new_app1_len = new_content_len + 2  # +2 for length field
        # 构造新APP1头
        new_app1_head = b'\xFF\xE1' + struct.pack('>H', new_app1_len)
        # 原头+原内容+padding
        new_app1_content = jpeg_bytes[app1_start+4:app1_end] + (b'\x00' * padding)
        new_app1_bytes = new_app1_head + new_app1_content
        # 替换原APP1
        return jpeg_bytes[:app1_start] + new_app1_bytes + jpeg_bytes[app1_end:]
    else:
        # 没有APP1，在合适位置插入新APP1（内容全为零）
        print(f'Add APP1: padding {padding + 8} byte')
        app1_inject_pos = app1info[1]
        new_app1_len = 2 + 4 + padding  # 2 for length field
        app1_segment = b'\xFF\xE1' + struct.pack('>H', new_app1_len) + b'EXIF' + (b'\x00' * padding)
        return jpeg_bytes[:app1_inject_pos] + app1_segment + jpeg_bytes[app1_inject_pos:]

def append_file_to_combined(img_bytes, combined_file_path):
    with open(combined_file_path, 'ab') as outfile:
        outfile.write(img_bytes)

def main(input_folder, output_file):
    if os.path.dirname(output_file) and not os.path.exists(os.path.dirname(output_file)):
        os.makedirs(os.path.dirname(output_file))
    print(f"Input folder: {input_folder}")
    print(f"Output file: {output_file}")
    if not os.path.exists(input_folder):
        print(f"The input directory does not exist: {input_folder}")
        return

    open(output_file, 'wb').close()
    file_cnt = 0
    for file_name in os.listdir(input_folder):
        file_path = os.path.join(input_folder, file_name)
        if not os.path.isfile(file_path):
            continue
        with open(file_path, 'rb') as infile:
            img_bytes = infile.read()
        if not is_baseline_jpeg(img_bytes):
            print(f"非baseline图片已跳过: {file_path}")
            continue
        new_img_bytes = pad_jpeg_via_app1_segment(img_bytes, alignment=8)
        current_offset = os.path.getsize(output_file)
        append_file_to_combined(new_img_bytes, output_file)
        print(f"Combined: {file_path} ({len(new_img_bytes)} bytes) @ {current_offset:#X} ")
        file_cnt = file_cnt + 1

    print(f"Done! {file_cnt} frames total")

if __name__ == "__main__":
    input_folder = r"JPEG/earth03"
    output_file = r"MJPEG/TEST.mjpeg"
    main(input_folder, output_file)

# This script will pad inside.
# ffmpeg -i earth_7_30.mp4 -r 30 -vf "format=yuvj420p" -q:v 1  mjpeg_frame/frame_%04d.jpg
