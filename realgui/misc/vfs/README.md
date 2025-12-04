# HoneyGUI Virtual File System (VFS)

## Overview

HoneyGUI VFS provides a unified file access interface supporting multiple backends:
- **ROMFS** - Internal resources with XIP (eXecute In Place) zero-copy support
- **POSIX** - Standard filesystem for PC simulator
- **Generic Adapter** - Support for FATFS, LittleFS, SPIFFS, and custom filesystems

## How to Mount a Generic Filesystem

To mount a generic filesystem like FATFS, you need to create an adapter. Here's the complete process:

### Step 1: Define Adapter Functions

Implement at least these 3 required functions:

```c
#include "ff.h"  // FATFS header
#include <stdlib.h>

// Open file
static void* my_fatfs_open(const char *path, int flags) {
    FIL *fp = malloc(sizeof(FIL));
    if (!fp) return NULL;
    
    BYTE mode = 0;
    if (flags & 0x01) mode |= FA_READ;
    if (flags & 0x02) mode |= FA_WRITE;
    
    if (f_open(fp, path, mode) != FR_OK) {
        free(fp);
        return NULL;
    }
    return fp;
}

// Close file
static int my_fatfs_close(void *file) {
    FIL *fp = (FIL *)file;
    f_close(fp);
    free(fp);
    return 0;
}

// Read file
static int my_fatfs_read(void *file, void *buf, size_t size) {
    FIL *fp = (FIL *)file;
    UINT br;
    if (f_read(fp, buf, size, &br) != FR_OK) return -1;
    return br;
}
```

### Step 2: Create Adapter Structure

```c
static const gui_fs_adapter_t my_fatfs_adapter = {
    .fs_open = my_fatfs_open,
    .fs_close = my_fatfs_close,
    .fs_read = my_fatfs_read,
    .fs_write = NULL,     // Optional
    .fs_seek = NULL,      // Optional
    .fs_tell = NULL,      // Optional
    .fs_opendir = NULL,   // Optional
    .fs_readdir = NULL,   // Optional
    .fs_closedir = NULL,  // Optional
    .fs_stat = NULL,      // Optional
};
```

### Step 3: Mount to VFS

```c
void app_init(void) {
    gui_vfs_init();
    
    // Mount ROMFS
    gui_vfs_mount_romfs("/rom", resource_root, 0x704D1400);
    
    // Mount FATFS using your adapter
    gui_vfs_mount_generic("/sd", "0:", &my_fatfs_adapter);
}
```

### Step 4: Use VFS

```c
void load_file(void) {
    size_t size;
    void *data = gui_vfs_load_file("/sd/photo.jpg", &size);
    if (data) {
        // Use data
        free(data);
    }
}
```

That's it! The last parameter is just `&your_adapter_variable`.

## Quick Start

### PC Simulator

```c
#include "gui_vfs.h"

void app_init(void) {
    gui_vfs_init();
    
    /* Mount internal ROMFS */
    extern unsigned char *resource_root;
    gui_vfs_mount_romfs("/rom", resource_root, 0);
    
    /* Mount PC filesystem - relative path */
    gui_vfs_mount_posix("/sd", "./sdcard");
    
    /* Mount PC filesystem - absolute path */
    gui_vfs_mount_posix("/data", "C:/Users/Public/Documents");
    
    /* Mount PC filesystem - current directory */
    gui_vfs_mount_posix("/", ".");
}

/* Usage */
void load_image(void) {
    size_t size;
    
    /* Load from ROMFS */
    void *logo = gui_vfs_load_file("/rom/logo.png", &size);
    
    /* Load from relative path */
    void *photo = gui_vfs_load_file("/sd/photo.jpg", &size);
    
    /* Load from absolute path */
    void *doc = gui_vfs_load_file("/data/config.txt", &size);
    
    /* Load from current directory */
    void *local = gui_vfs_load_file("/test.png", &size);
    
    if (logo) free(logo);
    if (photo) free(photo);
    if (doc) free(doc);
    if (local) free(local);
}
```

### Embedded System

```c
void app_init(void) {
    gui_vfs_init();
    
    /* Mount internal ROMFS with XIP */
    extern unsigned char resource_root[];
    gui_vfs_mount_romfs("/rom", resource_root, 0x704D1400);
    
    /* Mount FATFS using generic adapter */
    gui_vfs_mount_generic("/sd", "0:", &fatfs_adapter);
    
    /* Mount LittleFS using generic adapter */
    gui_vfs_mount_generic("/flash", "", &littlefs_adapter);
}

/* XIP zero-copy access */
void load_font(void) {
    gui_vfs_file_t *file = gui_vfs_open("/rom/font.ttf", GUI_VFS_READ);
    size_t size;
    const void *addr = gui_vfs_get_addr(file, &size);
    if (addr) {
        /* Direct access, no copy needed */
        use_font(addr, size);
    }
    gui_vfs_close(file);
}
```

## API Reference

### Initialization and Mounting

```c
/* Initialize VFS system */
void gui_vfs_init(void);

/* Mount ROMFS */
int gui_vfs_mount_romfs(const char *prefix, const void *base_addr, uint32_t base_offset);

/* Mount POSIX filesystem */
int gui_vfs_mount_posix(const char *prefix, const char *root_path);

/* Mount generic filesystem */
int gui_vfs_mount_generic(const char *prefix, const char *root_path, 
                          const gui_fs_adapter_t *adapter);

/* Unmount filesystem */
int gui_vfs_unmount(const char *prefix);
```

### File Operations

```c
/* Open file */
gui_vfs_file_t* gui_vfs_open(const char *path, gui_vfs_mode_t mode);

/* Close file */
int gui_vfs_close(gui_vfs_file_t *file);

/* Read from file */
int gui_vfs_read(gui_vfs_file_t *file, void *buf, size_t size);

/* Write to file */
int gui_vfs_write(gui_vfs_file_t *file, const void *buf, size_t size);

/* Seek in file */
int gui_vfs_seek(gui_vfs_file_t *file, int offset, gui_vfs_seek_t whence);

/* Get current position */
int gui_vfs_tell(gui_vfs_file_t *file);

/* Get direct address (XIP support, ROMFS only) */
const void* gui_vfs_get_addr(gui_vfs_file_t *file, size_t *size);
```

### Directory Operations

```c
/* Open directory */
gui_vfs_dir_t* gui_vfs_opendir(const char *path);

/* Read directory entry */
int gui_vfs_readdir(gui_vfs_dir_t *dir, gui_vfs_stat_t *stat);

/* Close directory */
int gui_vfs_closedir(gui_vfs_dir_t *dir);

/* Get file info */
int gui_vfs_stat(const char *path, gui_vfs_stat_t *stat);
```

### Helper Functions

```c
/* Load entire file into memory */
void* gui_vfs_load_file(const char *path, size_t *size);

/* Get mount point prefix for a path */
const char* gui_vfs_get_mount_prefix(const char *path);
```

### File Access Modes

```c
typedef enum {
    GUI_VFS_READ    = 0x01,
    GUI_VFS_WRITE   = 0x02,
    GUI_VFS_APPEND  = 0x04,
    GUI_VFS_CREATE  = 0x08,
} gui_vfs_mode_t;
```

### Seek Origins

```c
typedef enum {
    GUI_VFS_SEEK_SET = 0,
    GUI_VFS_SEEK_CUR = 1,
    GUI_VFS_SEEK_END = 2,
} gui_vfs_seek_t;
```

## Generic Adapter Interface

### Adapter Structure

```c
typedef struct {
    /* Required: File operations */
    void* (*fs_open)(const char *path, int flags);
    int (*fs_close)(void *file);
    int (*fs_read)(void *file, void *buf, size_t size);
    
    /* Optional: Write and seek */
    int (*fs_write)(void *file, const void *buf, size_t size);
    int (*fs_seek)(void *file, int offset, int whence);
    int (*fs_tell)(void *file);
    
    /* Optional: Directory operations */
    void* (*fs_opendir)(const char *path);
    int (*fs_readdir)(void *dir, char *name, size_t name_size, 
                      int *is_dir, size_t *size);
    int (*fs_closedir)(void *dir);
    
    /* Optional: File info */
    int (*fs_stat)(const char *path, int *is_dir, size_t *size);
} gui_fs_adapter_t;
```

### Adapter Flags

```c
#define FS_FLAG_READ    0x01  /* Read */
#define FS_FLAG_WRITE   0x02  /* Write */
#define FS_FLAG_CREATE  0x04  /* Create */
#define FS_FLAG_APPEND  0x08  /* Append */
```

### FATFS Adapter Example

```c
#include "ff.h"

static void* fatfs_open(const char *path, int flags) {
    FIL *fp = malloc(sizeof(FIL));
    BYTE mode = 0;
    if (flags & 0x01) mode |= FA_READ;
    if (flags & 0x02) mode |= FA_WRITE;
    if (f_open(fp, path, mode) != FR_OK) {
        free(fp);
        return NULL;
    }
    return fp;
}

static int fatfs_close(void *file) {
    FIL *fp = (FIL *)file;
    f_close(fp);
    free(fp);
    return 0;
}

static int fatfs_read(void *file, void *buf, size_t size) {
    FIL *fp = (FIL *)file;
    UINT br;
    if (f_read(fp, buf, size, &br) != FR_OK) return -1;
    return br;
}

static const gui_fs_adapter_t fatfs_adapter = {
    .fs_open = fatfs_open,
    .fs_close = fatfs_close,
    .fs_read = fatfs_read,
    /* ... other operations */
};

/* Mount */
gui_vfs_mount_generic("/sd", "0:", &fatfs_adapter);
```

### LittleFS Adapter Example

```c
#include "lfs.h"

static lfs_t g_lfs;

static void* littlefs_open(const char *path, int flags) {
    lfs_file_t *file = malloc(sizeof(lfs_file_t));
    int lfs_flags = 0;
    if (flags & 0x01) lfs_flags |= LFS_O_RDONLY;
    if (flags & 0x02) lfs_flags |= LFS_O_WRONLY;
    if (lfs_file_open(&g_lfs, file, path, lfs_flags) < 0) {
        free(file);
        return NULL;
    }
    return file;
}

static const gui_fs_adapter_t littlefs_adapter = {
    .fs_open = littlefs_open,
    /* ... other operations */
};

/* Mount */
gui_vfs_mount_generic("/flash", "", &littlefs_adapter);
```

## Usage Examples

### Multiple Filesystems

```c
/* Mount multiple filesystems */
gui_vfs_init();
gui_vfs_mount_romfs("/rom", resource_root, 0x704D1400);
gui_vfs_mount_generic("/sd", "0:", &fatfs_adapter);
gui_vfs_mount_generic("/flash", "", &littlefs_adapter);

/* Unified access */
void *font = gui_vfs_load_file("/rom/font.ttf", &size);
void *photo = gui_vfs_load_file("/sd/photo.jpg", &size);
void *config = gui_vfs_load_file("/flash/config.json", &size);
```

### Platform-Independent Code

```c
void app_init(void) {
    gui_vfs_init();
    
#ifdef _HONEYGUI_SIMULATOR_
    /* PC simulator */
    gui_vfs_mount_posix("/rom", "./resources");
    gui_vfs_mount_posix("/sd", "./sdcard");
#else
    /* Embedded system */
    gui_vfs_mount_romfs("/rom", resource_root, 0x704D1400);
    gui_vfs_mount_generic("/sd", "0:", &fatfs_adapter);
#endif
    
    /* Application code remains the same */
    load_resources();
}
```

### Check Filesystem Type

```c
bool is_romfs_file(const char *path) {
    const char *prefix = gui_vfs_get_mount_prefix(path);
    return (prefix && strcmp(prefix, "/rom") == 0);
}

void load_resource(const char *path) {
    if (is_romfs_file(path)) {
        /* Use XIP zero-copy */
        gui_vfs_file_t *file = gui_vfs_open(path, GUI_VFS_READ);
        size_t size;
        const void *addr = gui_vfs_get_addr(file, &size);
        if (addr) {
            use_data_directly(addr, size);
        }
        gui_vfs_close(file);
    } else {
        /* Load into memory */
        size_t size;
        void *data = gui_vfs_load_file(path, &size);
        if (data) {
            use_data(data, size);
            free(data);
        }
    }
}
```

### Smart Loading

```c
void* smart_load_file(const char *path, size_t *size, bool *need_free) {
    gui_vfs_file_t *file = gui_vfs_open(path, GUI_VFS_READ);
    if (!file) return NULL;
    
    /* Try XIP first */
    const void *addr = gui_vfs_get_addr(file, size);
    if (addr) {
        /* ROMFS - zero-copy */
        *need_free = false;
        gui_vfs_close(file);
        return (void *)addr;
    }
    
    /* Other FS - need to load */
    gui_vfs_close(file);
    void *data = gui_vfs_load_file(path, size);
    *need_free = true;
    return data;
}

/* Usage */
size_t size;
bool need_free;
void *data = smart_load_file("/rom/image.png", &size, &need_free);
if (data) {
    use_image(data, size);
    if (need_free) {
        free(data);
    }
}
```

## Notes

- Path format: Unix-style `/rom/image.png`
- XIP support: Only available for ROMFS via `gui_vfs_get_addr()`
- Memory management: `gui_vfs_load_file()` returns allocated memory that must be freed
- Mount order: Earlier mounts have higher priority for path matching
