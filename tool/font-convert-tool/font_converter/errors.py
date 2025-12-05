"""
Error codes and exception classes for the font converter.
"""

from enum import Enum
from typing import Optional


class ErrorCode(Enum):
    """Error codes for font converter operations."""
    
    SUCCESS = 0
    ARGUMENT_ERROR = 1
    JSON_FILE_NOT_EXIST = 2
    JSON_PARSE_FAILED = 3
    FONT_NOT_CONFIGURED = 4
    FONT_FILE_NOT_FOUND = 5
    CODEPAGE_FILE_NOT_FOUND = 6
    CHARACTER_NOT_IN_FONT = 7
    INDEX_METHOD_CONFLICT = 8
    INVALID_CONFIG = 9
    FILE_WRITE_ERROR = 10
    INVALID_FONT_SIZE = 11
    INVALID_RENDER_MODE = 12
    INVALID_ROTATION = 13
    FREETYPE_INIT_ERROR = 14
    FONT_LOAD_ERROR = 15
    CODEPAGE_PARSE_ERROR = 16
    OUTPUT_DIR_ERROR = 17


class FontConverterError(Exception):
    """Base exception class for font converter errors."""
    
    def __init__(self, error_code: ErrorCode, message: str, details: Optional[str] = None):
        """
        Initialize a font converter error.
        
        Args:
            error_code: The error code enum value
            message: Human-readable error message
            details: Optional additional details about the error
        """
        self.error_code = error_code
        self.message = message
        self.details = details
        
        full_message = f"[{error_code.name}] {message}"
        if details:
            full_message += f"\nDetails: {details}"
        
        super().__init__(full_message)
    
    def __str__(self) -> str:
        """Return string representation of the error."""
        result = f"Error {self.error_code.value}: {self.message}"
        if self.details:
            result += f"\n{self.details}"
        return result


class ConfigurationError(FontConverterError):
    """Exception raised for configuration-related errors."""
    
    def __init__(self, message: str, details: Optional[str] = None):
        super().__init__(ErrorCode.INVALID_CONFIG, message, details)


class FileNotFoundError(FontConverterError):
    """Exception raised when a required file is not found."""
    
    def __init__(self, file_type: str, file_path: str):
        message = f"{file_type} file not found: {file_path}"
        
        if file_type.lower() == "json":
            error_code = ErrorCode.JSON_FILE_NOT_EXIST
        elif file_type.lower() == "font":
            error_code = ErrorCode.FONT_FILE_NOT_FOUND
        elif file_type.lower() == "codepage":
            error_code = ErrorCode.CODEPAGE_FILE_NOT_FOUND
        else:
            error_code = ErrorCode.ARGUMENT_ERROR
        
        super().__init__(error_code, message)


class ParseError(FontConverterError):
    """Exception raised for parsing errors."""
    
    def __init__(self, file_type: str, file_path: str, details: Optional[str] = None):
        message = f"Failed to parse {file_type} file: {file_path}"
        
        if file_type.lower() == "json":
            error_code = ErrorCode.JSON_PARSE_FAILED
        elif file_type.lower() == "codepage":
            error_code = ErrorCode.CODEPAGE_PARSE_ERROR
        else:
            error_code = ErrorCode.ARGUMENT_ERROR
        
        super().__init__(error_code, message, details)


class ValidationError(FontConverterError):
    """Exception raised for validation errors."""
    
    def __init__(self, message: str, details: Optional[str] = None):
        super().__init__(ErrorCode.INVALID_CONFIG, message, details)


class FontRenderError(FontConverterError):
    """Exception raised for font rendering errors."""
    
    def __init__(self, message: str, details: Optional[str] = None):
        super().__init__(ErrorCode.FONT_LOAD_ERROR, message, details)
