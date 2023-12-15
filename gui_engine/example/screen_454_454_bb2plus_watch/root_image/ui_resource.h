#if defined _WIN32
extern unsigned char resource_root[1024 * 1024 * 20];

#define   OVAL_BIN                                (void *)(resource_root + 0x000008f8)
#define   CLOCK_BIN                               (void *)(resource_root + 0x00001138)
#define   CLOCK_BASE_BIN                          (void *)(resource_root + 0x00065bb8)
#define   ICON_ANSWER_BIN                         (void *)(resource_root + 0x000ca638)
#define   ICON_ANSWER_TOUCH_BIN                   (void *)(resource_root + 0x000cd878)
#define   ICON_BACK_BIN                           (void *)(resource_root + 0x000d0ab8)
#define   ICON_BACKSPACE_BIN                      (void *)(resource_root + 0x000d1cf8)
#define   ICON_BLUETOOTH_OFF_BIN                  (void *)(resource_root + 0x000d3d38)
#define   ICON_BLUETOOTH_ON_BIN                   (void *)(resource_root + 0x000d8b78)
#define   ICON_BLUETOOTH_TOUCH_BIN                (void *)(resource_root + 0x000dd9b8)
#define   ICON_BUDS_OFF_BIN                       (void *)(resource_root + 0x000e27f8)
#define   ICON_BUDS_ON_BIN                        (void *)(resource_root + 0x000e7638)
#define   ICON_BUDS_TOUCH_BIN                     (void *)(resource_root + 0x000ec478)
#define   ICON_BUTTON_OFF_BIN                     (void *)(resource_root + 0x000f12b8)
#define   ICON_BUTTON_ON_BIN                      (void *)(resource_root + 0x000f32f8)
#define   ICON_CALL_BIN                           (void *)(resource_root + 0x000f5338)
#define   ICON_CALL_CONTACTS_BIN                  (void *)(resource_root + 0x00107f78)
#define   ICON_CALL_CONTACTS_TOUCH_BIN            (void *)(resource_root + 0x00112fb8)
#define   ICON_CALL_HISTORY_BIN                   (void *)(resource_root + 0x0011dff8)
#define   ICON_CALL_HISTORY_TOUCH_BIN             (void *)(resource_root + 0x00129038)
#define   ICON_CALL_TOUCH_BIN                     (void *)(resource_root + 0x00134078)
#define   ICON_CALL_VOLUME_BAR_BIN                (void *)(resource_root + 0x00146cb8)
#define   ICON_CALL_VOLUME_BAR_1_BIN              (void *)(resource_root + 0x00147bf8)
#define   ICON_CALL_VOLUME_BAR_2_BIN              (void *)(resource_root + 0x00147db8)
#define   ICON_CALLER_BIN                         (void *)(resource_root + 0x00148cf8)
#define   ICON_CALLER_LARGE_BIN                   (void *)(resource_root + 0x00150738)
#define   ICON_CANCEL_BIN                         (void *)(resource_root + 0x001636b8)
#define   ICON_CANCEL_TOUCH_BIN                   (void *)(resource_root + 0x00167ef8)
#define   ICON_CLOCK_HAND_DOT_BIN                 (void *)(resource_root + 0x0016c738)
#define   ICON_CLOCK_HOUR_HAND_BIN                (void *)(resource_root + 0x0016c978)
#define   ICON_CLOCK_MINUTE_HAND_BIN              (void *)(resource_root + 0x0016cfb8)
#define   ICON_CLOCK_SECOND_HAND_BIN              (void *)(resource_root + 0x0016d8f8)
#define   ICON_CONFIRM_BIN                        (void *)(resource_root + 0x0016ddb8)
#define   ICON_CONFIRM_TOUCH_BIN                  (void *)(resource_root + 0x001725f8)
#define   ICON_CONTACTCS_BIN                      (void *)(resource_root + 0x00176e38)
#define   ICON_CONTACTCS_TOUCH_BIN                (void *)(resource_root + 0x00189a78)
#define   ICON_DIAL_0_BIN                         (void *)(resource_root + 0x0019c6b8)
#define   ICON_DIAL_1_BIN                         (void *)(resource_root + 0x001a10b8)
#define   ICON_DIAL_2_BIN                         (void *)(resource_root + 0x001a5ab8)
#define   ICON_DIAL_3_BIN                         (void *)(resource_root + 0x001aa4b8)
#define   ICON_DIAL_4_BIN                         (void *)(resource_root + 0x001aeeb8)
#define   ICON_DIAL_5_BIN                         (void *)(resource_root + 0x001b38b8)
#define   ICON_DIAL_6_BIN                         (void *)(resource_root + 0x001b82b8)
#define   ICON_DIAL_7_BIN                         (void *)(resource_root + 0x001bccb8)
#define   ICON_DIAL_8_BIN                         (void *)(resource_root + 0x001c1078)
#define   ICON_DIAL_9_BIN                         (void *)(resource_root + 0x001c5a78)
#define   ICON_DIAL_TOUCH_0_BIN                   (void *)(resource_root + 0x001ca478)
#define   ICON_DIAL_TOUCH_1_BIN                   (void *)(resource_root + 0x001cee78)
#define   ICON_DIAL_TOUCH_2_BIN                   (void *)(resource_root + 0x001d3878)
#define   ICON_DIAL_TOUCH_3_BIN                   (void *)(resource_root + 0x001d8278)
#define   ICON_DIAL_TOUCH_4_BIN                   (void *)(resource_root + 0x001dcc78)
#define   ICON_DIAL_TOUCH_5_BIN                   (void *)(resource_root + 0x001e1678)
#define   ICON_DIAL_TOUCH_6_BIN                   (void *)(resource_root + 0x001e6078)
#define   ICON_DIAL_TOUCH_7_BIN                   (void *)(resource_root + 0x001eaa78)
#define   ICON_DIAL_TOUCH_8_BIN                   (void *)(resource_root + 0x001ef6b8)
#define   ICON_DIAL_TOUCH_9_BIN                   (void *)(resource_root + 0x001f40b8)
#define   ICON_DISCONNECT_BIN                     (void *)(resource_root + 0x001f8ab8)
#define   ICON_DISCONNECT_TOUCH_BIN               (void *)(resource_root + 0x00203af8)
#define   ICON_FAIL_BIN                           (void *)(resource_root + 0x0020eb38)
#define   ICON_FILES_BASE_BLUE_BIN                (void *)(resource_root + 0x00216b78)
#define   ICON_FILES_BASE_DARK_BIN                (void *)(resource_root + 0x0021fbb8)
#define   ICON_FILES_BASE_RED_BIN                 (void *)(resource_root + 0x00228bf8)
#define   ICON_FOLDER_BIN                         (void *)(resource_root + 0x00231c38)
#define   ICON_HANGUP_BIN                         (void *)(resource_root + 0x002320f8)
#define   ICON_HANGUP_TOUCH_BIN                   (void *)(resource_root + 0x00235338)
#define   ICON_LOADING_BIN                        (void *)(resource_root + 0x00238578)
#define   ICON_MIC_MUTE_OFF_BIN                   (void *)(resource_root + 0x0023cf38)
#define   ICON_MIC_MUTE_OFF_TOUCH_BIN             (void *)(resource_root + 0x00240178)
#define   ICON_MIC_MUTE_ON_BIN                    (void *)(resource_root + 0x002433b8)
#define   ICON_MIC_MUTE_ON_TOUCH_BIN              (void *)(resource_root + 0x002465f8)
#define   ICON_MORE_OFF_BIN                       (void *)(resource_root + 0x00249838)
#define   ICON_MORE_ON_BIN                        (void *)(resource_root + 0x0024e678)
#define   ICON_MORE_TOUCH_BIN                     (void *)(resource_root + 0x002534b8)
#define   ICON_PHONE_OFF_BIN                      (void *)(resource_root + 0x002582f8)
#define   ICON_PHONE_ON_BIN                       (void *)(resource_root + 0x0025d138)
#define   ICON_PHONE_TOUCH_BIN                    (void *)(resource_root + 0x00261f78)
#define   ICON_PROCESS_BAR_BIN                    (void *)(resource_root + 0x00266db8)
#define   ICON_PROCESS_BAR_1_BIN                  (void *)(resource_root + 0x00267ff8)
#define   ICON_PROCESS_NODE_BIN                   (void *)(resource_root + 0x002680f8)
#define   ICON_PROCESS_NODE_TOUCH_BIN             (void *)(resource_root + 0x00268938)
#define   ICON_RECORD_DELETE_BIN                  (void *)(resource_root + 0x00269178)
#define   ICON_RECORD_DELETE_TOUCH_BIN            (void *)(resource_root + 0x002740f8)
#define   ICON_RECORD_PAUSE_BIN                   (void *)(resource_root + 0x0027f078)
#define   ICON_RECORD_PAUSE_TOUCH_BIN             (void *)(resource_root + 0x002910b8)
#define   ICON_RECORD_PLAY_BIN                    (void *)(resource_root + 0x002a30f8)
#define   ICON_RECORD_PLAY_BAR_BIN                (void *)(resource_root + 0x002b5138)
#define   ICON_RECORD_PLAY_TOUCH_BIN              (void *)(resource_root + 0x002b6f78)
#define   ICON_RECORD_START_BIN                   (void *)(resource_root + 0x002c8fb8)
#define   ICON_RECORD_STOP_BIN                    (void *)(resource_root + 0x002daff8)
#define   ICON_SEEKBAR_BIN                        (void *)(resource_root + 0x002ed038)
#define   ICON_SEEKBAR1_BIN                       (void *)(resource_root + 0x002ee8f8)
#define   ICON_SEEKBAR2_BIN                       (void *)(resource_root + 0x002eec38)
#define   ICON_SONG_BUDS_BIN                      (void *)(resource_root + 0x002f04f8)
#define   ICON_SONG_LOCAL_BIN                     (void *)(resource_root + 0x002f0d38)
#define   ICON_SONG_NEXT_BIN                      (void *)(resource_root + 0x002f1578)
#define   ICON_SONG_NEXT_TOUCH_BIN                (void *)(resource_root + 0x002f2f38)
#define   ICON_SONG_PAUSE_BIN                     (void *)(resource_root + 0x002f48b8)
#define   ICON_SONG_PAUSE_TOUCH_BIN               (void *)(resource_root + 0x002f68f8)
#define   ICON_SONG_PHONE_BIN                     (void *)(resource_root + 0x002f8938)
#define   ICON_SONG_PLAY_BIN                      (void *)(resource_root + 0x002f9178)
#define   ICON_SONG_PLAY_TOUCH_BIN                (void *)(resource_root + 0x002fb1b8)
#define   ICON_SONG_PREV_BIN                      (void *)(resource_root + 0x002fd1f8)
#define   ICON_SONG_PREV_TOUCH_BIN                (void *)(resource_root + 0x002feb38)
#define   ICON_SONG_SPK_BIN                       (void *)(resource_root + 0x00300478)
#define   ICON_SUCCESS_BIN                        (void *)(resource_root + 0x00300cb8)
#define   ICON_SWITCH_OFF_BIN                     (void *)(resource_root + 0x00308cf8)
#define   ICON_SWITCH_ON_BIN                      (void *)(resource_root + 0x0030bbf8)
#define   ICON_TEXT_BASE_BLUE_BIN                 (void *)(resource_root + 0x0030eaf8)
#define   ICON_TEXT_BASE_DARK_BIN                 (void *)(resource_root + 0x00317b38)
#define   ICON_TEXT_BASE_RED_BIN                  (void *)(resource_root + 0x00320b78)
#define   ICON_TEXT_BASE_SHORT_BIN                (void *)(resource_root + 0x00329bb8)
#define   ICON_TOUCH_BACKSPACE_BIN                (void *)(resource_root + 0x0032e3f8)
#define   ICON_TRASH_BIN                          (void *)(resource_root + 0x00330438)
#define   ICON_TRASH_TOUCH_BIN                    (void *)(resource_root + 0x00332478)
#define   ICON_UP_SLIDE_BIN                       (void *)(resource_root + 0x003344b8)
#define   ICON_VOLUME_BIN                         (void *)(resource_root + 0x0033f4f8)
#define   ICON_VOLUME_BAR_BIN                     (void *)(resource_root + 0x0033fd38)
#define   ICON_VOLUME_BAR_1_BIN                   (void *)(resource_root + 0x00340c78)
#define   ICON_VOLUME_BAR_2_BIN                   (void *)(resource_root + 0x00340e38)
#define   ICON_VOLUME_CALL_BIN                    (void *)(resource_root + 0x00341d78)
#define   ICON_VOLUME_CALL_TOUCH_BIN              (void *)(resource_root + 0x00344fb8)
#define   ICON_VOLUME_DOWN_BIN                    (void *)(resource_root + 0x003481f8)
#define   ICON_VOLUME_DOWN_TOUCH_BIN              (void *)(resource_root + 0x00349438)
#define   ICON_VOLUME_TOUCH_BIN                   (void *)(resource_root + 0x0034a678)
#define   ICON_VOLUME_UP_BIN                      (void *)(resource_root + 0x0034b8b8)
#define   ICON_VOLUME_UP_TOUCH_BIN                (void *)(resource_root + 0x0034caf8)
#define   RECORD_FILES_ICON_FOLDER_BIN            (void *)(resource_root + 0x0034dd38)
#define   RECORD_PLAY_BAR1_BIN                    (void *)(resource_root + 0x0034e1f8)
#define   RECORD_PLAY_BAR2_BIN                    (void *)(resource_root + 0x0034e2f8)
#define   RECORD_PLAY_BAR_DOT_BIN                 (void *)(resource_root + 0x0034f238)
#define   WATCH_BASE_BLACK_BIN                    (void *)(resource_root + 0x0034f478)
#define   WATCH_BASE_BLACK_LIGHT_BIN              (void *)(resource_root + 0x003b3ef8)
#define   WATCH_BASE_GREY_BIN                     (void *)(resource_root + 0x00418978)
#define   WATCH_BASE_GREY_70_ALPHA_BIN            (void *)(resource_root + 0x0047d3f8)
#define   WATCH_BASE_GREY_90ALPHA_HALF_BIN        (void *)(resource_root + 0x004e1e78)
#define   WATCH_BASE_MASK_BLACK_BIN               (void *)(resource_root + 0x005143f8)
#define   WATCH_BASE_MASK_GREY_BIN                (void *)(resource_root + 0x0052a6f8)
#define   SIMKAI_SIZE24_BITS4_FONT_BIN            (void *)(resource_root + 0x00540a38)
#define   SIMKAI_SIZE32_BITS1_FONT_BIN            (void *)(resource_root + 0x00559db8)
#define   SIMKAI_SIZE32_BITS4_FONT_BIN            (void *)(resource_root + 0x00a55a78)

#else
#include "flash_map.h"
#define   SIMKAI_SIZE32_BITS1_FONT_BIN            (uint8_t *)(USER_DATA1_ADDR)
#define   SIMKAI_SIZE32_BITS1_FONT_BIN_SIZE             0x4FBCA1
#define   USER_DATA_RES_BASE_ADDR                                       SIMKAI_SIZE32_BITS1_FONT_BIN + SIMKAI_SIZE32_BITS1_FONT_BIN_SIZE
#define   CLOCK_BIN                           (uint32_t *)(USER_DATA_RES_BASE_ADDR+0) //454x454
#define   CLOCK_BASE_BIN                      (uint32_t *)(USER_DATA_RES_BASE_ADDR+412288) //454x454
#define   ICON_ANSWER_BIN                     (uint32_t *)(USER_DATA_RES_BASE_ADDR+824576) //80x80
#define   ICON_ANSWER_TOUCH_BIN               (uint32_t *)(USER_DATA_RES_BASE_ADDR+837440) //80x80
#define   ICON_BACK_BIN                       (uint32_t *)(USER_DATA_RES_BASE_ADDR+850304) //48x48
#define   ICON_BACKSPACE_BIN                  (uint32_t *)(USER_DATA_RES_BASE_ADDR+854976) //64x64
#define   ICON_BLUETOOTH_OFF_BIN              (uint32_t *)(USER_DATA_RES_BASE_ADDR+863232) //100x100
#define   ICON_BLUETOOTH_ON_BIN               (uint32_t *)(USER_DATA_RES_BASE_ADDR+883264) //100x100
#define   ICON_BLUETOOTH_TOUCH_BIN            (uint32_t *)(USER_DATA_RES_BASE_ADDR+903296) //100x100
#define   ICON_BUDS_OFF_BIN                   (uint32_t *)(USER_DATA_RES_BASE_ADDR+923328) //100x100
#define   ICON_BUDS_ON_BIN                    (uint32_t *)(USER_DATA_RES_BASE_ADDR+943360) //100x100
#define   ICON_BUDS_TOUCH_BIN                 (uint32_t *)(USER_DATA_RES_BASE_ADDR+963392) //100x100
#define   ICON_BUTTON_OFF_BIN                 (uint32_t *)(USER_DATA_RES_BASE_ADDR+983424) //64x64
#define   ICON_BUTTON_ON_BIN                  (uint32_t *)(USER_DATA_RES_BASE_ADDR+991680) //64x64
#define   ICON_CALL_BIN                       (uint32_t *)(USER_DATA_RES_BASE_ADDR+999936) //196x196
#define   ICON_CALLER_BIN                     (uint32_t *)(USER_DATA_RES_BASE_ADDR+1076800) //125x125
#define   ICON_CALLER_LARGE_BIN               (uint32_t *)(USER_DATA_RES_BASE_ADDR+1108096) //197x197
#define   ICON_CALL_CONTACTS_BIN              (uint32_t *)(USER_DATA_RES_BASE_ADDR+1185728) //278x81
#define   ICON_CALL_CONTACTS_TOUCH_BIN        (uint32_t *)(USER_DATA_RES_BASE_ADDR+1230784) //278x81
#define   ICON_CALL_HISTORY_BIN               (uint32_t *)(USER_DATA_RES_BASE_ADDR+1275840) //278x81
#define   ICON_CALL_HISTORY_TOUCH_BIN         (uint32_t *)(USER_DATA_RES_BASE_ADDR+1320896) //278x81
#define   ICON_CALL_TOUCH_BIN                 (uint32_t *)(USER_DATA_RES_BASE_ADDR+1365952) //196x196
#define   ICON_CALL_VOLUME_BAR_BIN            (uint32_t *)(USER_DATA_RES_BASE_ADDR+1442816) //240x8
#define   ICON_CALL_VOLUME_BAR_1_BIN          (uint32_t *)(USER_DATA_RES_BASE_ADDR+1446720) //24x8
#define   ICON_CALL_VOLUME_BAR_2_BIN          (uint32_t *)(USER_DATA_RES_BASE_ADDR+1447168) //240x8
#define   ICON_CANCEL_BIN                     (uint32_t *)(USER_DATA_RES_BASE_ADDR+1451072) //96x96
#define   ICON_CANCEL_TOUCH_BIN               (uint32_t *)(USER_DATA_RES_BASE_ADDR+1469568) //96x96
#define   ICON_CLOCK_HAND_DOT_BIN             (uint32_t *)(USER_DATA_RES_BASE_ADDR+1488064) //16x16
#define   ICON_CLOCK_HOUR_HAND_BIN            (uint32_t *)(USER_DATA_RES_BASE_ADDR+1488640) //96x8
#define   ICON_CLOCK_MINUTE_HAND_BIN          (uint32_t *)(USER_DATA_RES_BASE_ADDR+1490240) //9x128
#define   ICON_CLOCK_SECOND_HAND_BIN          (uint32_t *)(USER_DATA_RES_BASE_ADDR+1492608) //144x4
#define   ICON_CONFIRM_BIN                    (uint32_t *)(USER_DATA_RES_BASE_ADDR+1493824) //96x96
#define   ICON_CONFIRM_TOUCH_BIN              (uint32_t *)(USER_DATA_RES_BASE_ADDR+1512320) //96x96
#define   ICON_CONTACTCS_BIN                  (uint32_t *)(USER_DATA_RES_BASE_ADDR+1530816) //196x196
#define   ICON_CONTACTCS_TOUCH_BIN            (uint32_t *)(USER_DATA_RES_BASE_ADDR+1607680) //196x196
#define   ICON_DIAL_0_BIN                     (uint32_t *)(USER_DATA_RES_BASE_ADDR+1684544) //118x80
#define   ICON_DIAL_1_BIN                     (uint32_t *)(USER_DATA_RES_BASE_ADDR+1703488) //118x80
#define   ICON_DIAL_2_BIN                     (uint32_t *)(USER_DATA_RES_BASE_ADDR+1722432) //118x80
#define   ICON_DIAL_3_BIN                     (uint32_t *)(USER_DATA_RES_BASE_ADDR+1741376) //118x80
#define   ICON_DIAL_4_BIN                     (uint32_t *)(USER_DATA_RES_BASE_ADDR+1760320) //118x80
#define   ICON_DIAL_5_BIN                     (uint32_t *)(USER_DATA_RES_BASE_ADDR+1779264) //118x80
#define   ICON_DIAL_6_BIN                     (uint32_t *)(USER_DATA_RES_BASE_ADDR+1798208) //118x80
#define   ICON_DIAL_7_BIN                     (uint32_t *)(USER_DATA_RES_BASE_ADDR+1817152) //108x80
#define   ICON_DIAL_8_BIN                     (uint32_t *)(USER_DATA_RES_BASE_ADDR+1834496) //118x80
#define   ICON_DIAL_9_BIN                     (uint32_t *)(USER_DATA_RES_BASE_ADDR+1853440) //118x80
#define   ICON_DIAL_TOUCH_0_BIN               (uint32_t *)(USER_DATA_RES_BASE_ADDR+1872384) //118x80
#define   ICON_DIAL_TOUCH_1_BIN               (uint32_t *)(USER_DATA_RES_BASE_ADDR+1891328) //118x80
#define   ICON_DIAL_TOUCH_2_BIN               (uint32_t *)(USER_DATA_RES_BASE_ADDR+1910272) //118x80
#define   ICON_DIAL_TOUCH_3_BIN               (uint32_t *)(USER_DATA_RES_BASE_ADDR+1929216) //118x80
#define   ICON_DIAL_TOUCH_4_BIN               (uint32_t *)(USER_DATA_RES_BASE_ADDR+1948160) //118x80
#define   ICON_DIAL_TOUCH_5_BIN               (uint32_t *)(USER_DATA_RES_BASE_ADDR+1967104) //118x80
#define   ICON_DIAL_TOUCH_6_BIN               (uint32_t *)(USER_DATA_RES_BASE_ADDR+1986048) //118x80
#define   ICON_DIAL_TOUCH_7_BIN               (uint32_t *)(USER_DATA_RES_BASE_ADDR+2004992) //108x90
#define   ICON_DIAL_TOUCH_8_BIN               (uint32_t *)(USER_DATA_RES_BASE_ADDR+2024448) //118x80
#define   ICON_DIAL_TOUCH_9_BIN               (uint32_t *)(USER_DATA_RES_BASE_ADDR+2043392) //118x80
#define   ICON_DISCONNECT_BIN                 (uint32_t *)(USER_DATA_RES_BASE_ADDR+2062336) //278x81
#define   ICON_DISCONNECT_TOUCH_BIN           (uint32_t *)(USER_DATA_RES_BASE_ADDR+2107392) //278x81
#define   ICON_FAIL_BIN                       (uint32_t *)(USER_DATA_RES_BASE_ADDR+2152448) //128x128
#define   ICON_FILES_BASE_BLUE_BIN            (uint32_t *)(USER_DATA_RES_BASE_ADDR+2185280) //288x64
#define   ICON_FILES_BASE_DARK_BIN            (uint32_t *)(USER_DATA_RES_BASE_ADDR+2222208) //288x64
#define   ICON_FILES_BASE_RED_BIN             (uint32_t *)(USER_DATA_RES_BASE_ADDR+2259136) //288x64
#define   ICON_FOLDER_BIN                     (uint32_t *)(USER_DATA_RES_BASE_ADDR+2296064) //24x24
#define   ICON_HANGUP_BIN                     (uint32_t *)(USER_DATA_RES_BASE_ADDR+2297280) //80x80
#define   ICON_HANGUP_TOUCH_BIN               (uint32_t *)(USER_DATA_RES_BASE_ADDR+2310144) //80x80
#define   ICON_LOADING_BIN                    (uint32_t *)(USER_DATA_RES_BASE_ADDR+2323008) //96x98
#define   ICON_MIC_MUTE_OFF_BIN               (uint32_t *)(USER_DATA_RES_BASE_ADDR+2341888) //80x80
#define   ICON_MIC_MUTE_OFF_TOUCH_BIN         (uint32_t *)(USER_DATA_RES_BASE_ADDR+2354752) //80x80
#define   ICON_MIC_MUTE_ON_BIN                (uint32_t *)(USER_DATA_RES_BASE_ADDR+2367616) //80x80
#define   ICON_MIC_MUTE_ON_TOUCH_BIN          (uint32_t *)(USER_DATA_RES_BASE_ADDR+2380480) //80x80
#define   ICON_MORE_OFF_BIN                   (uint32_t *)(USER_DATA_RES_BASE_ADDR+2393344) //100x100
#define   ICON_MORE_ON_BIN                    (uint32_t *)(USER_DATA_RES_BASE_ADDR+2413376) //100x100
#define   ICON_MORE_TOUCH_BIN                 (uint32_t *)(USER_DATA_RES_BASE_ADDR+2433408) //100x100
#define   ICON_PHONE_OFF_BIN                  (uint32_t *)(USER_DATA_RES_BASE_ADDR+2453440) //100x100
#define   ICON_PHONE_ON_BIN                   (uint32_t *)(USER_DATA_RES_BASE_ADDR+2473472) //100x100
#define   ICON_PHONE_TOUCH_BIN                (uint32_t *)(USER_DATA_RES_BASE_ADDR+2493504) //100x100
#define   ICON_PROCESS_BAR_BIN                (uint32_t *)(USER_DATA_RES_BASE_ADDR+2513536) //288x8
#define   ICON_PROCESS_BAR_1_BIN              (uint32_t *)(USER_DATA_RES_BASE_ADDR+2518208) //14x8
#define   ICON_PROCESS_NODE_BIN               (uint32_t *)(USER_DATA_RES_BASE_ADDR+2518464) //32x32
#define   ICON_PROCESS_NODE_TOUCH_BIN         (uint32_t *)(USER_DATA_RES_BASE_ADDR+2520576) //32x32
#define   ICON_RECORD_DELETE_BIN              (uint32_t *)(USER_DATA_RES_BASE_ADDR+2522688) //277x81
#define   ICON_RECORD_DELETE_TOUCH_BIN        (uint32_t *)(USER_DATA_RES_BASE_ADDR+2567616) //277x81
#define   ICON_RECORD_PAUSE_BIN               (uint32_t *)(USER_DATA_RES_BASE_ADDR+2612544) //192x192
#define   ICON_RECORD_PAUSE_TOUCH_BIN         (uint32_t *)(USER_DATA_RES_BASE_ADDR+2686336) //192x192
#define   ICON_RECORD_PLAY_BIN                (uint32_t *)(USER_DATA_RES_BASE_ADDR+2760128) //192x192
#define   ICON_RECORD_PLAY_BAR_BIN            (uint32_t *)(USER_DATA_RES_BASE_ADDR+2833920) //240x16
#define   ICON_RECORD_PLAY_TOUCH_BIN          (uint32_t *)(USER_DATA_RES_BASE_ADDR+2841664) //192x192
#define   ICON_RECORD_START_BIN               (uint32_t *)(USER_DATA_RES_BASE_ADDR+2915456) //192x192
#define   ICON_RECORD_STOP_BIN                (uint32_t *)(USER_DATA_RES_BASE_ADDR+2989248) //192x192
#define   ICON_SEEKBAR_BIN                    (uint32_t *)(USER_DATA_RES_BASE_ADDR+3063040) //16x196
#define   ICON_SEEKBAR1_BIN                   (uint32_t *)(USER_DATA_RES_BASE_ADDR+3069376) //16x24
#define   ICON_SEEKBAR2_BIN                   (uint32_t *)(USER_DATA_RES_BASE_ADDR+3070208) //16x196
#define   ICON_SONG_BUDS_BIN                  (uint32_t *)(USER_DATA_RES_BASE_ADDR+3076544) //32x32
#define   ICON_SONG_LOCAL_BIN                 (uint32_t *)(USER_DATA_RES_BASE_ADDR+3078656) //32x32
#define   ICON_SONG_NEXT_BIN                  (uint32_t *)(USER_DATA_RES_BASE_ADDR+3080768) //57x57
#define   ICON_SONG_NEXT_TOUCH_BIN            (uint32_t *)(USER_DATA_RES_BASE_ADDR+3087296) //57x57
#define   ICON_SONG_PAUSE_BIN                 (uint32_t *)(USER_DATA_RES_BASE_ADDR+3093824) //64x64
#define   ICON_SONG_PAUSE_TOUCH_BIN           (uint32_t *)(USER_DATA_RES_BASE_ADDR+3102080) //64x64
#define   ICON_SONG_PHONE_BIN                 (uint32_t *)(USER_DATA_RES_BASE_ADDR+3110336) //32x32
#define   ICON_SONG_PLAY_BIN                  (uint32_t *)(USER_DATA_RES_BASE_ADDR+3112448) //64x64
#define   ICON_SONG_PLAY_TOUCH_BIN            (uint32_t *)(USER_DATA_RES_BASE_ADDR+3120704) //64x64
#define   ICON_SONG_PREV_BIN                  (uint32_t *)(USER_DATA_RES_BASE_ADDR+3128960) //57x56
#define   ICON_SONG_PREV_TOUCH_BIN            (uint32_t *)(USER_DATA_RES_BASE_ADDR+3135360) //57x56
#define   ICON_SONG_SPK_BIN                   (uint32_t *)(USER_DATA_RES_BASE_ADDR+3141760) //32x32
#define   ICON_SUCCESS_BIN                    (uint32_t *)(USER_DATA_RES_BASE_ADDR+3143872) //128x128
#define   ICON_SWITCH_OFF_BIN                 (uint32_t *)(USER_DATA_RES_BASE_ADDR+3176704) //100x60
#define   ICON_SWITCH_ON_BIN                  (uint32_t *)(USER_DATA_RES_BASE_ADDR+3188736) //100x60
#define   ICON_TEXT_BASE_BLUE_BIN             (uint32_t *)(USER_DATA_RES_BASE_ADDR+3200768) //288x64
#define   ICON_TEXT_BASE_DARK_BIN             (uint32_t *)(USER_DATA_RES_BASE_ADDR+3237696) //288x64
#define   ICON_TEXT_BASE_RED_BIN              (uint32_t *)(USER_DATA_RES_BASE_ADDR+3274624) //288x64
#define   ICON_TEXT_BASE_SHORT_BIN            (uint32_t *)(USER_DATA_RES_BASE_ADDR+3311552) //144x64
#define   ICON_TOUCH_BACKSPACE_BIN            (uint32_t *)(USER_DATA_RES_BASE_ADDR+3330048) //64x64
#define   ICON_TRASH_BIN                      (uint32_t *)(USER_DATA_RES_BASE_ADDR+3338304) //64x64
#define   ICON_TRASH_TOUCH_BIN                (uint32_t *)(USER_DATA_RES_BASE_ADDR+3346560) //64x64
#define   ICON_UP_SLIDE_BIN                   (uint32_t *)(USER_DATA_RES_BASE_ADDR+3354816) //278x81
#define   ICON_VOLUME_BIN                     (uint32_t *)(USER_DATA_RES_BASE_ADDR+3399872) //32x32
#define   ICON_VOLUME_BAR_BIN                 (uint32_t *)(USER_DATA_RES_BASE_ADDR+3401984) //240x8
#define   ICON_VOLUME_BAR_1_BIN               (uint32_t *)(USER_DATA_RES_BASE_ADDR+3405888) //24x8
#define   ICON_VOLUME_BAR_2_BIN               (uint32_t *)(USER_DATA_RES_BASE_ADDR+3406336) //240x8
#define   ICON_VOLUME_CALL_BIN                (uint32_t *)(USER_DATA_RES_BASE_ADDR+3410240) //80x80
#define   ICON_VOLUME_CALL_TOUCH_BIN          (uint32_t *)(USER_DATA_RES_BASE_ADDR+3423104) //80x80
#define   ICON_VOLUME_DOWN_BIN                (uint32_t *)(USER_DATA_RES_BASE_ADDR+3435968) //48x48
#define   ICON_VOLUME_DOWN_TOUCH_BIN          (uint32_t *)(USER_DATA_RES_BASE_ADDR+3440640) //48x48
#define   ICON_VOLUME_TOUCH_BIN               (uint32_t *)(USER_DATA_RES_BASE_ADDR+3445312) //48x48
#define   ICON_VOLUME_UP_BIN                  (uint32_t *)(USER_DATA_RES_BASE_ADDR+3449984) //48x48
#define   ICON_VOLUME_UP_TOUCH_BIN            (uint32_t *)(USER_DATA_RES_BASE_ADDR+3454656) //48x48
#define   OVAL_BIN                            (uint32_t *)(USER_DATA_RES_BASE_ADDR+3459328) //32x32
#define   RECORD_FILES_ICON_FOLDER_BIN        (uint32_t *)(USER_DATA_RES_BASE_ADDR+3461440) //24x24
#define   RECORD_PLAY_BAR1_BIN                (uint32_t *)(USER_DATA_RES_BASE_ADDR+3462656) //12x8
#define   RECORD_PLAY_BAR2_BIN                (uint32_t *)(USER_DATA_RES_BASE_ADDR+3462912) //240x8
#define   RECORD_PLAY_BAR_DOT_BIN             (uint32_t *)(USER_DATA_RES_BASE_ADDR+3466816) //16x16
#define   WATCH_BASE_BLACK_BIN                (uint32_t *)(USER_DATA_RES_BASE_ADDR+3467392) //454x454
#define   WATCH_BASE_BLACK_LIGHT_BIN          (uint32_t *)(USER_DATA_RES_BASE_ADDR+3879680) //454x454
#define   WATCH_BASE_GREY_BIN                 (uint32_t *)(USER_DATA_RES_BASE_ADDR+4291968) //454x454
#define   WATCH_BASE_GREY_70_ALPHA_BIN        (uint32_t *)(USER_DATA_RES_BASE_ADDR+4704256) //454x454
#define   WATCH_BASE_GREY_90ALPHA_HALF_BIN    (uint32_t *)(USER_DATA_RES_BASE_ADDR+5116544) //454x227
#define   WATCH_BASE_MASK_BLACK_BIN           (uint32_t *)(USER_DATA_RES_BASE_ADDR+5322688) //454x100

#endif
