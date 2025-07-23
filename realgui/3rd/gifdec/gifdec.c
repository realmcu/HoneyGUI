#include "gifdec.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#ifdef ENABLE_HONEYGUI
#include "gui_api.h"
#endif



#define MIN(A, B) ((A) < (B) ? (A) : (B))
#define MAX(A, B) ((A) > (B) ? (A) : (B))

static size_t mem_read(gd_GIF *gif, void *buf, size_t size)
{
    gd_MemorySource *mem = &gif->source.mem;
    if (mem->pos + size > mem->size)
    {
        size = mem->size - mem->pos;
    }

    if (size > 0)
    {
        memcpy(buf, mem->data + mem->pos, size);
        mem->pos += size;
    }

    return size;
}

static long mem_lseek(gd_GIF *gif, long offset, int whence)
{
    gd_MemorySource *mem = &gif->source.mem;
    size_t new_pos = mem->pos;

    switch (whence)
    {
    case SEEK_SET:
        new_pos = offset;
        break;
    case SEEK_CUR:
        new_pos = mem->pos + offset;
        break;
    case SEEK_END:
        new_pos = mem->size + offset;
        break;
    }

    if (new_pos > mem->size)
    {
        new_pos = mem->size;
    }

    mem->pos = new_pos;
    return new_pos;
}

#ifdef _WIN32
#include <fcntl.h>
#include <io.h>
#include <sys/types.h>
#include <sys/stat.h>
#endif

static size_t port_read(int fd, void *buf, size_t len)
{
#ifdef ENABLE_HONEYGUI
    return gui_fs_read(fd, buf, len);
#else
    return read(fd, buf, len);
#endif
}

static int port_lseek(int fd, int offset, int whence)
{
#ifdef ENABLE_HONEYGUI
    return gui_fs_lseek(fd, offset, whence);
#else
    return lseek(fd, offset, whence);
#endif
}
static int port_open(const char *file, int flags)
{
#ifdef ENABLE_HONEYGUI
    return gui_fs_open(file, flags);
#else
    return open(file, flags);
#endif
}

static int port_close(int fd)
{
#ifdef ENABLE_HONEYGUI
    return gui_fs_close(fd);
#else
    return close(fd);
#endif
}

static void *port_malloc(uint32_t n)
{
#ifdef ENABLE_HONEYGUI
    return gui_malloc(n);
#else
    return malloc(n);
#endif
}

static void *port_realloc(void *ptr, uint32_t n)
{
#ifdef ENABLE_HONEYGUI
    return gui_realloc(ptr, n);
#else
    return realloc(ptr, n);
#endif
}


static void port_free(void *rmem)
{
#ifdef ENABLE_HONEYGUI
    gui_free(rmem);
#else
    free(rmem);
#endif
}





typedef struct Entry
{
    uint16_t length;
    uint16_t prefix;
    uint8_t  suffix;
} Entry;

typedef struct Table
{
    int bulk;
    int nentries;
    Entry *entries;
} Table;

static uint16_t read_num(gd_GIF *gif)
{
    uint8_t bytes[2];

    if (gif->is_mem)
    {
        mem_read(gif, bytes, 2);
    }
    else
    {
        port_read(gif->source.fd, bytes, 2);
    }
    return bytes[0] + (((uint16_t) bytes[1]) << 8);
}


static gd_GIF *gd_open_gif_common(gd_GIF *gif, \
                                  uint8_t sigver[3], \
                                  uint16_t width, \
                                  uint16_t height, \
                                  uint8_t fdsz, \
                                  uint8_t bgidx, \
                                  uint8_t aspect, \
                                  int gct_sz, \
                                  uint8_t depth)
{
    int i;
    uint8_t *bgcolor;

    /* Create gd_GIF Structure. */
    gif->width  = width;
    gif->height = height;
    gif->depth  = depth;

    /* Read GCT */
    gif->gct.size = gct_sz;
    if (gif->is_mem)
    {
        mem_read(gif, gif->gct.colors, 3 * gif->gct.size);
    }
    else
    {
        port_read(gif->source.fd, gif->gct.colors, 3 * gif->gct.size);
    }

    gif->palette = &gif->gct;
    gif->bgindex = bgidx;
    gif->frame = port_malloc(4 * width * height);
    memset(gif->frame, 0x00, 4 * width * height);
    if (!gif->frame)
    {
        port_free(gif);
        return NULL;
    }

    gif->canvas = &gif->frame[width * height];
    if (gif->bgindex)
    {
        memset(gif->frame, gif->bgindex, gif->width * gif->height);
    }

    bgcolor = &gif->palette->colors[gif->bgindex * 3];
    if (bgcolor[0] || bgcolor[1] || bgcolor[2])
    {
        for (i = 0; i < gif->width * gif->height; i++)
        {
            memcpy(&gif->canvas[i * 3], bgcolor, 3);
        }
    }

    if (gif->is_mem)
    {
        gif->anim_start = mem_lseek(gif, 0, SEEK_CUR);
    }
    else
    {
        gif->anim_start = port_lseek(gif->source.fd, 0, SEEK_CUR);
    }

    return gif;
}

gd_GIF *gd_open_gif(const char *fname)
{
    int fd;
    uint8_t sigver[3];
    uint16_t width, height, depth;
    uint8_t fdsz, bgidx, aspect;
    int gct_sz;
    gd_GIF *gif;

    fd = port_open(fname, 0);
    if (fd == -1) { return NULL; }
#ifdef _WIN32
    setmode(fd, O_BINARY);
#endif

    gif = port_malloc(1 * sizeof(*gif));
    if (!gif)
    {
        port_close(fd);
        return NULL;
    }
    memset(gif, 0x00, 1 * sizeof(*gif));

    gif->source.fd = fd;
    gif->is_mem = 0;

    /* Header */
    port_read(fd, sigver, 3);
    if (memcmp(sigver, "GIF", 3) != 0)
    {
        fprintf(stderr, "invalid signature\n");
        goto fail;
    }
    /* Version */
    port_read(fd, sigver, 3);
    if (memcmp(sigver, "89a", 3) != 0)
    {
        fprintf(stderr, "invalid version\n");
        goto fail;
    }
    /* Width x Height */
    width  = read_num(gif);
    height = read_num(gif);
    /* FDSZ */
    port_read(fd, &fdsz, 1);
    /* Presence of GCT */
    if (!(fdsz & 0x80))
    {
        fprintf(stderr, "no global color table\n");
        goto fail;
    }
    /* Color Space's Depth */
    depth = ((fdsz >> 4) & 7) + 1;
    /* Ignore Sort Flag. */
    /* GCT Size */
    gct_sz = 1 << ((fdsz & 0x07) + 1);
    /* Background Color Index */
    port_read(fd, &bgidx, 1);
    /* Aspect Ratio */
    port_read(fd, &aspect, 1);

    if (!gd_open_gif_common(gif, sigver, width, height, fdsz, bgidx, aspect, gct_sz, depth))
    {
        goto fail;
    }

    return gif;

fail:
    port_close(fd);
    port_free(gif);
    return NULL;
}

gd_GIF *gd_open_gif_from_memory(uint8_t *data, size_t size)
{
    uint8_t sigver[3];
    uint16_t width, height, depth;
    uint8_t fdsz, bgidx, aspect;
    int gct_sz;
    gd_GIF *gif;

    if (!data || size < 13)
    {
        return NULL;
    }

    /* Create gd_GIF Structure. */
    gif = port_malloc(1 * sizeof(*gif));

    if (!gif)
    {
        return NULL;
    }
    memset(gif, 0x00, 1 * sizeof(*gif));

    gif->source.mem.data = data;
    gif->source.mem.size = size;
    gif->source.mem.pos = 0;
    gif->is_mem = 1;

    /* Header */
    mem_read(gif, sigver, 3);
    if (memcmp(sigver, "GIF", 3) != 0)
    {
        fprintf(stderr, "invalid signature\n");
        goto fail;
    }
    /* Version */
    mem_read(gif, sigver, 3);
    if (memcmp(sigver, "89a", 3) != 0)
    {
        fprintf(stderr, "invalid version\n");
        goto fail;
    }
    /* Width x Height */
    width  = read_num(gif);
    height = read_num(gif);
    /* FDSZ */
    mem_read(gif, &fdsz, 1);
    /* Presence of GCT */
    if (!(fdsz & 0x80))
    {
        fprintf(stderr, "no global color table\n");
        goto fail;
    }
    /* Color Space's Depth */
    depth = ((fdsz >> 4) & 7) + 1;
    /* Ignore Sort Flag. */
    /* GCT Size */
    gct_sz = 1 << ((fdsz & 0x07) + 1);
    /* Background Color Index */
    mem_read(gif, &bgidx, 1);
    /* Aspect Ratio */
    mem_read(gif, &aspect, 1);

    if (!gd_open_gif_common(gif, sigver, width, height, fdsz, bgidx, aspect, gct_sz, depth))
    {
        goto fail;
    }

    return gif;

fail:
    port_free(gif);
    return NULL;
}

static void discard_sub_blocks(gd_GIF *gif)
{
    uint8_t size;

    do
    {
        if (gif->is_mem)
        {
            mem_read(gif, &size, 1);
            mem_lseek(gif, size, SEEK_CUR);
        }
        else
        {
            port_read(gif->source.fd, &size, 1);
            port_lseek(gif->source.fd, size, SEEK_CUR);
        }
    }
    while (size);
}

static void read_plain_text_ext(gd_GIF *gif)
{
    if (gif->plain_text)
    {
        uint16_t tx, ty, tw, th;
        uint8_t cw, ch, fg, bg;
        long sub_block;

        if (gif->is_mem)
        {
            mem_lseek(gif, 1, SEEK_CUR); /* block size = 12 */
            tx = read_num(gif);
            ty = read_num(gif);
            tw = read_num(gif);
            th = read_num(gif);
            mem_read(gif, &cw, 1);
            mem_read(gif, &ch, 1);
            mem_read(gif, &fg, 1);
            mem_read(gif, &bg, 1);
            sub_block = mem_lseek(gif, 0, SEEK_CUR);
            gif->plain_text(gif, tx, ty, tw, th, cw, ch, fg, bg);
            mem_lseek(gif, sub_block, SEEK_SET);
        }
        else
        {
            port_lseek(gif->source.fd, 1, SEEK_CUR); /* block size = 12 */
            tx = read_num(gif);
            ty = read_num(gif);
            tw = read_num(gif);
            th = read_num(gif);
            port_read(gif->source.fd, &cw, 1);
            port_read(gif->source.fd, &ch, 1);
            port_read(gif->source.fd, &fg, 1);
            port_read(gif->source.fd, &bg, 1);
            sub_block = port_lseek(gif->source.fd, 0, SEEK_CUR);
            gif->plain_text(gif, tx, ty, tw, th, cw, ch, fg, bg);
            port_lseek(gif->source.fd, sub_block, SEEK_SET);
        }
    }
    else
    {
        /* Discard plain text metadata. */
        if (gif->is_mem)
        {
            mem_lseek(gif, 13, SEEK_CUR);
        }
        else
        {
            port_lseek(gif->source.fd, 13, SEEK_CUR);
        }
    }
    /* Discard plain text sub-blocks. */
    discard_sub_blocks(gif);
}

static void read_graphic_control_ext(gd_GIF *gif)
{
    uint8_t rdit;

    /* Discard block size (always 0x04). */
    if (gif->is_mem)
    {
        mem_lseek(gif, 1, SEEK_CUR);
        mem_read(gif, &rdit, 1);
        gif->gce.disposal = (rdit >> 2) & 3;
        gif->gce.input = rdit & 2;
        gif->gce.transparency = rdit & 1;
        gif->gce.delay = read_num(gif);
        mem_read(gif, &gif->gce.tindex, 1);
        /* Skip block terminator. */
        mem_lseek(gif, 1, SEEK_CUR);
    }
    else
    {
        port_lseek(gif->source.fd, 1, SEEK_CUR);
        port_read(gif->source.fd, &rdit, 1);
        gif->gce.disposal = (rdit >> 2) & 3;
        gif->gce.input = rdit & 2;
        gif->gce.transparency = rdit & 1;
        gif->gce.delay = read_num(gif);
        port_read(gif->source.fd, &gif->gce.tindex, 1);
        /* Skip block terminator. */
        port_lseek(gif->source.fd, 1, SEEK_CUR);
    }
}

static void read_comment_ext(gd_GIF *gif)
{
    if (gif->comment)
    {
        long sub_block;
        if (gif->is_mem)
        {
            sub_block = mem_lseek(gif, 0, SEEK_CUR);
            gif->comment(gif);
            mem_lseek(gif, sub_block, SEEK_SET);
        }
        else
        {
            sub_block = port_lseek(gif->source.fd, 0, SEEK_CUR);
            gif->comment(gif);
            port_lseek(gif->source.fd, sub_block, SEEK_SET);
        }
    }
    /* Discard comment sub-blocks. */
    discard_sub_blocks(gif);
}

static void read_application_ext(gd_GIF *gif)
{
    char app_id[8];
    char app_auth_code[3];

    if (gif->is_mem)
    {
        /* Discard block size (always 0x0B). */
        mem_lseek(gif, 1, SEEK_CUR);
        /* Application Identifier. */
        mem_read(gif, app_id, 8);
        /* Application Authentication Code. */
        mem_read(gif, app_auth_code, 3);
        if (!strncmp(app_id, "NETSCAPE", sizeof(app_id)))
        {
            /* Discard block size (0x03) and constant byte (0x01). */
            mem_lseek(gif, 2, SEEK_CUR);
            gif->loop_count = read_num(gif);
            /* Skip block terminator. */
            mem_lseek(gif, 1, SEEK_CUR);
        }
        else if (gif->application)
        {
            long sub_block = mem_lseek(gif, 0, SEEK_CUR);
            gif->application(gif, app_id, app_auth_code);
            mem_lseek(gif, sub_block, SEEK_SET);
            discard_sub_blocks(gif);
        }
        else
        {
            discard_sub_blocks(gif);
        }
    }
    else
    {
        /* Discard block size (always 0x0B). */
        port_lseek(gif->source.fd, 1, SEEK_CUR);
        /* Application Identifier. */
        port_read(gif->source.fd, app_id, 8);
        /* Application Authentication Code. */
        port_read(gif->source.fd, app_auth_code, 3);
        if (!strncmp(app_id, "NETSCAPE", sizeof(app_id)))
        {
            /* Discard block size (0x03) and constant byte (0x01). */
            port_lseek(gif->source.fd, 2, SEEK_CUR);
            gif->loop_count = read_num(gif);
            /* Skip block terminator. */
            port_lseek(gif->source.fd, 1, SEEK_CUR);
        }
        else if (gif->application)
        {
            long sub_block = port_lseek(gif->source.fd, 0, SEEK_CUR);
            gif->application(gif, app_id, app_auth_code);
            port_lseek(gif->source.fd, sub_block, SEEK_SET);
            discard_sub_blocks(gif);
        }
        else
        {
            discard_sub_blocks(gif);
        }
    }
}

static void read_ext(gd_GIF *gif)
{
    uint8_t label;

    if (gif->is_mem)
    {
        mem_read(gif, &label, 1);
    }
    else
    {
        port_read(gif->source.fd, &label, 1);
    }

    switch (label)
    {
    case 0x01:
        read_plain_text_ext(gif);
        break;
    case 0xF9:
        read_graphic_control_ext(gif);
        break;
    case 0xFE:
        read_comment_ext(gif);
        break;
    case 0xFF:
        read_application_ext(gif);
        break;
    default:
        fprintf(stderr, "unknown extension: %02X\n", label);
    }
}

static Table *new_table(int key_size)
{
    int key;
    int init_bulk = MAX(1 << (key_size + 1), 0x100);
    Table *table = port_malloc(sizeof(*table) + sizeof(Entry) * init_bulk);
    if (table)
    {
        table->bulk = init_bulk;
        table->nentries = (1 << key_size) + 2;
        table->entries = (Entry *) &table[1];
        for (key = 0; key < (1 << key_size); key++)
            table->entries[key] = (Entry) {1, 0xFFF, key};
    }
    return table;
}

/* Add table entry. Return value:
 *  0 on success
 *  +1 if key size must be incremented after this addition
 *  -1 if could not realloc table */
static int add_entry(Table **tablep, uint16_t length, uint16_t prefix, uint8_t suffix)
{
    Table *table = *tablep;
    if (table->nentries == table->bulk)
    {
        table->bulk *= 2;
        table = port_realloc(table, sizeof(*table) + sizeof(Entry) * table->bulk);
        if (!table) { return -1; }
        table->entries = (Entry *) &table[1];
        *tablep = table;
    }
    table->entries[table->nentries] = (Entry) {length, prefix, suffix};
    table->nentries++;
    if ((table->nentries & (table->nentries - 1)) == 0)
    {
        return 1;
    }
    return 0;
}

static uint16_t get_key(gd_GIF *gif, int key_size, uint8_t *sub_len, uint8_t *shift, uint8_t *byte)
{
    int bits_read;
    int rpad;
    int frag_size;
    uint16_t key;

    key = 0;
    for (bits_read = 0; bits_read < key_size; bits_read += frag_size)
    {
        rpad = (*shift + bits_read) % 8;
        if (rpad == 0)
        {
            /* Update byte. */
            if (*sub_len == 0)
            {
                if (gif->is_mem)
                {
                    mem_read(gif, sub_len, 1); /* Must be nonzero! */
                }
                else
                {
                    port_read(gif->source.fd, sub_len, 1); /* Must be nonzero! */
                }
                if (*sub_len == 0)
                {
                    return 0x1000;
                }
            }
            if (gif->is_mem)
            {
                mem_read(gif, byte, 1);
            }
            else
            {
                port_read(gif->source.fd, byte, 1);
            }
            (*sub_len)--;
        }
        frag_size = MIN(key_size - bits_read, 8 - rpad);
        key |= ((uint16_t)((*byte) >> rpad)) << bits_read;
    }
    /* Clear extra bits to the left. */
    key &= (1 << key_size) - 1;
    *shift = (*shift + key_size) % 8;
    return key;
}

/* Compute output index of y-th input line, in frame of height h. */
static int
interlaced_line_index(int h, int y)
{
    int p; /* number of lines in current pass */

    p = (h - 1) / 8 + 1;
    if (y < p) /* pass 1 */
    {
        return y * 8;
    }
    y -= p;
    p = (h - 5) / 8 + 1;
    if (y < p) /* pass 2 */
    {
        return y * 8 + 4;
    }
    y -= p;
    p = (h - 3) / 4 + 1;
    if (y < p) /* pass 3 */
    {
        return y * 4 + 2;
    }
    y -= p;
    /* pass 4 */
    return y * 2 + 1;
}

/* Decompress image pixels.
 * Return 0 on success or -1 on out-of-memory (w.r.t. LZW code table). */
static int
read_image_data(gd_GIF *gif, int interlace)
{
    uint8_t sub_len, shift, byte;
    int init_key_size, key_size, table_is_full;
    int frm_off, frm_size, str_len, i, p, x, y;
    uint16_t key, clear, stop;
    int ret;
    Table *table;
    Entry entry;
    long start, end;

    if (gif->is_mem)
    {
        mem_read(gif, &byte, 1);
    }
    else
    {
        port_read(gif->source.fd, &byte, 1);
    }

    key_size = (int) byte;
    if (key_size < 2 || key_size > 8)
    {
        return -1;
    }

    if (gif->is_mem)
    {
        start = mem_lseek(gif, 0, SEEK_CUR);
        discard_sub_blocks(gif);
        end = mem_lseek(gif, 0, SEEK_CUR);
        mem_lseek(gif, start, SEEK_SET);
    }
    else
    {
        start = port_lseek(gif->source.fd, 0, SEEK_CUR);
        discard_sub_blocks(gif);
        end = port_lseek(gif->source.fd, 0, SEEK_CUR);
        port_lseek(gif->source.fd, start, SEEK_SET);
    }
    clear = 1 << key_size;
    stop = clear + 1;
    table = new_table(key_size);
    key_size++;
    init_key_size = key_size;
    sub_len = shift = 0;
    key = get_key(gif, key_size, &sub_len, &shift, &byte); /* clear code */
    frm_off = 0;
    ret = 0;
    frm_size = gif->fw * gif->fh;
    while (frm_off < frm_size)
    {
        if (key == clear)
        {
            key_size = init_key_size;
            table->nentries = (1 << (key_size - 1)) + 2;
            table_is_full = 0;
        }
        else if (!table_is_full)
        {
            ret = add_entry(&table, str_len + 1, key, entry.suffix);
            if (ret == -1)
            {
                port_free(table);
                return -1;
            }
            if (table->nentries == 0x1000)
            {
                ret = 0;
                table_is_full = 1;
            }
        }
        key = get_key(gif, key_size, &sub_len, &shift, &byte);
        if (key == clear) { continue; }
        if (key == stop || key == 0x1000) { break; }
        if (ret == 1) { key_size++; }
        entry = table->entries[key];
        str_len = entry.length;
        for (i = 0; i < str_len; i++)
        {
            p = frm_off + entry.length - 1;
            x = p % gif->fw;
            y = p / gif->fw;
            if (interlace)
            {
                y = interlaced_line_index((int) gif->fh, y);
            }
            if (!(gif->gce.transparency && (entry.suffix == gif->gce.tindex)))  // add by wanghao
            {
                gif->frame[(gif->fy + y) * gif->width + gif->fx + x] = entry.suffix;
            }
            if (entry.prefix == 0xFFF)
            {
                break;
            }
            else
            {
                entry = table->entries[entry.prefix];
            }
        }
        frm_off += str_len;
        if (key < table->nentries - 1 && !table_is_full)
        {
            table->entries[table->nentries - 1].suffix = entry.suffix;
        }
    }
    port_free(table);
    if (key == stop)
    {
        if (gif->is_mem)
        {
            mem_read(gif, &sub_len, 1);    /* Must be zero! */
        }
        else
        {
            port_read(gif->source.fd, &sub_len, 1);    /* Must be zero! */
        }
    }

    if (gif->is_mem)
    {
        mem_lseek(gif, end, SEEK_SET);
    }
    else
    {
        port_lseek(gif->source.fd, end, SEEK_SET);
    }
    return 0;
}

/* Read image.
 * Return 0 on success or -1 on out-of-memory (w.r.t. LZW code table). */
static int
read_image(gd_GIF *gif)
{
    uint8_t fisrz;
    int interlace;

    /* Image Descriptor. */
    gif->fx = read_num(gif);
    gif->fy = read_num(gif);

    if (gif->fx >= gif->width || gif->fy >= gif->height)
    {
        return -1;
    }

    gif->fw = read_num(gif);
    gif->fh = read_num(gif);

    gif->fw = MIN(gif->fw, gif->width - gif->fx);
    gif->fh = MIN(gif->fh, gif->height - gif->fy);

    if (gif->is_mem)
    {
        mem_read(gif, &fisrz, 1);
    }
    else
    {
        port_read(gif->source.fd, &fisrz, 1);
    }

    interlace = fisrz & 0x40;
    /* Ignore Sort Flag. */
    /* Local Color Table? */
    if (fisrz & 0x80)
    {
        /* Read LCT */
        gif->lct.size = 1 << ((fisrz & 0x07) + 1);
        if (gif->is_mem)
        {
            mem_read(gif, gif->lct.colors, 3 * gif->lct.size);
        }
        else
        {
            port_read(gif->source.fd, gif->lct.colors, 3 * gif->lct.size);
        }
        gif->palette = &gif->lct;
    }
    else
    {
        gif->palette = &gif->gct;
    }
    /* Image Data. */
    return read_image_data(gif, interlace);
}

static void
render_frame_rect(gd_GIF *gif, uint8_t *buffer)
{
#if ENABLE_HONEYGUI // wanghao comment: this is not used in honeygui
    return;
#else
    int j, k;
    uint8_t index, *color;
    for (j = gif->fy; j < gif->fh + gif->fy; j++)
    {
        for (k = gif->fx; k < gif->fw + gif->fx; k++)
        {
            index = gif->frame[j * gif->width + k];
            color = &gif->palette->colors[index * 3];
            if (!(gif->gce.transparency && index == gif->gce.tindex))
            {
                memcpy(&buffer[(j * gif->width + k) * 3], color, 3);
            }
        }
    }
#endif
}

static void
dispose(gd_GIF *gif)
{
    int i, j, k;
    uint8_t *bgcolor;
    switch (gif->gce.disposal)
    {
    case 2: /* Restore to background color. */
        bgcolor = &gif->palette->colors[gif->bgindex * 3];
        i = gif->fy * gif->width + gif->fx;
        for (j = 0; j < gif->fh; j++)
        {
            for (k = 0; k < gif->fw; k++)
            {
                memcpy(&gif->canvas[(i + k) * 3], bgcolor, 3);
            }
            i += gif->width;
        }
        break;
    case 3: /* Restore to previous, i.e., don't update canvas.*/
        break;
    default:
        /* Add frame non-transparent pixels to canvas. */
        render_frame_rect(gif, gif->canvas);
    }
}

/* Return 1 if got a frame; 0 if got GIF trailer; -1 if error. */
int
gd_get_frame(gd_GIF *gif)
{
    char sep;

    dispose(gif);
    if (gif->is_mem)
    {
        mem_read(gif, &sep, 1);
    }
    else
    {
        port_read(gif->source.fd, &sep, 1);
    }

    while (sep != ',')
    {
        if (sep == ';')
        {
            return 0;
        }
        if (sep == '!')
        {
            read_ext(gif);
        }
        else { return -1; }

        if (gif->is_mem)
        {
            mem_read(gif, &sep, 1);
        }
        else
        {
            port_read(gif->source.fd, &sep, 1);
        }
    }
    if (read_image(gif) == -1)
    {
        return -1;
    }
    return 1;
}

void
gd_render_frame(gd_GIF *gif, uint8_t *buffer)
{
    memcpy(buffer, gif->canvas, gif->width * gif->height * 3);
    render_frame_rect(gif, buffer);
}

int
gd_is_bgcolor(gd_GIF *gif, uint8_t color[3])
{
    return !memcmp(&gif->palette->colors[gif->bgindex * 3], color, 3);
}

void
gd_rewind(gd_GIF *gif)
{
    if (gif->is_mem)
    {
        mem_lseek(gif, gif->anim_start, SEEK_SET);
    }
    else
    {
        port_lseek(gif->source.fd, gif->anim_start, SEEK_SET);
    }
}

void
gd_close_gif(gd_GIF *gif)
{
    if (!gif->is_mem)
    {
        port_close(gif->source.fd);
    }
    port_free(gif->frame);
    port_free(gif);
}
