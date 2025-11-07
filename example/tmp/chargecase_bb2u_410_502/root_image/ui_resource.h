#if defined _WIN32
extern unsigned char *resource_root;

#define   EARTH_420_410_502_40_LQ_MJPG            (void *)(resource_root + 0x00000078)

#else
#define   EARTH_420_410_502_40_LQ_MJPG            (void *)(0x704d1478)

#endif
