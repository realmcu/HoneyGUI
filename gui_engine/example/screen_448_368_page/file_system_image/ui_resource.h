#if defined _WIN32
extern unsigned char resource_root[];

#define   APPS_BIN                                (void *)(resource_root + 0x000000f8)
#define   BACK_BIN                                (void *)(resource_root + 0x000233b8)
#define   BATTERY_BIN                             (void *)(resource_root + 0x000c43f8)
#define   BLUETOOTH_BIN                           (void *)(resource_root + 0x000e76b8)
#define   DISPLAY_BIN                             (void *)(resource_root + 0x0010a978)
#define   MYDEVICE_BIN                            (void *)(resource_root + 0x0012dc38)
#define   RTOUCH_BIN                              (void *)(resource_root + 0x00150ef8)
#define   SECURITY_BIN                            (void *)(resource_root + 0x001741b8)
#define   SETTINGSBACK_BIN                        (void *)(resource_root + 0x00197478)
#define   SOUND_BIN                               (void *)(resource_root + 0x001ba738)
#define   TEXTSETTING_BIN                         (void *)(resource_root + 0x001dd9f8)
#define   WALLET_BIN                              (void *)(resource_root + 0x001f1d38)
#define   WLAN_BIN                                (void *)(resource_root + 0x00214ff8)

#else
#define   APPS_BIN                                (void *)(0x0c0000f8)
#define   BACK_BIN                                (void *)(0x0c0233b8)
#define   BATTERY_BIN                             (void *)(0x0c0c43f8)
#define   BLUETOOTH_BIN                           (void *)(0x0c0e76b8)
#define   DISPLAY_BIN                             (void *)(0x0c10a978)
#define   MYDEVICE_BIN                            (void *)(0x0c12dc38)
#define   RTOUCH_BIN                              (void *)(0x0c150ef8)
#define   SECURITY_BIN                            (void *)(0x0c1741b8)
#define   SETTINGSBACK_BIN                        (void *)(0x0c197478)
#define   SOUND_BIN                               (void *)(0x0c1ba738)
#define   TEXTSETTING_BIN                         (void *)(0x0c1dd9f8)
#define   WALLET_BIN                              (void *)(0x0c1f1d38)
#define   WLAN_BIN                                (void *)(0x0c214ff8)

#endif
