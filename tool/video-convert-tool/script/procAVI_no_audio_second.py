import struct
import os
import sys
import tkinter as tk
from tkinter import filedialog

import sys
import argparse

def build_exif_app0_segment(padlen=0):
    # 构造标准APP0段 (JFIF1.01，没有缩略图)
    # Marker(2)+Len(2)+Id(5)+Ver(2)+Units(1)+Xd(2)+Yd(2)+Xth(1)+Yth(1) = 18字节
    # padlen为内容尾补零
    # JFIF相关：0xFF 0xE0 len_hi len_lo 'JFIF\0' 0x01 0x01 0x00 0x00 0x01 0x00 0x01 0x00 0x00
    base = b'\xFF\xE0'                                   # APP0 Marker
    ex = b'JFIF' 
    length = 6 # 长度字段包括2字节自己
    seg = base + length.to_bytes(2, 'big') + ex 
    return seg

def build_exif_app1_segment(padlen=0):
    # 构造标准APP1段 
    base = b'\xFF\xE1'                                   # APP1 Marker
    ex = b'EXIF' 
    length = 6 + padlen # 长度字段包括2字节自己
    seg = base + length.to_bytes(2, 'big') + ex + (b'\0' * padlen)
    return seg

def insert_app1_to_jpeg(jpeg_data, padlen):
    """对没有APP0和APP1的JPEG，插入APP0+填充到SOI之后"""
    assert jpeg_data[:2] == b'\xFF\xD8'
    # 保留SOI
    soi = jpeg_data[:2]
    if jpeg_data[2:4] == b'\xFF\xE0':
        seg_len = int.from_bytes(jpeg_data[4:6], 'big')
        app0seg = jpeg_data[2:4+seg_len]
    else:
        app0seg = build_exif_app0_segment(padlen)
        seg_len = 0
    
    rest = jpeg_data[2+seg_len:]
    app1seg = build_exif_app1_segment(padlen)
    return soi + app0seg + app1seg + rest

def pad_appn(jpeg_data, padlen, pad_byte=b'\0'):
    '''若有APP1，则在尾部填充字节并修正长度。'''
    pos = 2
    found = False
    while pos + 4 < len(jpeg_data):
        if jpeg_data[pos] == 0xFF and jpeg_data[pos+1] == 0xE1:
            appn_type = jpeg_data[pos+1]
            seg_len = int.from_bytes(jpeg_data[pos+2:pos+4], 'big')
            seg_start = pos
            seg_end = pos + 2 + seg_len
            # 修改len，尾部补padlen字节
            new_len_word = (seg_len + padlen).to_bytes(2, 'big')
            new_appn = jpeg_data[seg_start:seg_start+2] + new_len_word + jpeg_data[seg_start+4:seg_end] + pad_byte*padlen
            result = jpeg_data[:seg_start] + new_appn + jpeg_data[seg_end:]
            return result
        elif jpeg_data[pos] == 0xFF:
            seg_len = int.from_bytes(jpeg_data[pos+2:pos+4], 'big')
            pos += 2 + seg_len
        else:
            break
    # print("APP1 NOT FOUND")
    return None # 没有

def read_chunk_header(f, offset):
    f.seek(offset)
    header = f.read(8)
    if len(header) < 8:
        return None, None, None
    chunk_id, size = struct.unpack('<4sI', header)
    return chunk_id.decode('ascii', errors='replace'), size, offset + 8

def find_root_chunks(f):
    f.seek(0)
    riff_id = f.read(4)
    riff_size = struct.unpack('<I', f.read(4))[0]
    riff_type = f.read(4)
    root_chunks = []
    offset = 12
    riff_end = 8 + riff_size
    while offset + 8 <= riff_end:
        f.seek(offset)
        chunk_header = f.read(8)
        if len(chunk_header) < 8:
            break
        chunk_id, size = struct.unpack('<4sI', chunk_header)
        chunk_id_str = chunk_id.decode('ascii', errors='replace')
        root_chunks.append((chunk_id_str, offset, size, offset+8))
        pad = size % 2
        offset += 8 + size + pad
    return riff_size, riff_end, riff_type.decode('ascii', 'replace'), root_chunks

def find_movi_chunks(f, movi_do, movi_sz):
    offset = movi_do + 4 # skip 'movi'
    end = movi_do + movi_sz
    chunks = []
    while offset + 8 <= end:
        cid, sz, doff = read_chunk_header(f, offset)
        if cid is None or sz is None or offset + 8 + sz > end:
            break
        if cid not in ('JUNK', 'LIST'):
            # 仅统计帧
            chunks.append((cid, offset, sz, offset + 8)) # (id, 头, 长度, 数据)
        pad = sz % 2
        offset += 8 + sz + pad
    return chunks

def align_avi_frames(infile, outfile, pad_byte=b'\0'):
    # --- 分析 ---
    with open(infile, 'rb') as fin:
        riff_size, riff_end, riff_type, root_chunks = find_root_chunks(fin)
        movi_idx, movi_off, movi_sz, movi_do = None, None, None, None
        idx1_idx = None
        for i, (cid, off, sz, doff) in enumerate(root_chunks):
            if cid == 'LIST':
                fin.seek(doff)
                tp = fin.read(4).decode('ascii', errors='replace')
                if tp == 'movi':
                    movi_idx = i
                    movi_off, movi_sz, movi_do = off, sz, doff
            elif cid == 'idx1':
                idx1_idx = i
        if movi_idx is None or idx1_idx is None:
            print('缺少movi或idx1')
            return
        
        # 解析movi所有帧chunk
        jpeg_chunks = find_movi_chunks(fin, movi_do, movi_sz)
        if not jpeg_chunks:
            print('未找到帧')
            return

        # --- 读取movi帧原始数据缓冲 ---
        jpeg_datas = []
        for i, (cid, coff, csz, doff) in enumerate(jpeg_chunks):
            fin.seek(doff)
            jpeg_datas.append(fin.read(csz))
    
    # --- 计算并填充补齐（调整上一帧的APP1/插入APP1） ---
    # 记录新帧的数据和所有新offset
    new_jpeg_datas = []
    new_chunk_offsets = []
    pos = None  # 当前帧在新AVI中的起始offset
    for idx, (cid, coff, csz, doff) in enumerate(jpeg_chunks):
        # 计算chunk头在新文件的位置
        if idx == 0:
            # 第一帧
            # 假定movido+4第一帧就是8字节对齐 （或者不对齐由前面JUNK方案修正）
            current_offset = None  # 暂无，最后写入计算
            new_jpeg_datas.append(jpeg_datas[0])
            new_chunk_offsets.append(None) # 占位
            print('idx ', idx, coff, csz)
            continue
        # 现在要让本帧start对齐，实际需调整上一帧内容
        next_chunk_align = 8
        # prev_new_chunk_start = 新的pos需累加
        # 先模拟前面所有帧实际物理位置
        if idx == 1:
            # 第一帧通常movido+4起始，且8字节对齐，假设不需补
            prev_chunk_start = coff
            print('idx ', idx, coff, csz, 'pre len ', len(new_jpeg_datas[-1]))
            # 算上头8字节
            prevdata_len = len(new_jpeg_datas[-1])
            prev_chunk_data_start = (prev_chunk_start + 8) if prev_chunk_start is not None else None
            # prev_chunk_end = (prev_chunk_start + 8 + prevdata_len) if prev_chunk_start is not None else None
            prev_chunk_end = prev_chunk_data_start if prev_chunk_data_start is not None else None
        else:
            prev_chunk_start = new_chunk_offsets[-1]
            # 算上头8字节
            prevdata_len = len(new_jpeg_datas[-1])
            prev_chunk_data_start = (prev_chunk_start + 8) if prev_chunk_start is not None else None
            prev_chunk_end = (prev_chunk_start + 8 + prevdata_len) if prev_chunk_start is not None else None
        
        # 下一个chunk header紧跟前一个chunk（含头+数据）后，若需pad则由前一帧JPEG尾部APP1补齐
        this_chunk_header = prev_chunk_end
        need_pad = (next_chunk_align - (this_chunk_header % next_chunk_align)) % next_chunk_align
        print(idx, need_pad, this_chunk_header)
        # 计算本帧头应在的位置
        new_this_chunk_off = this_chunk_header + need_pad
        new_chunk_offsets.append(new_this_chunk_off)
        # 填补前一帧JPEG
        last_jpeg = new_jpeg_datas[-1]
        if need_pad == 0:
            # 不补
            new_jpeg_datas[-1] = last_jpeg
        else:
            # 优先APP1扩充，否则插入APP0
            done = False
            t = pad_appn(last_jpeg, need_pad, pad_byte)
            if t is not None:
                new_jpeg_datas[-1] = t
                done = True
            else:
                # 没有，插入APP1
                t = insert_app1_to_jpeg(last_jpeg, need_pad)
                new_jpeg_datas[-1] = t
                done = True
        # 本帧内容维持不变
        new_jpeg_datas.append(jpeg_datas[idx])
    # 第一帧offset需单独算出来
    # movi实际数据部分紧跟'LIST'+size+'movi'即doff+4起始
    movi_datastart = None  # 新文件计算时定
    # 末尾不要多余
    new_jpeg_datas = new_jpeg_datas[:len(jpeg_chunks)]
    
    # --------------------------------------

    # --- 重新生成输出文件 ---
    with open(infile, 'rb') as fin, open(outfile, 'wb') as fout:
        # --- 拷贝movi前全部 ---
        # 先拷贝到movi LIST头，“movi”字段4字节为止（doff+4）
        movi_list_off = root_chunks[movi_idx][1]
        movi_list_sz = root_chunks[movi_idx][2]
        movi_do = root_chunks[movi_idx][3]
        fin.seek(0)
        # movi header区
        before_movi = fin.read(movi_do + 4)
        fout.write(before_movi)
        pos = movi_do + 4
        new_chunk_offsets[0] = pos
        # --- 写每帧chunk ---
        for idx, (cid, coff, csz, doff) in enumerate(jpeg_chunks):
            this_data = new_jpeg_datas[idx]
            # chunk header
            fout.write(cid.encode('ascii'))
            fout.write(struct.pack('<I', len(this_data)))
            pos += 8
            fout.write(this_data)
            pos += len(this_data)
            if len(this_data) % 2 == 1:
                fout.write(b'\0')
                pos += 1
            # 给出下一个chunk实际文件内偏移
            if idx + 1 < len(jpeg_chunks):
                new_chunk_offsets[idx+1] = pos
        # --- 拷贝movi后其余内容 ---
        movi_end = movi_list_off + 8 + movi_list_sz
        fin.seek(movi_end)
        rest = fin.read()
        rest_start = fout.tell()
        fout.write(rest)

        # ---- 修正 movi list size 和 RIFF size ----
    with open(outfile, 'r+b') as fout:
        # 修正 movi 区 size 字段
        fout.seek(movi_list_off + 4)
        new_movi_size = rest_start - (movi_list_off + 8)
        fout.write(struct.pack('<I', new_movi_size))
        # 修正 RIFF size 字段
        fout.seek(0, os.SEEK_END)
        filesize = fout.tell()
        fout.seek(4)
        fout.write(struct.pack('<I', filesize - 8))

        # ---- 修正 idx1 块 offset 和 size 字段 ----
        # 重新获取 chunk 信息
        _, _, _, root_chunks2 = find_root_chunks(fout)
        idx1_off = None
        idx1_sz = 0
        movi_do2 = None
        for cid, off, sz, doff in root_chunks2:
            if cid == 'LIST':
                fout.seek(doff)
                tp = fout.read(4).decode('ascii', errors='replace')
                if tp == 'movi':
                    movi_do2 = doff
            if cid == 'idx1':
                idx1_off = off
                idx1_sz = sz
        
        if idx1_off is None or movi_do2 is None:
            print('无法定位idx1')
            return

        fout.seek(idx1_off + 8)
        for idx, (cid, coff, csz, doff) in enumerate(jpeg_chunks):
            idx1ent = fout.read(16)
            if len(idx1ent) < 16: break
            ckid, flags, oldoff, oldlen = struct.unpack('<4sIII', idx1ent)
            # 新的 offset：本帧 chunk header 相对于 "movi" 字符后的 offset
            new_off = new_chunk_offsets[idx] - (movi_do2)
            # 新的 size：统一为 new_jpeg_datas[idx] 长度
            new_len = len(new_jpeg_datas[idx])
            # 重写 offset 和 size
            fout.seek(-8, 1)
            fout.write(struct.pack('<I', new_off))
            fout.write(struct.pack('<I', new_len))
        print(f"所有帧已8字节对齐，idx1已修复！\nmovi size 已修正为 {new_movi_size}，RIFF size 已修正为 {filesize - 8}")
    print('新文件：', outfile)



def main():
    parser = argparse.ArgumentParser(description="对AVI文件的其余帧对齐8字节")
    parser.add_argument('-i', '--input', required=True, help='输入的AVI文件路径')
    parser.add_argument('-o', '--output', required=True, help='输出修正后AVI文件路径')
    args = parser.parse_args()

    FILL_BYTE = b'\x00'
    try:
        align_avi_frames(args.input, args.output, pad_byte=b'\x00')
        print(f'修正完成: {args.output}')
    except Exception as e:
        print(f"处理失败: {e}")
        sys.exit(1)

if __name__ == "__main__":
    main()