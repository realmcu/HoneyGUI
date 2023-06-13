#ifndef __NES_APU_H
#define __NES_APU_H
#include "nes_main.h"


#define NES_APU_BUF_NUM         20
#define APU_SAMPLE_RATE         22050
#define APU_PCMBUF_SIZE         APU_SAMPLE_RATE/60      //44100/60=735      22050/60=367


#undef   NULL
#define  NULL 0
#ifndef  TRUE
#define  TRUE     1
#endif
#ifndef  FALSE
#define  FALSE    0
#endif

/* define this for realtime generated noise */
#define  REALTIME_NOISE

#define  APU_WRA0       0x4000
#define  APU_WRA1       0x4001
#define  APU_WRA2       0x4002
#define  APU_WRA3       0x4003
#define  APU_WRB0       0x4004
#define  APU_WRB1       0x4005
#define  APU_WRB2       0x4006
#define  APU_WRB3       0x4007
#define  APU_WRC0       0x4008
#define  APU_WRC2       0x400A
#define  APU_WRC3       0x400B
#define  APU_WRD0       0x400C
#define  APU_WRD2       0x400E
#define  APU_WRD3       0x400F
#define  APU_WRE0       0x4010
#define  APU_WRE1       0x4011
#define  APU_WRE2       0x4012
#define  APU_WRE3       0x4013

#define  APU_SMASK      0x4015

/* length of generated noise */
#define  APU_NOISE_32K  0x7FFF
#define  APU_NOISE_93   93

#define  APU_BASEFREQ   1789772.7272727272727272

/* to/from 16.16 fixed point */
#define  APU_TO_FIXED(x)    ((x) << 16)
#define  APU_FROM_FIXED(x)  ((x) >> 16)


/* channel structures */
/* As much data as possible is precalculated,
** to keep the sample processing as lean as possible
*/

typedef struct rectangle_s
{
    uint8_t regs[4];

    uint8_t enabled;

    int phaseacc;
    int freq;
    int output_vol;
    uint8_t fixed_envelope;
    uint8_t holdnote;
    uint8_t volume;

    int sweep_phase;
    int sweep_delay;
    uint8_t sweep_on;
    uint8_t sweep_shifts;
    uint8_t sweep_length;
    uint8_t sweep_inc;

    /* this may not be necessary in the future */
    int freq_limit;

    /* rectangle 0 uses a complement addition for sweep
    ** increases, while rectangle 1 uses subtraction
    */
    uint8_t sweep_complement;

    int env_phase;
    int env_delay;
    uint8_t env_vol;

    int vbl_length;
    uint8_t adder;
    int duty_flip;
} rectangle_t;

typedef struct triangle_s
{
    uint8_t regs[3];

    uint8_t enabled;

    int freq;
    int phaseacc;
    int output_vol;

    uint8_t adder;

    uint8_t holdnote;
    uint8_t counter_started;
    /* quasi-hack */
    int write_latency;

    int vbl_length;
    int linear_length;
} triangle_t;
typedef struct noise_s
{
    uint8_t regs[3];

    uint8_t enabled;

    int freq;
    int phaseacc;
    int output_vol;

    int env_phase;
    int env_delay;
    uint8_t env_vol;
    uint8_t fixed_envelope;
    uint8_t holdnote;

    uint8_t volume;

    int vbl_length;

#ifdef REALTIME_NOISE
    uint8_t xor_tap;
#else
    uint8_t short_sample;
    int cur_pos;
#endif /* REALTIME_NOISE */
} noise_t;

typedef struct dmc_s
{
    uint8_t regs[4];

    /* bodge for timestamp queue */
    uint8_t enabled;

    int freq;
    int phaseacc;
    int output_vol;

    uint32_t address;
    uint32_t cached_addr;
    int dma_length;
    int cached_dmalength;
    uint8_t cur_byte;

    uint8_t looping;
    uint8_t irq_gen;
    uint8_t irq_occurred;
} dmc_t;

enum
{
    APU_FILTER_NONE,
    APU_FILTER_LOWPASS,
    APU_FILTER_WEIGHTED
};

typedef struct
{
    //uint32_t min_range, max_range;
    signed long int min_range, max_range;
    uint8_t (*read_func)(uint32_t address);
} apu_memread;

typedef struct
{
    //uint32_t min_range, max_range;
    signed long int min_range, max_range;
    void (*write_func)(uint32_t address, uint8_t value);
} apu_memwrite;

/* external sound chip stuff */
typedef struct apuext_s
{
    void (*init)(void);
    void (*shutdown)(void);
    void (*reset)(void);
    //DCR
    void (*paramschanged)(void);
    int (*process)(void);
    apu_memread *mem_read;
    apu_memwrite *mem_write;
} apuext_t;

/* APU queue structure */
#define  APUQUEUE_SIZE  256  //4096
#define  APUQUEUE_MASK  (APUQUEUE_SIZE - 1)

/* apu ring buffer member */
typedef struct apudata_s
{
    uint32_t timestamp, address;
    uint8_t value;
} apudata_t;


typedef struct apu_s
{
    rectangle_t rectangle[2];
    triangle_t triangle;
    noise_t noise;
    dmc_t dmc;
    uint8_t enable_reg;

    apudata_t queue[APUQUEUE_SIZE];//APUQUEUE_SIZE
    int q_head, q_tail;
    uint32_t elapsed_cycles;

    void *buffer;
    int num_samples;

    uint8_t mix_enable[6];
    int filter_type;

    int cycle_rate;

    int sample_rate;
    int sample_bits;
    int refresh_rate;

    int decay_lut[16];
    int vbl_lut[32];
    int trilength_lut[128];
    apuext_t *ext;
} apu_t;

extern apu_t *apu;
extern uint16_t *wave_buffers;

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */
extern void apu_init(void);
extern void apu_setparams(int sample_rate, int refresh_rate, int frag_size, int sample_bits);
extern void apu_process(uint16_t *buffer, int num_samples);
extern void apu_reset(void);

extern uint8_t Apu_Read4015(uint32_t address);
extern void Apu_Write(uint8_t value, uint32_t address);

void apu_soundoutput(void);
void apu_write4017(uint32_t address, uint8_t value);

#define u8 uint8_t
#define u16 uint16_t
#define u32 uint32_t
#include "string.h"
#ifdef __cplusplus
}
#endif
#endif





