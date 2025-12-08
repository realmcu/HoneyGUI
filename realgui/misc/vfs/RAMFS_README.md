# RAMFS & VFS Copy - Quick Reference

## RAMFS: RAM Filesystem with XIP

### Mount & Basic Usage
```c
void *ram = gui_malloc(1024 * 1024);
gui_vfs_mount_ramfs("/ram", ram, 1024 * 1024);

/* Write */
gui_vfs_file_t *f = gui_vfs_open("/ram/data.bin", GUI_VFS_WRITE | GUI_VFS_CREATE);
gui_vfs_write(f, data, size);
gui_vfs_close(f);

/* XIP (zero-copy) */
const void *addr = gui_vfs_get_file_address("/ram/data.bin");
process_data(addr);
```

### Configuration
```c
#define RAMFS_MAX_FILES 32   // Max files
#define RAMFS_MAX_PATH 128   // Max path length
```

## VFS Copy Functions

### Copy Files Between Filesystems
```c
/* Single file */
gui_vfs_copy_file("/sd/photo.jpg", "/ram/photo.jpg");

/* Directory */
gui_vfs_copy_dir("/sd/fonts", "/ram/fonts");
```

### Example: Cache SD Resources to RAM
```c
/* Cache for fast XIP access */
gui_vfs_copy_file("/sd/logo.png", "/ram/logo.png");
const void *logo = gui_vfs_get_file_address("/ram/logo.png");
display_image(logo);  // Zero-copy
```

## Features
- [x] RAMFS: Read/Write + XIP
- [x] Copy: Auto XIP optimization
- [x] Cross-filesystem support
