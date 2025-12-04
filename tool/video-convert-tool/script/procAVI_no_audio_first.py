import struct
import os
import tkinter as tk
from tkinter import filedialog

import sys
import argparse


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
    try:
        riff_id_str = riff_id.decode('ascii')
    except UnicodeDecodeError:
        print(f"[错误] 文件头非ascii: riff_id={riff_id}")
        return None, None, None, None
    if riff_id_str != "RIFF":
        print(f"[错误] 文件非RIFF头: id={riff_id_str}")
        return None, None, None, None

    riff_size_bytes = f.read(4)
    if len(riff_size_bytes) < 4:
        print("[错误] 未读到完整RIFF size")
        return None, None, None, None
    riff_size = struct.unpack('<I', riff_size_bytes)[0]
    riff_type = f.read(4)
    try:
        riff_type_str = riff_type.decode('ascii')
    except UnicodeDecodeError:
        riff_type_str = repr(riff_type)
    root_chunks = []
    offset = 12
    riff_end = 8 + riff_size
    while offset + 8 <= riff_end:
        f.seek(offset)
        chunk_header = f.read(8)
        if len(chunk_header) < 8:
            break
        chunk_id, size = struct.unpack('<4sI', chunk_header)
        try:
            chunk_id_str = chunk_id.decode('ascii')
        except UnicodeDecodeError:
            print(f"[错误] chunk_id@{offset:x}不是ascii")
            break
        root_chunks.append((chunk_id_str, offset, size, offset+8))
        pad = size % 2
        offset += 8 + size + pad
    return riff_size, riff_end, riff_type_str, root_chunks

def find_first_movi_chunk(f, movi_do, movi_sz):
    offset = movi_do + 4  # 跳过 'movi'
    end = movi_do + movi_sz
    while offset + 8 <= end:
        cid, sz, doff = read_chunk_header(f, offset)
        if cid not in ('LIST', 'JUNK'):
            return cid, offset, offset + 8
        pad = sz % 2
        offset += 8 + sz + pad
    return None, None, None

def adjust_junk_for_8byte_alignment(infile, outfile, fill_byte=b'\x00'):
    # 解析AVI
    with open(infile, 'rb') as fin:
        riff_size, riff_end, riff_type, root_chunks = find_root_chunks(fin)
        if riff_size is None:
            print("输入文件格式异常")
            return
        movi_idx, movi_off, movi_sz, movi_do = None, None, None, None
        for i, (cid, off, sz, doff) in enumerate(root_chunks):
            if cid == 'LIST':
                fin.seek(doff)
                tp = fin.read(4).decode('ascii', errors='replace')
                if tp == 'movi':
                    movi_idx = i
                    movi_off, movi_sz, movi_do = off, sz, doff
                    break
        if movi_idx is None:
            print("未找到 movi 块。")
            return

        # 找movi前最近的JUNK
        junk_idx = None
        for i in reversed(range(movi_idx)):
            if root_chunks[i][0] == 'JUNK':
                junk_idx = i
                break
        if junk_idx is None:
            print("未检测到正确的 JUNK 块。")
            return
        junk_cid, junk_off, junk_sz, junk_do = root_chunks[junk_idx]

        # 查首帧
        cid, first_chunk_off, first_chunk_data = find_first_movi_chunk(fin, movi_do, movi_sz)
        print(f"\nAVI结构：{infile}")
        print(f"[RIFF] @0x00000000, size: {riff_size:8d}, type:{riff_type}")
        for idx, (cid_, off_, sz_, doff_) in enumerate(root_chunks):
            extra = ""
            if cid_ == "LIST":
                fin.seek(doff_)
                tp = fin.read(4).decode('ascii', errors='replace')
                extra = f", type:{tp}"
            print(f"  {cid_:4}  @0x{off_:08X}, size:{sz_:8d}{extra}")

        print(f"\n首帧: {cid} @0x{first_chunk_off:08X} data首地址 @0x{first_chunk_data:08X}")
        needpad = (8 - (first_chunk_data % 8)) % 8
        if needpad == 0:
            print(">> 已8字节对齐，无需调整。")
            with open(infile,'rb') as fin2, open(outfile,'wb') as fout2:
                while True:
                    chunk = fin2.read(1048576)
                    if not chunk: break
                    fout2.write(chunk)
            return

        print(f">> 需调整，当前data首地址与8字节对齐相差 {needpad} 字节。将修正 JUNK @0x{junk_off:08X}。")
        new_junk_sz = junk_sz + needpad
        new_riff_size = riff_size + needpad

    # ------- 写出修正文件 -------
    with open(infile, 'rb') as fin, open(outfile, 'wb') as fout:
        # 先写头（严格field-by-field防错位）
        fin.seek(0)
        riff_id = fin.read(4)       # RIFF
        old_size = fin.read(4)      # 原size，舍弃
        riff_type = fin.read(4)     # 'AVI '
        fout.write(riff_id)
        fout.write(struct.pack('<I', new_riff_size))
        fout.write(riff_type)
        pos = 12
        for idx, (cid, off, sz, doff) in enumerate(root_chunks):
            # 拷贝chunk header
            fin.seek(off)
            chunk_header = fin.read(8)
            # 如果JUNK块，修正size
            if idx == junk_idx:
                chunk_header = chunk_header[:4] + struct.pack('<I', new_junk_sz)
            fout.write(chunk_header)
            pos += 8

            # 拷贝数据
            fin.seek(doff)
            if idx == junk_idx:
                fout.write(fill_byte * new_junk_sz)
                fin.seek(sz, 1)
                pos += new_junk_sz
            else:
                buf = fin.read(sz)
                fout.write(buf)
                pos += sz
            # 补齐pad
            if sz % 2 == 1:
                padding = fin.read(1)
                fout.write(padding)
                pos += 1
        # 末尾补写全部剩余数据（如存在）
        remain = fin.read()
        if remain:
            fout.write(remain)

    print(f"JUNK已调整为 {new_junk_sz} 字节，RIFF size已修正为 {new_riff_size}。")
    print(f"新文件生成: {outfile}")

    # -- 校验文件头 --
    with open(outfile, 'rb') as fin:
        riff_size2, _, _, root_chunks2 = find_root_chunks(fin)
        movi_idx2, movi_do2, movi_sz2 = None, None, None
        for i, (cid, off, sz, doff) in enumerate(root_chunks2):
            if cid == 'LIST':
                fin.seek(doff)
                tp = fin.read(4).decode('ascii', errors='replace')
                if tp == 'movi':
                    movi_idx2 = i
                    movi_do2 = doff
                    movi_sz2 = sz
                    break
        if movi_idx2 is not None:
            cid2, first_chunk_off2, first_chunk_data2 = find_first_movi_chunk(fin, movi_do2, movi_sz2)
            print(f"校验: 新第一帧chunk {cid2} data首地址: 0x{first_chunk_data2:08X}, 8字节对齐: {first_chunk_data2 % 8 == 0}")
        else:
            print("校验: 新movi未找到。")




def main():
    parser = argparse.ArgumentParser(description="对AVI文件的JUNK区对齐8字节")
    parser.add_argument('-i', '--input', required=True, help='输入的AVI文件路径')
    parser.add_argument('-o', '--output', required=True, help='输出修正后AVI文件路径')
    args = parser.parse_args()

    FILL_BYTE = b'\x00'
    try:
        adjust_junk_for_8byte_alignment(args.input, args.output, fill_byte=FILL_BYTE)
        print(f'修正完成: {args.output}')
    except Exception as e:
        print(f"处理失败: {e}")
        sys.exit(1)

if __name__ == "__main__":
    main()