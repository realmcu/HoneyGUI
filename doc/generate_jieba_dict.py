#
# Copyright (c) 2026, Realtek Semiconductor Corporation
#
# SPDX-License-Identifier: MIT
#

# run cmd: 
# python generate_jieba_dict.py ./cn ./cn/word_dict.txt
# note: ./cn is source_dir; ./cn/word_dict.txt is output_file


import os
import re
import jieba
import argparse
from collections import Counter
from sphinx.util import logging

# 初始化记录器
logger = logging.getLogger(__name__)

# 用于存储词条和词频, 使用Counter来计数
word_counter = Counter()

# 定义正则表达式用于清理文本
remove_chars = re.compile(r"[^\w\s]", re.UNICODE)  # 保留字母、数字和空白字符

def extract_words_from_file(file_path):
    try:
        with open(file_path, 'r', encoding='utf-8') as file:
            for line in file:
                # 清理无用字符
                clean_line = remove_chars.sub('', line.strip())
                # 使用jieba进行初步分词处理
                words = jieba.lcut(clean_line)
                # 进一步检测长词
                detect_long_words(words)
                word_counter.update(words)
    except Exception as e:
        logger.error(f"Error reading file {file_path}: {e}")

def detect_long_words(words):
    # 我们使用滑动窗口方法来检测多个连续词可能组成的长词
    # 比如，可以设置一个滑动窗口的大小范围，尝试不同的组合长度
    max_window_size = 3  # 假设长词最多由3个连续的词组成

    for size in range(2, max_window_size + 1):
        for i in range(len(words) - size + 1):
            long_word = ''.join(words[i:i + size])
            if len(long_word) > 1:  # 忽略单字符长词
                word_counter[long_word] += 1

def scan_files_and_extract_words(directory):
    for root, _, files in os.walk(directory):
        for filename in files:
            if filename.lower().endswith(('.rst', '.md')):  # 处理 .rst 和 .md 文件
                file_path = os.path.join(root, filename)
                extract_words_from_file(file_path)
            else:
                logger.info(f"Skipping non-rst and non-md file: {filename}")

def save_word_dict(output_path):
    try:
        with open(output_path, 'w', encoding='utf-8') as file:
            for word, freq in word_counter.most_common():
                # 这里我们用 x 作为默认的词性
                file.write(f"{word} {freq} x\n")
    except Exception as e:
        logger.error(f"Error writing to file {output_path}: {e}")

def main():
    parser = argparse.ArgumentParser(description='Extract word dictionary from Sphinx documentation')
    parser.add_argument('source_dir', type=str, help='The source directory of Sphinx documents')
    parser.add_argument('output_file', type=str, help='The output file to save the word dictionary')

    args = parser.parse_args()

    logger.info("Starting word extraction process...")
    scan_files_and_extract_words(args.source_dir)
    save_word_dict(args.output_file)
    logger.info(f"Word dictionary saved to {args.output_file}")

if __name__ == "__main__":
    main()
