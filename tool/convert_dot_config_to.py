#
# Copyright (c) 2026, Realtek Semiconductor Corporation
#
# SPDX-License-Identifier: MIT
#

# parse_kconfig.py
import sys

def parse_config(config_file):
    config_options = {}
    with open(config_file, 'r') as file:
        for line in file:
            if line.startswith('#') or not line.strip():
                continue
            key, _, value = line.partition('=')
            config_options[key.strip()] = value.strip()
    return config_options

if __name__ == "__main__":
    if len(sys.argv) != 3:
        print("Usage: python parse_kconfig.py <config_file> <output_file>")
        sys.exit(1)
    
    config_file = sys.argv[1]
    output_file = sys.argv[2]
    
    config_options = parse_config(config_file)
    
    with open(output_file, 'w') as file:
        for key, value in config_options.items():
            file.write(f"set({key} {value})\n")
