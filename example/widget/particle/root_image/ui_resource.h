#if defined _HONEYGUI_SIMULATOR_
extern unsigned char *resource_root;

#define   NOTOSANS_REGULAR_VECTOR_BIN              (void *)(resource_root + 0x000000b8)
#define   GALAXY_CIRCLE_BIN                        (void *)(resource_root + 0x00006778)
#define   MAGIC_INNER_BIN                          (void *)(resource_root + 0x0001cd78)
#define   MAGIC_OUTER_BIN                          (void *)(resource_root + 0x00038f78)
#define   ROCKET_BIN                               (void *)(resource_root + 0x0008b638)

#else
#define   NOTOSANS_REGULAR_VECTOR_BIN              (void *)(0x704d14b8)
#define   GALAXY_CIRCLE_BIN                        (void *)(0x704d7b78)
#define   MAGIC_INNER_BIN                          (void *)(0x704ee178)
#define   MAGIC_OUTER_BIN                          (void *)(0x7050a378)
#define   ROCKET_BIN                               (void *)(0x7055ca38)

#endif
