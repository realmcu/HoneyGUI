"""
RTK FastLz Compression - Compatible with Windows Tool
RTK FastLz 压缩 - 与 Windows 工具兼容

This implementation uses MAX_DISTANCE=64 to match the Windows tool's behavior.
"""
from .base import CompressionAlgorithm
from formats.format_types import COMPRESS_FASTLZ
import struct

# RTK FastLz constants - MUST match Windows tool!
MAX_COPY = 32
MAX_LEN = 264  # 256 + 8
MAX_DISTANCE = 64  # ⚠️ Windows tool uses 64, not 8192!
MAX_FARDISTANCE = 65535 + MAX_DISTANCE - 1

HASH_LOG = 13
HASH_SIZE = 1 << HASH_LOG
HASH_MASK = HASH_SIZE - 1

# Instruction types
LITERAL_RUN = 1
SHORT_MATCH = 2
LONG_MATCH = 3


def hash_function(data, offset):
    """Hash function matching Windows tool"""
    if offset + 2 >= len(data):
        return 0
    
    v = data[offset] | (data[offset + 1] << 8)
    v ^= (data[offset + 1] | (data[offset + 2] << 8)) ^ (v >> (16 - HASH_LOG))
    return v & HASH_MASK


def compress_level1_rtk(input_data):
    """
    RTK FastLz Level 1 compression
    Compatible with Windows tool (MAX_DISTANCE=64)
    """
    if not input_data or len(input_data) == 0:
        return bytearray()
    
    length = len(input_data)
    ip = 0  # input pointer
    ip_bound = length - 2
    ip_limit = length - 12
    
    output = bytearray()
    
    # Sanity check
    if length < 4:
        if length > 0:
            # Create literal copy only
            output.append(length - 1)
            output.extend(input_data)
            return output
        else:
            return bytearray()
    
    # Initialize hash table
    htab = [0] * HASH_SIZE
    for i in range(HASH_SIZE):
        htab[i] = 0
    
    # Start with literal copy
    instruction_type = LITERAL_RUN
    copy = 2
    output.append(MAX_COPY - 1)  # Assuming first literal is MAX_COPY
    output.append(input_data[ip])
    ip += 1
    output.append(input_data[ip])
    ip += 1
    
    # Main compression loop
    while ip < ip_limit:
        anchor = ip
        
        # Find potential match through hash
        hval = hash_function(input_data, ip)
        ref = htab[hval]
        distance = anchor - ref
        
        # Update hash table
        htab[hval] = anchor
        
        # Verify match
        if (distance == 0 or distance >= MAX_DISTANCE or
            ref + 2 >= length or ip + 2 >= length or
            input_data[ref] != input_data[ip] or
            input_data[ref + 1] != input_data[ip + 1] or
            input_data[ref + 2] != input_data[ip + 2]):
            # No match - literal run
            instruction_type = LITERAL_RUN
        else:
            # Match found
            if copy:
                output[len(output) - copy - 1] = copy - 1
            else:
                output.pop()
            copy = 0
            
            distance -= 1
            ip = anchor + 3
            ref += 3
            
            # Extend match
            while ip < ip_bound and ref < length:
                if input_data[ref] != input_data[ip]:
                    break
                ref += 1
                ip += 1
            
            match_len = ip - anchor
            
            if (match_len - 2) < 7:
                instruction_type = SHORT_MATCH
            else:
                instruction_type = LONG_MATCH
        
        # Output instruction
        if instruction_type == LITERAL_RUN:
            output.append(input_data[anchor])
            ip = anchor + 1
            copy += 1
            if copy == MAX_COPY:
                copy = 0
                output.append(MAX_COPY - 1)
        
        elif instruction_type == SHORT_MATCH:
            match_len = ip - anchor
            output.append(((match_len - 2) << 5) + (distance >> 8))
            output.append(distance & 255)
            
            # Update hash at match boundary
            if ip - 1 < length:
                hval = hash_function(input_data, ip - 1)
                htab[hval] = ip - 1
            if ip - 2 >= 0:
                hval = hash_function(input_data, ip - 2)
                htab[hval] = ip - 2
            
            # Assuming literal copy
            output.append(MAX_COPY - 1)
        
        elif instruction_type == LONG_MATCH:
            match_len = ip - anchor
            
            # Handle overflow
            while match_len > MAX_LEN:
                output.append((7 << 5) + (distance >> 8))
                output.append(MAX_LEN - 2 - 7 - 2)
                output.append(distance & 255)
                match_len -= MAX_LEN - 2
            
            match_len -= 2
            if match_len < 7:
                output.append((match_len << 5) + (distance >> 8))
                output.append(distance & 255)
            else:
                output.append((7 << 5) + (distance >> 8))
                output.append(match_len - 7)
                output.append(distance & 255)
            
            # Update hash at match boundary
            if ip - 1 < length:
                hval = hash_function(input_data, ip - 1)
                htab[hval] = ip - 1
            if ip - 2 >= 0:
                hval = hash_function(input_data, ip - 2)
                htab[hval] = ip - 2
            
            # Assuming literal copy
            output.append(MAX_COPY - 1)
    
    # Left-over as literal copy
    ip_bound += 1
    while ip <= ip_bound and ip < length:
        output.append(input_data[ip])
        ip += 1
        copy += 1
        if copy == MAX_COPY:
            copy = 0
            output.append(MAX_COPY - 1)
    
    # Adjust last copy length
    if copy:
        output[len(output) - copy - 1] = copy - 1
    else:
        if len(output) > 0:
            output.pop()
    
    return output


class RTKFastLzCompression(CompressionAlgorithm):
    """
    RTK FastLz compression algorithm
    Compatible with Windows tool (MAX_DISTANCE=64)
    """
    
    def compress(self, pixel_data, width, height, pixel_bytes):
        """
        Compress using RTK FastLz algorithm (MAX_DISTANCE=64)
        """
        # Calculate bytes per line
        bytes_per_line = width * pixel_bytes
        
        # Compress each line
        compressed_lines = []
        for line in range(height):
            line_start = line * bytes_per_line
            line_end = line_start + bytes_per_line
            line_data = pixel_data[line_start:line_end]
            
            # Compress using RTK FastLz Level 1
            compressed_line = compress_level1_rtk(bytes(line_data))
            compressed_lines.append(compressed_line)
        
        # Build compressed data with metadata
        compressed_data = bytearray()
        line_offsets = []
        
        # Calculate final offsets
        temp_size = sum(len(line) for line in compressed_lines)
        imdc_offset = 12 + (height + 1) * 4
        
        # Metadata = last two offsets
        last_line_offset = imdc_offset + temp_size - len(compressed_lines[-1])
        end_offset = imdc_offset + temp_size
        
        # Build compressed data
        for i, compressed_line in enumerate(compressed_lines):
            line_offsets.append(len(compressed_data))
            
            if i == 0:
                # Add 8-byte metadata for first line
                compressed_data.extend(struct.pack('<I', last_line_offset))
                compressed_data.extend(struct.pack('<I', end_offset))
            
            compressed_data.extend(compressed_line)
        
        return compressed_data, line_offsets, {
            'feature_1': 0,
            'feature_2': 0
        }
    
    def get_algorithm_type(self):
        return COMPRESS_FASTLZ
