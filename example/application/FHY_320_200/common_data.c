/*============================================================================*
 *                        Header Files
 *============================================================================*/
#include "common_data.h"


/*============================================================================*
 *                           Types
 *============================================================================*/


/*============================================================================*
 *                            Macros
 *============================================================================*/


/*============================================================================*
 *                            Variables
 *============================================================================*/
/* Time */
struct tm *timeinfo = NULL;
struct tm tx_time =
{
    .tm_hour = 11,
    .tm_min = 20,
    .tm_sec = 0,
    .tm_mday = 15,
    .tm_mon = 10,
    .tm_year = 2025,
    .tm_wday = 3,
};
bool time_format_24 = false; // default 24H format

/* Date */
const char *month[13][12] =
{
    /* 0: English */
    {"Jan",      "Feb",      "Mar",      "Apr",      "May",      "Jun",      "Jul",      "Aug",      "Sep",      "Oct",      "Nov",           "Dec"          },
    /* 1: French */
    {"Jan",      "Fév",      "Mar",      "Avr",      "Mai",      "Jun",      "Jui",      "Aoû",      "Sep",      "Oct",      "Nov",           "Déc"          },
    /* 2: German */
    {"Jan",      "Feb",      "Mär",      "Apr",      "Mai",      "Jun",      "Jul",      "Aug",      "Sep",      "Okt",      "Nov",           "Dez"          },
    /* 3: Spanish */
    {"Ene",      "Feb",      "Marzo",    "Abr",      "Mayo",     "Jun",      "Jul",      "Ago",      "Sep",      "Oct",      "Nov",           "Dic"          },
    /* 4: Italian */
    {"Gen",      "Feb",      "Mar",      "Apr",      "Mag",      "Giu",      "Lug",      "Ago",      "Set",      "Ott",      "Nov",           "Dic"          },
    /* 5: Indonesian */
    {"Jan",      "Feb",      "Mar",      "Apr",      "Mei",      "Jun",      "Jul",      "Aug",      "Sep",      "Oct",      "Nov",           "Dec"          },
    /* 6: Portuguese */
    {"Jan",      "Fev",      "Mar",      "Abr",      "Mai",      "Jun",      "Jul",      "Ago",      "Set",      "Out",      "Nov",           "Dez"          },
    /* 7: Russian */
    {"Янв",      "Февр",     "Март",     "Апр",      "Май",      "Июнь",     "Июль",     "Авг",      "Сент",     "Окт",      "Ноя",           "Дек"          },
    /* 8: Simplified Chinese */
    {"一月",     "二月",     "三月",     "四月",     "五月",     "六月",     "七月",     "八月",     "九月",     "十月",     "十一月",        "十二月"        },
    /* 9: Japanese */
    {"1月",      "2月",      "3月",      "4月",      "5月",      "6月",      "7月",      "8月",      "9月",      "10月",     "11月",          "12月"         },
    /* 10: Korean */
    {"1월",      "2월",      "3월",      "4월",      "5월",      "6월",      "7월",      "8월",      "9월",      "10월",     "11월",          "12월"         },
    /* 11: Thai */
    {"ม.ค.",     "ก.พ.",     "มี.ค.",    "เม.ย.",    "พ.ค.",     "มิ.ย.",    "ก.ค.",     "ส.ค.",     "ก.ย.",     "ต.ค.",     "พ.ย.",          "ธ.ค."         },
    /* 12: Vietnamese */
    {"Tháng Một", "Tháng Hai", "Tháng Ba",  "Tháng Tư", "Tháng Năm", "Tháng Sáu", "Tháng Bảy", "Tháng Tám", "Tháng Chín", "Tháng Mười", "Tháng Mười Một", "Tháng Mười Hai"}
};

const char *day[13][7] =
{
    /* 0: English */
    {"Mon",     "Tue",     "Wed",     "Thu",     "Fri",     "Sat",      "Sun"     },
    /* 1: French */
    {"Lun",     "Mar",     "Mer",     "Jeu",     "Ven",     "Sam",      "Dim"     },
    /* 2: German */
    {"Mo",      "Di",      "Mi",      "Do",      "Fr",      "Sa",       "So"      },
    /* 3: Spanish */
    {"Lun",     "Mar",     "Mié",     "Jue",     "Vie",     "Sáb",      "Dom"     },
    /* 4: Italian */
    {"Lun",     "Mar",     "Mer",     "Gio",     "Ven",     "Sab",      "Dom"     },
    /* 5: Dutch */
    {"Ma",      "Di",      "Wo",      "Do",      "Vr",      "Za",       "Zo"      },
    /* 6: Portuguese */
    {"Seg",     "Ter",     "Qua",     "Qui",     "Sex",     "Sáb",      "Dom"     },
    /* 7: Russian */
    {"Пн",      "Вт",      "Ср",      "Чт",      "Пт",      "Сб",       "Вс"      },
    /* 8: Simplified Chinese */
    {"周一",    "周二",    "周三",    "周四",    "周五",    "周六",     "周日"    },
    /* 9: Japanese */
    {"月曜",    "火曜",    "水曜",    "木曜",    "金曜",    "土曜",     "日曜"    },
    /* 10: Korean */
    {"월",      "화",      "수",      "목",      "금",      "토",       "일"      },
    /* 11: Thai */
    {"จ.",      "อ.",      "พ.",      "พฤ.",     "ศ.",      "ส.",       "อา."     },
    /* 12: Vietnamese */
    {"Thứ Hai", "Thứ Ba",  "Thứ Tư",  "Thứ Năm", "Thứ Sáu", "Thứ Bảy",  "Chủ Nhật"}
};

/* Page name */
const char *page_name_array[13][35] =
{
    {"playback",        "Volume",               "Ambient Sound",      "Equalizer",              "Smart Talk",    "Spatial Sound",                 "High-Resolution Audio (LDAC)", "VoiceAware",          "Auto Play/Pause",                        "Audio Source", "Auracast Broadcast", "Timer",        "Flashlight",       "SilentNow", "Volume Unit Meter", "JBL Headphones App",         "Smart Tx Management", "Screen Brightness",              "Dark/Light Mode",         "Lock Screen",           "Auto Dim Off Screen",                       "Quick Wake Up Screen", "Case Button Customize", "Information Center Customize", "Unlock Slider",                   "Notification",     "Clock Settings", "Time Format",          "language", "Reorder Quick Access",               "Support",       "Product Tips",               "Factory Reset",                             "Password", "Message Preview"},
    {"Lecture",         "Volume",               "Son d'ambiance",     "Égaliseur",              "Smart Talk",    "Son Spatial",                   "High-Resolution Audio (LDAC)", "VoiceAware",          "Lecture et pause auto",                  "Audio Source", "Auracast Broadcast", "Minuteur",     "Lampe torche",     "SilentNow", "Volume Unit Meter", "Application JBL Headphones", "Smart Tx Management", "Luminosité de l'écran",          "Mode sombre/clair",       "Écran de verrouillage", "Écran arrêt atténuation auto",              "Quick Wake Up Screen", "Case Button Customize", "Information Center Customize", "Curseur de déverrouillage",       "Notification",     "Clock Settings", "Format de l'heure",    "Langue",   "Réorganiser l'accès rapide",         "Assistance",    "Conseils sur le produit",    "Réinitialisation d'usine",                  "Password", "Aperçu du message"},
    {"Wiedergabe",      "Lautstärke",           "Umgebungsgeräusche", "Equalizer",              "Smart Talk",    "Raumsound",                     "High-Resolution Audio (LDAC)", "VoiceAware",          "Autom. Wiedergabe und Pause",            "Audio Source", "Auracast Broadcast", "Timer",        "Taschenlampe",     "SilentNow", "Volume Unit Meter", "JBL Headphones-App",         "Smart Tx Management", "Bildschirmhelligkeit",           "Dunkel-/Hell-Modus",      "Sperrbildschirm",       "Automatische Bildschirmabdunkelung",        "Quick Wake Up Screen", "Case Button Customize", "Information Center Customize", "Entsperrschieber",                "Benachrichtigung", "Clock Settings", "Uhrzeitformat",        "Sprache",  "Schnellzugriff neu anordnen",        "Unterstützung", "Tipps zum Gerät",            "Reset auf die Werkseinstellungen",          "Password", "Vorschau für Nachricht"},
    {"Reproducción",    "Volumen",              "Sonido ambiental",   "Ecualizador",            "Smart Talk",    "Sonido espacial",               "High-Resolution Audio (LDAC)", "VoiceAware",          "Reproducción/pausa automática",          "Audio Source", "Auracast Broadcast", "Temporizador", "Linterna",         "SilentNow", "Volume Unit Meter", "Aplicación JBL Headphones",  "Smart Tx Management", "BildschirBrillo de la pantalla", "Modo oscuro/claro",       "Pantalla de bloqueo",   "Atenuación automática de la pantalla",      "Quick Wake Up Screen", "Case Button Customize", "Information Center Customize", "Deslizador de desbloqueo",        "Notificación",     "Clock Settings", "Formato de hora",      "Idioma",   "Reordenar acceso rápido",            "Asistencia",    "Consejos sobre el producto", "Restablecer a la configuración de fábrica", "Password", "Vista previa del mensaje"},
    {"Riproduzione",    "Volume",               "Suono ambientale",   "Equalizzatore",          "Smart Talk",    "Suono Spaziale",                "High-Resolution Audio (LDAC)", "VoiceAware",          "Play & Pausa automatici",                "Audio Source", "Auracast Broadcast", "Timer",        "Torcia",           "SilentNow", "Volume Unit Meter", "App JBL Headphones",         "Smart Tx Management", "Luminosità schermo",             "Modalità scura/luminosa", "Schermata di blocco",   "Spegnimento graduale automatico schermo",   "Quick Wake Up Screen", "Case Button Customize", "Information Center Customize", "Cursore di sblocco",              "Notifiche",        "Clock Settings", "Formato ora",          "Lingua",   "Riordina Quick Access",              "Supporto",      "Suggerimenti sul prodotto",  "Ripristino delle impostazioni predefinite", "Password", "Anteprima dei messaggi"},
    {"Afspelen",        "Volume",               "Omgevingsgeluid",    "Equalizer",              "Smart Talk",    "Ruimtelijk geluid",             "High-Resolution Audio (LDAC)", "VoiceAware",          "Automatisch afspelen en pauzeren",       "Audio Source", "Auracast Broadcast", "Timer",        "Zaklamp",          "SilentNow", "Volume Unit Meter", "JBL Headphones-app",         "Smart Tx Management", "Schermhelderheid",               "Donker/lichte modus",     "Vergrendelscherm",      "Scherm automatisch dimmen uit",             "Quick Wake Up Screen", "Case Button Customize", "Information Center Customize", "Schuif ontgrendelen",             "Melding",          "Clock Settings", "Tijdnotatie",          "Taal",     "Quick Access herordenen",            "Ondersteuning", "Producttips",                "Fabrieksreset",                             "Password", "Berichtvoorbeeld"},
    {"Reprodução",      "Volume",               "Som Ambiente",       "Equalizador",            "Smart Talk",    "Som espacial",                  "High-Resolution Audio (LDAC)", "VoiceAware",          "Reproduzir e pausa automáticos",         "Audio Source", "Auracast Broadcast", "Temporizador", "Lanterna",         "SilentNow", "Volume Unit Meter", "Aplicação JBL Headphones",   "Smart Tx Management", "Luminosidade do Ecrã",           "Modo Escuro/Claro",       "Ecrã de Bloqueio",      "Desligar Escurecimento Automático do Ecrã", "Quick Wake Up Screen", "Case Button Customize", "Information Center Customize", "Desbloquear Controlo Deslizante", "Notificação",      "Clock Settings", "Formato da Hora",      "Idioma",   "Reordenar Acesso Rápido",            "Suporte",       "Sugestões do Produto",       "Redefinir de fábrica",                      "Password", "Pré-visualização da mensagem"},
    {"Воспроизведение", "Громкость",            "Фоновый звук",       "Эквалайзер",             "Smart Talk",    "Объемное Звучание",             "High-Resolution Audio (LDAC)", "Функция VoiceAware",  "Автоматическое воспроизведение и пауза", "Audio Source", "Auracast Broadcast", "Таймер",       "Фонарик",          "SilentNow", "Volume Unit Meter", "Приложение JBL Headphones",  "Smart Tx Management", "Яркость экрана",                 "Темный/светлый режим",    "Экран блокировки",      "Авто-затемнение экрана выключено",          "Quick Wake Up Screen", "Case Button Customize", "Information Center Customize", "Ползунок разблокировки",          "Уведомление",      "Clock Settings", "Формат времени",       "Язык",     "Изменение порядка быстрого доступа", "Поддержка",     "Советы по устройству",       "Сброс до заводских настроек",               "Password", "Предварительный просмотр сообщения"},
    {"回放",            "音量",                 "环境音",               "均衡器",                 "智能通话",       "空间音频",                      "High-Resolution Audio (LDAC)", "语音感知",             "自动播放和暂停",                          "Audio Source", "Auracast Broadcast", "定时器",        "手电筒",           "SilentNow", "Volume Unit Meter", "JBL Headphones app",         "Smart Tx Management", "屏幕亮度",                        "深色/浅色模式",            "锁屏",                   "自动熄屏调暗",                               "Quick Wake Up Screen", "Case Button Customize", "Information Center Customize", "解锁滑块",                        "通知",              "Clock Settings", "时间格式",             "语言",      "重新排序快速访问",                    "支持",           "产品小贴士",                  "恢复出厂设置",                               "Password", "消息预览"},
    {"再生",            "音量",                 "アンビエントサウンド",  "イコライザー",            "スマートトーク", "空間サウンド",                   "High-Resolution Audio (LDAC)", "ボイスアウェア",       "自動再生&一時停止",                        "Audio Source", "Auracast Broadcast", "タイマー",      "フラッシュライト", "SilentNow", "Volume Unit Meter", "JBL Headphonesアプリ",        "Smart Tx Management", "画面の輝度",                      "ダーク/ライトモード",       "ロック画面",             "自動ディムオフ画面",                          "Quick Wake Up Screen", "Case Button Customize", "Information Center Customize", "スライダーをアンロック",           "通知",              "Clock Settings", "時間のフォーマット",    "言語",      "クイックアクセスの並び替え",           "サポート",       "製品のヒント",                 "初期化",                                    "Password", "メッセージのプレビュー"},
    {"재생",            "볼륨",                 "주변 소리",            "이퀄라이저",              "Smart Talk",   "공간 음향",                      "High-Resolution Audio (LDAC)", "VoiceAware",          "자동 재생 및 일시 중지",                   "Audio Source", "Auracast Broadcast", "타이머",       "플래시",           "SilentNow", "Volume Unit Meter", "JBL Headphones 앱",           "Smart Tx Management", "화면 밝기",                       "다크/라이트 모드",         "잠금 화면",              "화면 자동 어둡게 꺼짐",                       "Quick Wake Up Screen", "Case Button Customize", "Information Center Customize", "잠금 해제 슬라이더",               "알림",              "Clock Settings", "시간 형식",            "언어",     "빠른 접근 재정렬",                    "지원",           "제품 팁",                     "공장 초기화",                                "Password", "메시지 미리 보기"},
    {"การเล่น",          "เสียงรอบข้าง",           "Ambient Sound",       "อีควอไลเซอร์",             "Smart Talk",    "เสียงรอบทิศทาง",                  "High-Resolution Audio (LDAC)", "การรับรู้เสียงพูด",        "เล่นและหยุดอัตโนมัติ",                        "Audio Source", "Auracast Broadcast", "ตัวจับเวลา",     "ไฟฉาย",           "SilentNow", "Volume Unit Meter", "แอป JBL Headphones",         "Smart Tx Management", "ความสว่างหน้าจอ",                  "โหมดมืด/สว่าง",             "ล็อกหน้าจอ",              "หน้าจอมืดอัตโนมัติ",                            "Quick Wake Up Screen", "Case Button Customize", "Information Center Customize", "แถบเลื่อนปลดล็อก",                 "การแจ้งเตือน",         "Clock Settings", "รูปแบบเวลา",           "ภาษา",     "จัดลำดับการเข้าถึงด่วนใหม่",               "การสนับสนุน",     "เคล็ดลับผลิตภัณฑ์",             "รีเซ็ตเป็นค่าจากโรงงาน",                         "Password", "ตัวอย่างข้อความ"},
    {"Phát lại",        "Âm thanh môi trường", "Ambient Sound",       "Bộ điều chỉnh âm thanh", "Smart Talk",    "Tính năng âm thanh không gian", "High-Resolution Audio (LDAC)", "Phát hiện giọng nói", "Tự động phát & tạm dừng",                "Audio Source", "Auracast Broadcast", "Bộ đếm giờ",   "Đèn pin",         "SilentNow", "Volume Unit Meter", "Ứng dụng JBL Headphones",    "Smart Tx Management", "Độ sáng màn hình",                "Chế độ Tối/Sáng",         "Màn hình khóa",         "Tự động làm mờ khi tắt màn hình",           "Quick Wake Up Screen", "Case Button Customize", "Information Center Customize", "Thanh trượt mở khóa",             "Thông báo",         "Clock Settings", "Định dạng thời gian", "Ngôn ngữ", "Sắp xếp lại Truy cập nhanh",         "Hỗ trợ",        "Mẹo sử dụng sản phẩm",       "Khôi phục cài đặt gốc",                     "Password", "Xem trước tin nhắn"},
};


/* Lock screen */
int8_t wallpaper_index = 0; // [0, 4]
bool switch_from_lock_screen = false;

/* Theme color */
uint8_t theme_color_index = 0;
uint32_t theme_color_array[5] =
{
    0xFFFF5901,
    0xFFFF9420,
    0xFF8FBF80,
    0xFF7C9EFF,
    0xFFB490FF,
};

/* Theme bg */
bool theme_bg_white = false; // default black bg

/* Playback */
int8_t music_index = 0;
char music_array[MUSIC_MAX_NUM][20] =
{
    "Watermelon Sugar",
    "Save Your Tears",
    "Feels",
    "Stay",
    "Blinding Lights",
    "Levitating",
    "Peaches",
    "Montero",
    "Good 4 U",
    "Kiss Me More",
    "Industry Baby",
    "Heat Waves",
};
char lyrics_str[200] =
{
    "Never gonna give you up \
 Never gonna let you down \
 Never gonna run around and desert you \
 Never gonna make you cry \
 Never gonna say goodbye \
 Never gonna tell a lie and hurt you"
};

/* Volume */
int8_t volume_val = 8; // [0, 16]

/* Battery */
uint8_t battery_tx_val = 30; //[0,100]
uint8_t battery_headband_val = 10;

/* Message */
uint8_t message_num_val = 9;

/* Function status */
struct FUNCTION_STATUS f_status = {0};

/* Screen Brightness */
int8_t screen_brightness_val = 1; // [1, 3]

/* Smart talk time */
uint8_t smart_talk_time_type = 0; // 0: 5s, 1: 15s, 2: 20s

/* Ambient sound type */
AMBIENT_SOUND_TYPE ambient_sound_type = ANC;

/* Spatial sound type */
SPATIAL_SOUND_TYPE spatial_sound_type = SPATIAL_SOUND_MOVIE;
#if SS_WITH_HEAD_TRACKING
SPATIAL_SOUND_STATUS spatial_sound_status = SPATIAL_SOUND_OFF;
#endif

/* Voice aware */
int8_t voice_aware_val = 2; // [1, 3]

/* Codec type index */
uint8_t codec_type_index = AAC; // [0, 5]

/* Timer */
int8_t timer_max_min_val = 5; // minutes [1, 60]
uint16_t timer_val = 300; // seconds
char timer_str[6] = {0};

/* Call */
uint16_t calltime_val = 0; // seconds
char calltime_str[6] = {0};
char call_source_str[20] = "+8618888888888";
gui_call_t *gui_call = NULL;

/* Auto dim off screen*/
int8_t auto_dim_time_val = 30; // [5, 60], step 5

/* Quick wake up screen*/
int8_t quick_wake_up_click_num = 0; // [0, 2]

/* Case button customize*/
uint8_t case_button_customize_index_array[3] = {0};
uint8_t case_button_customize_type_index = 0; // [0, 2] single, double, long
const char *case_button_customize_type[3] =
{
    "Single press",
    "Double press",
    "Long press 5s"
};

/* Language */
int8_t language_index = ENGLISH;

/* Reorder quick access */
const char *quick_page_name[4] = {0};
uint8_t quick_page_name_index = 0;

/* Information center customize */
int8_t info_center_func_cnt = 3;
const char *info_center_func_name[6] =
{
    "Play/Pause",
    "Ambient\nSound",
    "EQ",
    "Spatial\nSound",
    "Flash light",
    "Dark/Light\nMode"
};

/* Audio source */
struct AUDIO_SOURCE audio_source =
{
    .auracast_num = 0,
    .connected_num = 1,
    .streaming_index = 1,
    .connected_source_is_cast = {0, 0},
    .auracast_hq = {0, 0},
    .paired_num = 3,
    .auracast_receiver = 0,
    .connected_name = {"device 1"},
    .paired_name = {"device 2", "device 3", "device 4"},
};

/* OTA */
uint8_t ota_val = 0; // [0, 100]
char ota_str[4] = {0};

/* Auracast */
char auracast_password[5] = "1234";

/* Volume unit meter */
uint8_t vu_strength_l = 6; // [0, 13]
uint8_t vu_strength_r = 13; // [0, 13]

/* Silentnow */
uint16_t silent_time_val = 900; // seconds

/* Smart tx management */
struct TX_MANAGEMENT tx_management =
{
    .connected = true,
    .connected_name = "Headphones 1",
    .paired_num = 3,
    .paired_name = {"Headphones 2", "Headphones 3", "Headphones 4"},
};

/* Clock settings */
uint8_t hour_val = 0; //[0, 23]
uint8_t minutes_val = 0; //[0, 59]
char hour_str[3] = {0};
char minutes_str[3] = {0};

/* Notification */
struct NOTIFICATION_CONTENT notification_content[NOTIFICATION_MAX_NUM] =
{
    {"Wechat", "Avocado: Hi how's everything going? I'm planing to visit our grandpa next week."},
    {"Line", "Orange: Yo sup bro how's everything going? I'm planing to visit our grandpa next week."},
    {"QQ", "Banana: Yo sup bro how's everything going? I'm planing to visit our grandpa next week."},
    {"Wechat", "Avocado: Hi how's everything going? I'm planing to visit our grandpa next week."},
    {"Line", "Orange: Yo sup bro how's everything going? I'm planing to visit our grandpa next week."},
    {"QQ", "Banana: Yo sup bro how's everything going? I'm planing to visit our grandpa next week."},
    {"Wechat", "Avocado: Hi how's everything going? I'm planing to visit our grandpa next week."},
    {"Line", "Orange: Yo sup bro how's everything going? I'm planing to visit our grandpa next week."},
    {"QQ", "Banana: Yo sup bro how's everything going? I'm planing to visit our grandpa next week."},
};

void gui_call_register(gui_call_t *call)
{
    gui_call = call;
}
