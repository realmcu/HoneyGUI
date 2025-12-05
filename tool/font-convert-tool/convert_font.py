#!/usr/bin/env python3
"""
Font Converter - Simple launcher script

Usage:
    python convert_font.py <config.json>
    python convert_font.py --help
"""

import sys
from pathlib import Path

# Add font_converter to path
sys.path.insert(0, str(Path(__file__).parent))

from font_converter.__main__ import main

if __name__ == '__main__':
    main()
