#if defined _WIN32
extern unsigned char resource_root[];

#define   APP_FACEBOOK_BIN                        (void *)(resource_root + 0x00000078)
#define   LAKE_240_240_BIN                        (void *)(resource_root + 0x000020b8)

#else
#define   APP_FACEBOOK_BIN                        (void *)(0x00858078)
#define   LAKE_240_240_BIN                        (void *)(0x0085a0b8)

#endif
