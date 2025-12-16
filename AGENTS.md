# HoneyGUI Development Guidelines

## General Rules

**IMPORTANT**: 
- Do NOT create or modify Markdown (.md) documentation files unless explicitly requested by the user.
- Do NOT create README.md, QUICKSTART.md, or any other documentation files by default.
- Only add documentation when the user specifically asks for it.

## Git Commit Rules

### Commit Message Format

#### Title (First Line)
- **Prefix**: Must start with `GUI:`
- **Length**: Maximum 48 characters

#### Body (Subsequent Lines)
- **Line Length**: Maximum 60 characters per line

### Gerrit Submission

**IMPORTANT**: Only commit and push when explicitly requested.

```bash
git push origin HEAD:refs/for/master
```

## Coding Standards

### Logging
- Use `gui_log()` instead of `printf()`
- Include: `#include "gui_api_os.h"`

### File System Access
- Use VFS API instead of system calls
- Include: `#include "gui_vfs.h"`
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
