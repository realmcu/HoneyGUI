
// GSensor Algorithm
#ifndef RTK_GSA_H
#define RTK_GSA_H

#ifdef __cplusplus
extern "C" {
#endif

/*************************************
***    Typedefs, Structs
*************************************/
typedef unsigned int    uint32;
typedef signed int      int32;
typedef unsigned short  uint16;
typedef signed short    int16;
typedef unsigned char   uint8;
typedef signed char     int8;
#ifndef bool
typedef unsigned char   bool;
#endif

typedef struct
{
    uint8   major;
    uint8   minor;
    uint16  revision;
} gsa_ver_t;

typedef struct
{
    uint32  reserve : 5;
    uint32  weight  : 10;   // Q1, kg
    uint32  height  : 9;    // Q1, cm
    uint32  age     : 7;
    uint32  gender  : 1;    // 0-female, 1-male
} usr_prof_t;

typedef enum
{
    GS_XPOS_RIGHT   = 0,
    GS_XPOS_DOWN,
    GS_XPOS_LEFT,
    GS_XPOS_UP
} gs_xpos_e;

typedef enum
{
    GS_ZPOS_UP      = 0,
    GS_ZPOS_DOWN
} gs_zpos_e;

typedef struct
{
    gs_xpos_e   xpos;
    gs_zpos_e   zpos;
    uint32      odr;    // Q10
} gsa_gs_inf_t;

typedef enum
{
    SPORT_MODE_WALK,
    SPORT_MODE_RUN,
    SPORT_MODE_INVALID
} gsa_sport_mode_e;

typedef struct
{
    gsa_sport_mode_e    mode;
    uint16  steps;
    uint16  distance;   // Q4, cm
    uint16  speed;      // Q8, km/h
    uint16  frequency;  // Q6, steps/m
    uint16  calories;   // Q2, cal
} gsa_pedo_info_t;

typedef enum
{
    GSA_SLEEP_OFF   = 0,
    GSA_SLEEP_DEEP,
    GSA_SLEEP_SLEEP,
    GSA_SLEEP_WAKE
} gsa_sleep_stt_e;

typedef enum
{
    GSA_ACT_LIFT    = 0,
    GSA_ACT_TWIST,
    GSA_ACT_WAVE
} gsa_act_type_e;

typedef void (*gsa_pedo_cb)(gsa_pedo_info_t *);
typedef void (*gsa_sleep_cb)(gsa_sleep_stt_e, uint32);
typedef void (*gsa_act_cb)(gsa_act_type_e, uint16);
typedef bool (*gsa_wear_chk)(void);
typedef void (*gsa_sleep_stat)(uint16);

typedef struct
{
    gsa_pedo_cb     pedo_cb;
    gsa_sleep_cb    sleep_cb;
    gsa_act_cb      act_cb;
    gsa_wear_chk    wear_chk;   // NULL if not supported
    gsa_sleep_stat  sleep_stat; // For debug, act statistics per minute
} gsa_cbs_t;


/*************************************
***    Public Functions Declaration
*************************************/
gsa_ver_t *rtk_gsa_get_ver(void);
void rtk_gsa_init(usr_prof_t *prof, gsa_gs_inf_t *gs_inf, gsa_cbs_t *cbs);
void rtk_gsa_fsm(int16 *gsa_accs);
void rtk_gsa_sleep_wakeup(void);
void rtk_gsa_motor_state(bool is_on);
void rtk_gsa_act_switch(gsa_act_type_e type, bool turn_on);

#ifdef __cplusplus
}
#endif

#endif

