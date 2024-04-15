#if defined _WIN32
extern unsigned char resource_root[1024 * 1024 * 20];

#define   ARIALBD_SIZE16_BITS4_FONT_BIN           (void *)(resource_root + 0x000000f8)
#define   COMPRESS8565_BIN                        (void *)(resource_root + 0x000232b8)
#define   COMPRESS888_BIN                         (void *)(resource_root + 0x00024838)
#define   COMPRESS8888_BIN                        (void *)(resource_root + 0x00025678)
#define   COMPRESS_565_BIN                        (void *)(resource_root + 0x00027138)
#define   COMPRESS_MIX_565_BIN                    (void *)(resource_root + 0x00027bf8)
#define   IMAGE565_BIN                            (void *)(resource_root + 0x00028bb8)
#define   IMAGE8565_BIN                           (void *)(resource_root + 0x0002c938)
#define   IMAGE888_BIN                            (void *)(resource_root + 0x00032578)
#define   IMAGE8888_BIN                           (void *)(resource_root + 0x000381b8)
#define   MIX565_BIN                              (void *)(resource_root + 0x0003fcb8)
#define   PATH111_BIN                             (void *)(resource_root + 0x00043a38)

#else
#define   ARIALBD_SIZE16_BITS4_FONT_BIN           (void *)(0x022fe4f8)
#define   COMPRESS8565_BIN                        (void *)(0x023216b8)
#define   COMPRESS888_BIN                         (void *)(0x02322c38)
#define   COMPRESS8888_BIN                        (void *)(0x02323a78)
#define   COMPRESS_565_BIN                        (void *)(0x02325538)
#define   COMPRESS_MIX_565_BIN                    (void *)(0x02325ff8)
#define   IMAGE565_BIN                            (void *)(0x02326fb8)
#define   IMAGE8565_BIN                           (void *)(0x0232ad38)
#define   IMAGE888_BIN                            (void *)(0x02330978)
#define   IMAGE8888_BIN                           (void *)(0x023365b8)
#define   MIX565_BIN                              (void *)(0x0233e0b8)
#define   PATH111_BIN                             (void *)(0x02341e38)

#endif
