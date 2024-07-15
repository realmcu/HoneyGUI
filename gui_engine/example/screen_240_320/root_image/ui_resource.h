#if defined _WIN32
extern unsigned char resource_root[];

#define   APP_FACEBOOK_BIN                        (void *)(resource_root + 0x00000078)
#define   LAKE_BIN                                (void *)(resource_root + 0x000020b8)
#define   LAKE_240_240_BIN                        (void *)(resource_root + 0x000278f8)
#define   TEST_BIN                                (void *)(resource_root + 0x00043b38)
#define   WHITE_240_240_BIN                       (void *)(resource_root + 0x0004b578)

#else
#define   APP_FACEBOOK_BIN                        (void *)(0x00858078)
#define   LAKE_BIN                                (void *)(0x0085a0b8)
#define   LAKE_240_240_BIN                        (void *)(0x0087f8f8)
#define   TEST_BIN                                (void *)(0x0089bb38)
#define   WHITE_240_240_BIN                       (void *)(0x008a3578)

#endif
