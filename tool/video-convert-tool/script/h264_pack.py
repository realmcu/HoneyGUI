# -*- coding: utf-8 -*-
import struct
import os
from bitstring import BitStream

# -----------------------------
# 工具函数：起始码与 EBSP/RBSP
# -----------------------------

def find_start_code_pos(data, start=0):
    """
    返回下一个起始码位置与长度：
    - 若找到 0x00000001，返回 (pos, 4)
    - 若找到 0x000001，返回 (pos, 3)
    - 若未找到，返回 (len(data), 0)
    """
    # 优先匹配 4 字节，避免把 0x00000001 的最后 3 字节误识为 0x000001
    pos4 = data.find(b'\x00\x00\x00\x01', start)
    pos3 = data.find(b'\x00\x00\x01', start)

    if pos4 != -1 and (pos3 == -1 or pos4 < pos3):
        return pos4, 4
    if pos3 != -1:
        return pos3, 3
    return len(data), 0


def next_nal_bounds(data, start=0):
    """
    返回当前 NAL 的起始与结束坐标：
    - (nal_start, nal_header_pos, payload_start, payload_end)
    - 若无后续 NAL，payload_end 为 len(data)
    """
    sc_pos, sc_len = find_start_code_pos(data, start)
    if sc_len == 0:
        return len(data), None, None, None

    # NAL header 紧跟在起始码后 1 字节
    nal_header_pos = sc_pos + sc_len
    # 负载开始位置
    payload_start = nal_header_pos + 1

    # 找下一个起始码位置，作为本 NAL 的结束边界
    next_sc_pos, _ = find_start_code_pos(data, payload_start)
    payload_end = next_sc_pos if next_sc_pos != len(data) else len(data)

    return sc_pos, nal_header_pos, payload_start, payload_end


def ebsp_to_rbsp(ebsp):
    """
    Annex B EBSP -> RBSP:
    去除 emulation prevention byte (0x03) 序列：00 00 03 -> 00 00
    仅在出现 0x000003 时移除中间的 0x03。
    """
    out = bytearray()
    zeros = 0
    for b in ebsp:
        if zeros >= 2 and b == 0x03:
            # 丢弃该字节，重置计数
            zeros = 0
            continue
        out.append(b)
        if b == 0x00:
            zeros += 1
        else:
            zeros = 0
    return bytes(out)


# -----------------------------
# 比特读取（RBSP 解析）
# -----------------------------

class BitReader:
    def __init__(self, data):
        self.bs = BitStream(bytes=data)

    def read_bits(self, n):
        return self.bs.read(f'uint:{n}')

    def read_bit(self):
        return self.read_bits(1)

    def read_ue(self):
        # Exp-Golomb UE
        zeros = 0
        while True:
            bit = self.read_bit()
            if bit == 0:
                zeros += 1
            else:
                break
        if zeros == 0:
            return 0
        info_bits = self.read_bits(zeros)
        return (1 << zeros) - 1 + info_bits

    def read_se(self):
        ue = self.read_ue()
        # Map to signed
        return -((ue + 1) // 2) if (ue & 1) == 0 else ((ue + 1) // 2)


# -----------------------------
# H.264 头结构
# -----------------------------

class H264Header:
    def __init__(self, width, height, frame_num, fps, size):
        self.symbol = b"H264"
        self.width = width
        self.height = height
        self.frame_num = frame_num
        self.frame_time = round(1000 / fps) if fps > 0 else 0
        self.size = size

    def pack(self):
        # 可选调试打印
        # print(f"Symbol: {self.symbol}")
        # print(f"Width: {self.width}")
        # print(f"Height: {self.height}")
        # print(f"Frame Count: {self.frame_num}")
        # print(f"Frame Time: {self.frame_time}")
        # print(f"Size: {self.size}")
        return struct.pack('<4s5I', self.symbol, self.width, self.height, self.frame_num, self.frame_time, self.size)


# -----------------------------
# SPS 解析（分辨率）
# -----------------------------

def parse_sps_from_payload(sps_payload):
    """
    传入 SPS 的 NAL 负载（不含起始码和 NAL header），解析分辨率。
    更严谨：根据 chroma_format_idc 确定裁剪单位。
    """
    rbsp = ebsp_to_rbsp(sps_payload)
    br = BitReader(rbsp)

    # NAL header 已不在 rbsp 中，这里直接从 profile_idc 开始
    profile_idc = br.read_bits(8)
    br.read_bits(8)  # constraint_set_flags + reserved_zero_2bits
    level_idc = br.read_bits(8)
    seq_parameter_set_id = br.read_ue()

    chroma_format_idc = 1
    separate_colour_plane_flag = 0
    if profile_idc in (100, 110, 122, 244, 44, 83, 86, 118, 128, 138, 139, 134):
        chroma_format_idc = br.read_ue()
        if chroma_format_idc == 3:
            separate_colour_plane_flag = br.read_bit()
        br.read_ue()  # bit_depth_luma_minus8
        br.read_ue()  # bit_depth_chroma_minus8
        br.read_bit()  # qpprime_y_zero_transform_bypass_flag
        seq_scaling_matrix_present_flag = br.read_bit()
        if seq_scaling_matrix_present_flag:
            # 跳过 scaling list
            count = 8 if chroma_format_idc != 3 else 12
            for i in range(count):
                present = br.read_bit()
                if present:
                    # skip scaling list
                    last_scale = 8
                    next_scale = 8
                    size = 16 if i < 6 else 64
                    for _ in range(size):
                        if next_scale != 0:
                            delta_scale = br.read_se()
                            next_scale = (last_scale + delta_scale + 256) % 256
                        last_scale = next_scale if next_scale != 0 else last_scale

    log2_max_frame_num_minus4 = br.read_ue()
    pic_order_cnt_type = br.read_ue()
    if pic_order_cnt_type == 0:
        _ = br.read_ue()  # log2_max_pic_order_cnt_lsb_minus4
    elif pic_order_cnt_type == 1:
        br.read_bit()  # delta_pic_order_always_zero_flag
        br.read_se()   # offset_for_non_ref_pic
        br.read_se()   # offset_for_top_to_bottom_field
        num_ref_frames_in_pic_order_cnt_cycle = br.read_ue()
        for _ in range(num_ref_frames_in_pic_order_cnt_cycle):
            br.read_se()

    br.read_ue()   # max_num_ref_frames
    br.read_bit()  # gaps_in_frame_num_value_allowed_flag

    pic_width_in_mbs_minus1 = br.read_ue()
    pic_height_in_map_units_minus1 = br.read_ue()

    frame_mbs_only_flag = br.read_bit()
    if frame_mbs_only_flag == 0:
        br.read_bit()  # mb_adaptive_frame_field_flag
    br.read_bit()      # direct_8x8_inference_flag

    frame_cropping_flag = br.read_bit()
    frame_crop_left_offset = frame_crop_right_offset = 0
    frame_crop_top_offset = frame_crop_bottom_offset = 0
    if frame_cropping_flag:
        frame_crop_left_offset = br.read_ue()
        frame_crop_right_offset = br.read_ue()
        frame_crop_top_offset = br.read_ue()
        frame_crop_bottom_offset = br.read_ue()

    # 计算宽高
    width = (pic_width_in_mbs_minus1 + 1) * 16
    height = (pic_height_in_map_units_minus1 + 1) * 16
    if frame_mbs_only_flag == 0:
        height *= 2  # 帧-场编码（MBAFF/PAFF）

    # 裁剪单位（H.264 标准 7.4.2.1.1）
    if chroma_format_idc == 0:
        crop_unit_x = 1
        crop_unit_y = 2 - frame_mbs_only_flag
    elif chroma_format_idc == 1:
        crop_unit_x = 2
        crop_unit_y = 2 * (2 - frame_mbs_only_flag)
    elif chroma_format_idc == 2:
        crop_unit_x = 2
        crop_unit_y = 2 - frame_mbs_only_flag
    else:  # chroma_format_idc == 3
        crop_unit_x = 1
        crop_unit_y = 2 - frame_mbs_only_flag

    width -= (frame_crop_left_offset + frame_crop_right_offset) * crop_unit_x
    height -= (frame_crop_top_offset + frame_crop_bottom_offset) * crop_unit_y

    return max(0, width), max(0, height)


# -----------------------------
# Slice Header 解析（用于帧计数）
# -----------------------------

def parse_slice_header_fields(slice_payload, sps_ctx):
    """
    简化的 slice header 解析：
    - 返回 first_mb_in_slice, slice_type, frame_num
    - 仅用于判断新帧开始。sps_ctx 可传入 log2_max_frame_num_minus4 等，但此处简化不依赖。
    """
    rbsp = ebsp_to_rbsp(slice_payload)
    br = BitReader(rbsp)

    first_mb_in_slice = br.read_ue()
    slice_type = br.read_ue()  # 0..2 P, 3..5 B, 6..8 I（mod 5 等价）
    pic_parameter_set_id = br.read_ue()
    # 以下字段解析省略；若需更严谨判断可继续解析 frame_num 等
    # 注意：frame_num 的位宽是 log2_max_frame_num_minus4+4（来自 SPS），这里略过
    return first_mb_in_slice, slice_type, None


# -----------------------------
# 帧计数（更可靠）
# -----------------------------

def count_frames_annexb(data):
    """
    更可靠的帧计数：
    - 若码流含 AUD(nal_unit_type=9)，按访问单元计数
    - 否则：每遇到 slice（nal_unit_type in {1,5}）且 first_mb_in_slice == 0，则 +1
    """
    pos = 0
    frame_count = 0
    au_started = False
    aud_seen = False

    while True:
        nal_start, nal_header_pos, payload_start, payload_end = next_nal_bounds(data, pos)
        if nal_header_pos is None:
            break

        nal_header = data[nal_header_pos]
        nal_unit_type = nal_header & 0x1F

        payload = data[payload_start:payload_end]

        if nal_unit_type == 9:  # AUD
            aud_seen = True
            if au_started:
                frame_count += 1
            au_started = True

        elif nal_unit_type in (1, 5):  # 非 IDR / IDR slice
            if aud_seen:
                # 有 AUD 的情况下，按 AU 计数，遇到 slice 表示 AU 有实际图像
                au_started = True
            else:
                # 无 AUD：用 first_mb_in_slice 判断新帧开始
                first_mb_in_slice, _, _ = parse_slice_header_fields(payload, None)
                if first_mb_in_slice == 0:
                    frame_count += 1

        # 推进到下一个 NAL
        pos = payload_end

    # 文件结束时，如果用 AUD 方式且已开始 AU，需要收尾计 1 帧
    if aud_seen and au_started:
        frame_count += 1

    return frame_count


# -----------------------------
# 主流程：读取、解析、写输出
# -----------------------------

def add_header_to_h264(input_file_path, output_file_path, fps=24):
    # 读取文件
    with open(input_file_path, "rb") as infile:
        h264_data = infile.read()

    # 解析 SPS 分辨率
    width, height = 0, 0
    pos = 0
    while True:
        nal_start, nal_header_pos, payload_start, payload_end = next_nal_bounds(h264_data, pos)
        if nal_header_pos is None:
            break
        nal_unit_type = h264_data[nal_header_pos] & 0x1F
        if nal_unit_type == 7:  # SPS
            sps_payload = h264_data[payload_start:payload_end]
            width, height = parse_sps_from_payload(sps_payload)
            break
        pos = payload_end

    # 计数帧数（更可靠）
    frame_num = count_frames_annexb(h264_data)

    # 数据大小（不含自定义头）
    size = len(h264_data)

    # 构建并写出头 + 原始数据
    header = H264Header(width, height, frame_num, fps, size)
    packed_header = header.pack()

    with open(output_file_path, "wb") as outfile:
        outfile.write(packed_header)
        outfile.write(h264_data)

    print(f"解析到分辨率: {width}x{height}, 帧数: {frame_num}, 帧时间(ms): {header.frame_time}")
    print(f"已写入文件: {output_file_path}")



if __name__ == "__main__":
    # Example usage
    input_file = "birds.h264"  # 输入的 H.264 文件路径
    output_file = "birds_header.h264"
    fps = 24
    
    # 将头添加到 H.264 文件中
    add_header_to_h264(input_file, output_file, fps)

    print(f"Header added and file saved as {output_file}")
