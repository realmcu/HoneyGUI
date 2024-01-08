#if defined _WIN32
extern unsigned char resource_root[];

#define   APPS_BIN                                (void *)(resource_root + 0x00000138)
#define   BACK_BIN                                (void *)(resource_root + 0x000233f8)
#define   BATTERY_BIN                             (void *)(resource_root + 0x000c4438)
#define   BLUETOOTH_BIN                           (void *)(resource_root + 0x000e76f8)
#define   DEVICE_NAME_BIN                         (void *)(resource_root + 0x0010a9f8)
#define   REALGUI_BIN                             (void *)(resource_root + 0x0012a478)
#define   STORAGE_BIN                             (void *)(resource_root + 0x00177a38)
#define   DISPLAY_BIN                             (void *)(resource_root + 0x001a2e38)
#define   MYDEVICE_BIN                            (void *)(resource_root + 0x001c60f8)
#define   RTOUCH_BIN                              (void *)(resource_root + 0x001e93b8)
#define   SECURITY_BIN                            (void *)(resource_root + 0x0020c678)
#define   SETTINGSBACK_BIN                        (void *)(resource_root + 0x0022f938)
#define   SOUND_BIN                               (void *)(resource_root + 0x00252bf8)
#define   TEXTSETTING_BIN                         (void *)(resource_root + 0x00275eb8)
#define   WALLET_BIN                              (void *)(resource_root + 0x0028a1f8)
#define   WLAN_BIN                                (void *)(resource_root + 0x002ad4b8)

#else
#define   APPS_BIN                                (void *)(0x0c000138)
#define   BACK_BIN                                (void *)(0x0c0233f8)
#define   BATTERY_BIN                             (void *)(0x0c0c4438)
#define   BLUETOOTH_BIN                           (void *)(0x0c0e76f8)
#define   DEVICE_NAME_BIN                         (void *)(0x0c10a9f8)
#define   REALGUI_BIN                             (void *)(0x0c12a478)
#define   STORAGE_BIN                             (void *)(0x0c177a38)
#define   DISPLAY_BIN                             (void *)(0x0c1a2e38)
#define   MYDEVICE_BIN                            (void *)(0x0c1c60f8)
#define   RTOUCH_BIN                              (void *)(0x0c1e93b8)
#define   SECURITY_BIN                            (void *)(0x0c20c678)
#define   SETTINGSBACK_BIN                        (void *)(0x0c22f938)
#define   SOUND_BIN                               (void *)(0x0c252bf8)
#define   TEXTSETTING_BIN                         (void *)(0x0c275eb8)
#define   WALLET_BIN                              (void *)(0x0c28a1f8)
#define   WLAN_BIN                                (void *)(0x0c2ad4b8)

#endif
