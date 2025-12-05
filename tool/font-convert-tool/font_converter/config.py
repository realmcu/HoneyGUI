"""
Configuration management for the font converter.

This module handles loading, merging, and validating configuration from
JSON files, INI files, and command-line arguments.
"""

import json
import os
from dataclasses import dataclass, field
from pathlib import Path
from typing import List, Tuple, Dict, Any, Optional
import configparser

from .constants import (
    DEFAULT_GAMMA,
    DEFAULT_ROTATION,
    DEFAULT_FONT_SIZE,
    DEFAULT_BACK_SIZE,
    DEFAULT_RENDER_MODE,
    DEFAULT_INDEX_METHOD,
    DEFAULT_OUTPUT_FORMAT,
    VALID_RENDER_MODES,
    VALID_ROTATIONS,
    INDEX_METHOD_ADDRESS,
    INDEX_METHOD_OFFSET,
)
from .errors import (
    FileNotFoundError,
    ParseError,
    ValidationError,
    ErrorCode,
)


@dataclass
class FontConfig:
    """
    Font configuration data class.
    
    This class holds all configuration parameters for font generation,
    including font file path, rendering options, and output settings.
    """
    
    # Required fields
    font: str
    font_size: int
    
    # Optional fields with defaults
    back_size: int = DEFAULT_BACK_SIZE
    render_mode: int = DEFAULT_RENDER_MODE
    rotation: int = DEFAULT_ROTATION
    bold: bool = False
    italic: bool = False
    crop: bool = False
    index_method: int = DEFAULT_INDEX_METHOD
    output_format: int = DEFAULT_OUTPUT_FORMAT
    gamma: float = DEFAULT_GAMMA
    rvd: bool = False
    output_folder: str = ""
    
    # Character set configuration
    codepages: List[str] = field(default_factory=list)
    mappings: List[str] = field(default_factory=list)
    symbols: List[str] = field(default_factory=list)
    custom_ranges: List[Tuple[int, int]] = field(default_factory=list)
    
    def __post_init__(self):
        """Validate and normalize configuration after initialization."""
        # Ensure lists are mutable copies
        if not isinstance(self.codepages, list):
            self.codepages = list(self.codepages) if self.codepages else []
        if not isinstance(self.mappings, list):
            self.mappings = list(self.mappings) if self.mappings else []
        if not isinstance(self.symbols, list):
            self.symbols = list(self.symbols) if self.symbols else []
        if not isinstance(self.custom_ranges, list):
            self.custom_ranges = list(self.custom_ranges) if self.custom_ranges else []


class ConfigManager:
    """
    Configuration manager for loading and merging configurations.
    
    This class provides static methods for loading configuration from
    JSON files, INI files, and merging them with command-line arguments.
    """
    
    @staticmethod
    def load_json_config(json_path: str) -> FontConfig:
        """
        Load configuration from a JSON file.
        
        Args:
            json_path: Path to the JSON configuration file
            
        Returns:
            FontConfig object with loaded configuration
            
        Raises:
            FileNotFoundError: If the JSON file does not exist
            ParseError: If the JSON file is invalid or missing required fields
        """
        # Check if file exists
        if not os.path.exists(json_path):
            raise FileNotFoundError("JSON", json_path)
        
        # Load and parse JSON
        try:
            with open(json_path, 'r', encoding='utf-8') as f:
                data = json.load(f)
        except json.JSONDecodeError as e:
            raise ParseError("JSON", json_path, f"Invalid JSON format: {str(e)}")
        except Exception as e:
            raise ParseError("JSON", json_path, f"Failed to read file: {str(e)}")
        
        # Check for required fields
        required_fields = ['font', 'fontSize', 'renderMode']
        missing_fields = [field for field in required_fields if field not in data]
        
        if missing_fields:
            raise ParseError(
                "JSON",
                json_path,
                f"Missing required fields: {', '.join(missing_fields)}"
            )
        
        # Extract configuration values
        try:
            config = FontConfig(
                font=data['font'],
                font_size=int(data['fontSize']),
                back_size=int(data.get('backSize', DEFAULT_BACK_SIZE)),
                render_mode=int(data['renderMode']),
                rotation=int(data.get('rotation', DEFAULT_ROTATION)),
                bold=bool(data.get('bold', False)),
                italic=bool(data.get('italic', False)),
                crop=bool(data.get('crop', False)),
                index_method=int(data.get('indexMethod', DEFAULT_INDEX_METHOD)),
                output_format=int(data.get('outputFormat', DEFAULT_OUTPUT_FORMAT)),
                gamma=float(data.get('gamma', DEFAULT_GAMMA)),
                rvd=bool(data.get('rvd', False)),
                output_folder=str(data.get('outputFolder', '')),
                codepages=data.get('codepages', []),
                mappings=data.get('mappings', []),
                symbols=data.get('symbols', []),
                custom_ranges=ConfigManager._parse_custom_ranges(data.get('customRanges', []))
            )
            return config
        except (ValueError, TypeError, KeyError) as e:
            raise ParseError("JSON", json_path, f"Invalid field value: {str(e)}")
    
    @staticmethod
    def _parse_custom_ranges(ranges_data: List) -> List[Tuple[int, int]]:
        """
        Parse custom Unicode ranges from JSON data.
        
        Args:
            ranges_data: List of range specifications (can be lists or dicts)
            
        Returns:
            List of (start, end) tuples
        """
        result = []
        for item in ranges_data:
            if isinstance(item, (list, tuple)) and len(item) == 2:
                result.append((int(item[0]), int(item[1])))
            elif isinstance(item, dict) and 'start' in item and 'end' in item:
                result.append((int(item['start']), int(item['end'])))
        return result
    
    @staticmethod
    def load_ini_config(ini_path: str = "setting.ini") -> Dict[str, Any]:
        """
        Load configuration from an INI file.
        
        Args:
            ini_path: Path to the INI configuration file (default: "setting.ini")
            
        Returns:
            Dictionary with configuration values (gamma, rotation)
            Returns default values if file doesn't exist or is invalid
        """
        config = {
            'gamma': DEFAULT_GAMMA,
            'rotation': DEFAULT_ROTATION
        }
        
        # If file doesn't exist, return defaults
        if not os.path.exists(ini_path):
            return config
        
        # Try to parse INI file
        try:
            parser = configparser.ConfigParser()
            parser.read(ini_path, encoding='utf-8')
            
            # Try to read gamma value
            if parser.has_option('DEFAULT', 'gamma'):
                try:
                    config['gamma'] = float(parser.get('DEFAULT', 'gamma'))
                except (ValueError, TypeError):
                    pass  # Keep default value
            
            # Try to read rotation value
            if parser.has_option('DEFAULT', 'rotate'):
                try:
                    config['rotation'] = int(parser.get('DEFAULT', 'rotate'))
                except (ValueError, TypeError):
                    pass  # Keep default value
            elif parser.has_option('DEFAULT', 'rotation'):
                try:
                    config['rotation'] = int(parser.get('DEFAULT', 'rotation'))
                except (ValueError, TypeError):
                    pass  # Keep default value
                    
        except Exception:
            # If any error occurs, just return defaults
            pass
        
        return config
    
    @staticmethod
    def merge_configs(
        json_config: FontConfig,
        ini_config: Dict[str, Any],
        cli_args: Optional[Any] = None
    ) -> FontConfig:
        """
        Merge configurations with priority: CLI args > INI config > JSON config.
        
        Args:
            json_config: Configuration loaded from JSON file
            ini_config: Configuration loaded from INI file
            cli_args: Command-line arguments (argparse.Namespace or dict)
            
        Returns:
            Merged FontConfig object
        """
        # Start with JSON config
        merged = FontConfig(
            font=json_config.font,
            font_size=json_config.font_size,
            back_size=json_config.back_size,
            render_mode=json_config.render_mode,
            rotation=json_config.rotation,
            bold=json_config.bold,
            italic=json_config.italic,
            crop=json_config.crop,
            index_method=json_config.index_method,
            output_format=json_config.output_format,
            gamma=json_config.gamma,
            rvd=json_config.rvd,
            output_folder=json_config.output_folder,
            codepages=json_config.codepages.copy(),
            mappings=json_config.mappings.copy(),
            symbols=json_config.symbols.copy(),
            custom_ranges=json_config.custom_ranges.copy()
        )
        
        # Apply INI config (overrides JSON)
        if ini_config:
            if 'gamma' in ini_config:
                merged.gamma = ini_config['gamma']
            if 'rotation' in ini_config:
                merged.rotation = ini_config['rotation']
        
        # Apply CLI args (overrides everything)
        if cli_args:
            # Convert to dict if it's an argparse.Namespace
            if hasattr(cli_args, '__dict__'):
                args_dict = vars(cli_args)
            else:
                args_dict = cli_args
            
            # Override with non-None CLI arguments
            if args_dict.get('font') is not None:
                merged.font = args_dict['font']
            if args_dict.get('size') is not None:
                merged.font_size = args_dict['size']
            if args_dict.get('render_mode') is not None:
                merged.render_mode = args_dict['render_mode']
            if args_dict.get('output') is not None:
                merged.output_folder = args_dict['output']
            if args_dict.get('codepage') is not None:
                merged.codepages = args_dict['codepage']
            if args_dict.get('rotation') is not None:
                merged.rotation = args_dict['rotation']
            if args_dict.get('gamma') is not None:
                merged.gamma = args_dict['gamma']
            # For boolean flags (store_true), only override if explicitly set to True
            if args_dict.get('bold'):
                merged.bold = True
            if args_dict.get('italic'):
                merged.italic = True
            if args_dict.get('crop'):
                merged.crop = True
            if args_dict.get('rvd'):
                merged.rvd = True
        
        return merged
    
    @staticmethod
    def validate_config(config: FontConfig) -> None:
        """
        Validate configuration parameters.
        
        Args:
            config: FontConfig object to validate
            
        Raises:
            ValidationError: If any configuration parameter is invalid
        """
        errors = []
        
        # Validate font size
        if config.font_size <= 0:
            errors.append(f"Invalid font size: {config.font_size}. Must be greater than 0.")
        
        # Validate render mode
        if config.render_mode not in VALID_RENDER_MODES:
            errors.append(
                f"Invalid render mode: {config.render_mode}. "
                f"Must be one of {sorted(VALID_RENDER_MODES)}."
            )
        
        # Validate rotation
        if config.rotation not in VALID_ROTATIONS:
            errors.append(
                f"Invalid rotation: {config.rotation}. "
                f"Must be one of {sorted(VALID_ROTATIONS)}."
            )
        
        # Check for index method and crop conflict
        if config.index_method == INDEX_METHOD_OFFSET and config.crop:
            errors.append(
                "Configuration conflict: indexMethod=1 (offset mode) "
                "cannot be used with crop=true."
            )
        
        # Validate font file path
        if not config.font:
            errors.append("Font file path is empty.")
        elif not os.path.exists(config.font):
            errors.append(f"Font file not found: {config.font}")
        
        # If there are any errors, raise ValidationError
        if errors:
            raise ValidationError(
                "Configuration validation failed",
                "\n".join(f"  - {error}" for error in errors)
            )
