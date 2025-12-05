# HoneyGUI Development Guidelines

## Git Commit Rules

### Commit Message Format

#### Title (First Line)
- **Prefix**: Must start with `GUI:`
- **Length**: Maximum 48 characters
- **Style**: Brief and descriptive

Example:
```
GUI: Add VFS shell commands for HoneyGUI
```

#### Body (Subsequent Lines)
- **Line Length**: Maximum 60 characters per line
- **Content**: Detailed description of changes
- **Format**: 
  - Blank line after title
  - Bullet points for multiple items
  - Clear and concise explanations

Example:
```
GUI: Add VFS shell commands for HoneyGUI

Add honeygui_cmd.c with VFS-based shell commands:
- vls: List VFS directory contents with details
- stat: Display VFS file information
- version: Show HoneyGUI platform info

All commands use gui_log for output and VFS API
for filesystem access, avoiding direct system
calls for better portability.

Commands are automatically registered via
SHELL_EXPORT_CMD macro.
```

### Gerrit Submission

Push commits to Gerrit for review:
```bash
git push origin HEAD:refs/for/master
```

## Coding Standards

### Logging
- Use `gui_log()` instead of `printf()`
- Include in header: `#include "gui_api_os.h"`

### File System Access
- Use VFS API instead of system calls
- Include in header: `#include "gui_vfs.h"`
- VFS functions:
  - `gui_vfs_opendir()` - Open directory
  - `gui_vfs_readdir()` - Read directory entry
  - `gui_vfs_closedir()` - Close directory
  - `gui_vfs_stat()` - Get file info
  - `gui_vfs_open()` - Open file
  - `gui_vfs_read()` - Read file
  - `gui_vfs_close()` - Close file

### Shell Commands
- Location: `realgui/3rd/letter_shell/`
- Export macro: `SHELL_EXPORT_CMD()`
- Format:
```c
SHELL_EXPORT_CMD(
    SHELL_CMD_PERMISSION(0) | SHELL_CMD_TYPE(SHELL_TYPE_CMD_MAIN),
    command_name, function_name, description);
```

## Platform Support

### Conditional Compilation
```c
#if defined(_WIN32) || defined(__linux__)
// Platform-specific code
#endif
```

### Linker Scripts
- Windows: `win32_sim/honeygui_mingw.ld`
- Linux: `win32_sim/honeygui_linux.lds`
- Section names: Use without dot prefix (e.g., `shellCommand` not `.shellCommand`)
