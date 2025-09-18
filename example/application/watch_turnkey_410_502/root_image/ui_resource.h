#if defined _WIN32
extern unsigned char *resource_root;

#define   CJSON_DATA_BIN                          (void *)(resource_root + 0x00000078)
#define   INTERREGULAR_BIN                        (void *)(resource_root + 0x00000338)

#else
#define   CJSON_DATA_BIN                          (void *)(0x704d1478)
#define   INTERREGULAR_BIN                        (void *)(0x704d1738)

#endif
