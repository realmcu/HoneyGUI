#!/usr/bin/env python

import sys
import os
import struct
from collections import namedtuple
from functools import cmp_to_key

import argparse

# Constants
ALIGNMENT_4 = 4
ALIGNMENT_64 = 64

parser = argparse.ArgumentParser()
parser.add_argument('rootdir', type=str, help='the path to rootfs or binary file to extract')
parser.add_argument('output', type=argparse.FileType('wb'), nargs='?', help='output file name')
parser.add_argument('--dump', action='store_true', help='dump the fs hierarchy')
parser.add_argument('--binary', action='store_true', help='output binary file')
parser.add_argument('--addr', default='0', help='set the base address of the binary file, default to 0.')
parser.add_argument('--extract', action='store_true', help='extract files from binary romfs')
parser.add_argument('--output-dir', type=str, default='extracted_romfs', help='output directory for extracted files')


def pad_to_alignment(data, alignment):
    """Pad data to specified alignment with null bytes."""
    remainder = len(data) % alignment
    if remainder != 0:
        return data + b'\0' * (alignment - remainder)
    return data


def pad_string_to_alignment(string, alignment):
    """Pad string to specified alignment with null bytes."""
    remainder = len(string) % alignment
    if remainder != 0:
        return string + '\0' * (alignment - remainder)
    return string


class File(object):
    """Represents a file in the ROM filesystem."""
    
    def __init__(self, name):
        self._name = name
        try:
            with open(name, 'rb') as f:
                self._data = f.read()
        except IOError as e:
            print(f"Error reading file {name}: {e}", file=sys.stderr)
            raise

    @property
    def name(self):
        return self._name

    @property
    def c_name(self):
        return '_' + self._name.replace('.', '_')

    @property
    def bin_name(self):
        return pad_string_to_alignment(self._name, ALIGNMENT_4)

    def c_data(self, prefix=''):
        """Get the C code representation of the file content."""
        head = 'static const rt_uint8_t %s[] = {\n' % (prefix + self.c_name)
        tail = '\n};'
        return head + ','.join(('0x%02x' % i for i in self._data)) + tail

    @property
    def entry_size(self):
        return len(self._data)

    def bin_data(self, base_addr=0x0):
        return bytes(self._data)

    def dump(self, indent=0):
        print('%s%s' % (' ' * indent, self._name))


class Folder(object):
    """Represents a folder in the ROM filesystem."""
    
    bin_fmt = struct.Struct('IIII')
    bin_item = namedtuple('dirent', 'type, name, data, size')

    def __init__(self, name):
        self._name = name
        self._children = []

    @property
    def name(self):
        return self._name

    @property
    def c_name(self):
        return '_' + self._name

    @property
    def bin_name(self):
        return pad_string_to_alignment(self._name, ALIGNMENT_4)

    def walk(self):
        """Walk through directory tree and build file structure."""
        for ent in os.listdir('.'):
            if os.path.isdir(ent):
                cwd = os.getcwd()
                d = Folder(ent)
                os.chdir(os.path.join(cwd, ent))
                d.walk()
                os.chdir(cwd)
                self._children.append(d)
            else:
                self._children.append(File(ent))

    def sort(self):
        """Sort children alphabetically and recursively."""
        def _sort(x, y):
            if x.name == y.name:
                return 0
            elif x.name > y.name:
                return 1
            else:
                return -1
        self._children.sort(key=cmp_to_key(_sort))

        for c in self._children:
            if isinstance(c, Folder):
                c.sort()

    def dump(self, indent=0):
        """Print directory structure."""
        print('%s%s' % (' ' * indent, self._name))
        for c in self._children:
            c.dump(indent + 1)

    def c_data(self, prefix=''):
        """Get the C code representation of the folder (recursive)."""
        dhead = 'static const struct romfs_dirent %s[] = {\n' % (prefix + self.c_name)
        dtail = '\n};'
        body_fmt = '    {{{type}, "{name}", (rt_uint8_t *){data}, sizeof({data})/sizeof({data}[0])}}'
        
        cpf = prefix + self.c_name
        body_list = []
        payload_list = []
        
        for c in self._children:
            if isinstance(c, File):
                tp = 'ROMFS_DIRENT_FILE'
            elif isinstance(c, Folder):
                tp = 'ROMFS_DIRENT_DIR'
            else:
                assert False, 'Unknown instance:%s' % str(c)
            
            body_list.append(body_fmt.format(type=tp,
                                             name=c.name,
                                             data=cpf + c.c_name))
            payload_list.append(c.c_data(prefix=cpf))

        payload_list.append(dhead + ',\n'.join(body_list) + dtail)
        return '\n\n'.join(payload_list)

    @property
    def entry_size(self):
        return len(self._children)

    def bin_data(self, base_addr, resource_defines):
        """Generate binary data for folder (recursive).
        
        Args:
            base_addr: Base address for this folder
            resource_defines: Dictionary to store resource definitions
            
        Returns:
            bytes: Binary data for this folder
        """
        dirent_list = []
        payload_base = base_addr + self.bin_fmt.size * self.entry_size
        virtual_length = payload_base
        payload_list = []
        
        for c in self._children:
            if isinstance(c, File):
                tp = 0  # ROMFS_DIRENT_FILE
            elif isinstance(c, Folder):
                tp = 1  # ROMFS_DIRENT_DIR
            else:
                assert False, 'Unknown instance:%s' % str(c)

            name = bytes(c.bin_name, 'utf8')
            name_addr = virtual_length
            virtual_length += len(name)

            data_addr = virtual_length
            
            if isinstance(c, File):
                add_data = b''
                if (data_addr + 8) % ALIGNMENT_64 != 0:
                    add_data = b'\0' * (ALIGNMENT_64 - (data_addr + 8) % ALIGNMENT_64)
                data_addr = data_addr + len(add_data)
                
                # Store resource definitions
                offset = data_addr - resource_defines['base_addr']
                macro_name = str(c._name).replace('.', '_').upper()
                resource_defines['if_defs'].append(
                    f"#define   {macro_name:<40}(void *)(resource_root + 0x{offset:08x})"
                )
                resource_defines['else_defs'].append(
                    f"#define   {macro_name:<40}(void *)(0x{data_addr:08x})"
                )
                
                data = add_data + c.bin_data(virtual_length)
            else:
                data = c.bin_data(virtual_length, resource_defines)
            
            virtual_length += len(data)

            dirent_list.append(self.bin_fmt.pack(*self.bin_item(
                type=tp,
                name=name_addr,
                data=data_addr,
                size=c.entry_size
            )))
            payload_list.extend((name, data))

        return bytes().join(dirent_list) + bytes().join(payload_list)


def get_c_data(tree):
    """Generate C code representation of the ROM filesystem."""
    root_dirent_fmt = '''/* Generated by mkromfs. Edit with caution. */
#include <rtthread.h>
#include <dfs_romfs.h>

{data}

const struct romfs_dirent {name} = {{
    ROMFS_DIRENT_DIR, "/", (rt_uint8_t *){rootdirent}, sizeof({rootdirent})/sizeof({rootdirent}[0])
}};
'''
    return root_dirent_fmt.format(name='romfs_root',
                                  rootdirent=tree.c_name,
                                  data=tree.c_data())


def get_bin_data(tree, base_addr):
    """Generate binary representation of the ROM filesystem.
    
    Args:
        tree: Root folder tree
        base_addr: Base address for the filesystem
        
    Returns:
        tuple: (binary_data, header_file_content)
    """
    resource_defines = {
        'base_addr': base_addr,
        'if_defs': [],
        'else_defs': []
    }
    
    virtual_length = base_addr + Folder.bin_fmt.size
    name = bytes('/\0\0\0', 'utf8')
    name_addr = virtual_length
    virtual_length += len(name)
    data_addr = virtual_length
    
    # Root entry
    data = Folder.bin_fmt.pack(*Folder.bin_item(
        type=1,
        name=name_addr,
        data=data_addr,
        size=tree.entry_size
    ))
    data = data + name

    tree_data = tree.bin_data(virtual_length, resource_defines)
    
    # Generate header file content
    header_content = '#if defined _HONEYGUI_SIMULATOR_\n'
    header_content += 'extern unsigned char *resource_root;\n\n'
    header_content += '\n'.join(resource_defines['if_defs'])
    header_content += '\n\n#else\n'
    header_content += '\n'.join(resource_defines['else_defs'])
    header_content += '\n\n#endif\n'
    
    return (data + tree_data), header_content


class RomfsExtractor(object):
    """Extract files from binary ROM filesystem."""
    
    DIRENT_SIZE = 16  # sizeof(struct romfs_dirent)
    DIRENT_FILE = 0
    DIRENT_DIR = 1
    
    def __init__(self, binary_data, base_addr=None):
        self.data = binary_data
        if base_addr is None:
            self.base_addr = self.detect_base_addr()
        else:
            self.base_addr = base_addr
        print(f"Using base address: 0x{self.base_addr:08x}")
    
    def detect_base_addr(self):
        """Auto-detect base address from root dirent."""
        # Read root dirent name address (offset 4)
        name_addr = self.read_uint32(4)
        # Root name should be at offset 16 (after root dirent)
        # So base_addr = name_addr - 16
        detected = name_addr - 16
        return detected
    
    def read_uint32(self, offset):
        """Read uint32 from binary data."""
        return struct.unpack('I', self.data[offset:offset+4])[0]
    
    def read_string(self, offset):
        """Read null-terminated string from binary data."""
        end = offset
        while end < len(self.data) and self.data[end] != 0:
            end += 1
        return self.data[offset:end].decode('utf8', errors='ignore')
    
    def extract_dirent(self, offset, output_dir):
        """Extract directory entry recursively."""
        entry_type = self.read_uint32(offset)
        name_addr = self.read_uint32(offset + 4) - self.base_addr
        data_addr = self.read_uint32(offset + 8) - self.base_addr
        size = self.read_uint32(offset + 12)
        
        name = self.read_string(name_addr)
        
        if entry_type == self.DIRENT_FILE:
            # Extract file
            file_data = self.data[data_addr:data_addr + size]
            file_path = os.path.join(output_dir, name)
            print(f"Extracting file: {file_path} ({size} bytes)")
            with open(file_path, 'wb') as f:
                f.write(file_data)
        
        elif entry_type == self.DIRENT_DIR:
            # Extract directory
            dir_path = os.path.join(output_dir, name)
            if name != '/':
                os.makedirs(dir_path, exist_ok=True)
                print(f"Creating directory: {dir_path}")
            else:
                dir_path = output_dir
            
            # Process all entries in this directory
            for i in range(size):
                entry_offset = data_addr + i * self.DIRENT_SIZE
                self.extract_dirent(entry_offset, dir_path)
    
    def extract(self, output_dir):
        """Extract entire filesystem."""
        os.makedirs(output_dir, exist_ok=True)
        print(f"Extracting ROM filesystem to: {output_dir}")
        
        # Start from root entry at offset 0
        self.extract_dirent(0, output_dir)
        
        print(f"\nExtraction complete!")


def extract_romfs(binary_file, output_dir, base_addr=None):
    """Extract ROM filesystem from binary file.
    
    Args:
        binary_file: Path to binary ROM filesystem file
        output_dir: Output directory for extracted files
        base_addr: Base address (optional, will auto-detect if not provided)
    """
    try:
        with open(binary_file, 'rb') as f:
            data = f.read()
        
        extractor = RomfsExtractor(data, base_addr)
        extractor.extract(output_dir)
        
    except IOError as e:
        print(f"Error reading binary file {binary_file}: {e}", file=sys.stderr)
        sys.exit(1)
    except Exception as e:
        print(f"Error extracting filesystem: {e}", file=sys.stderr)
        sys.exit(1)


if __name__ == '__main__':
    args = parser.parse_args()

    # Extract mode
    if args.extract:
        base_addr = int(args.addr, 16) if args.addr != '0' else None
        extract_romfs(args.rootdir, args.output_dir, base_addr)
        sys.exit(0)

    # Build mode
    os.chdir(args.rootdir)

    tree = Folder('romfs_root')
    tree.walk()
    tree.sort()

    if args.dump:
        tree.dump()

    base_addr = int(args.addr, 16)
    header_content = ''
    
    if args.binary:
        data, header_content = get_bin_data(tree, base_addr)
    else:
        data = get_c_data(tree)

    output = args.output
    if not output:
        output = sys.stdout.buffer if hasattr(sys.stdout, 'buffer') else sys.stdout

    output.write(data)
    
    if header_content:
        header_path = os.path.join(os.path.dirname(os.getcwd()), 'ui_resource.h')
        try:
            with open(header_path, 'w') as f:
                f.write(header_content)
        except IOError as e:
            print(f"Error writing header file {header_path}: {e}", file=sys.stderr)
            sys.exit(1)

