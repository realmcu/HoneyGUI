"""
Main entry point for the font converter package.

This allows the package to be run as a module:
    python -m font_converter [args]
"""

from .cli import main

if __name__ == '__main__':
    main()
