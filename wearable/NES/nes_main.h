#ifndef __NES_MAIN_H
#define __NES_MAIN_H

#include "stdint.h"
#include "string.h"


#define INLINE  static inline
#define int8    char
#define int16   short
#define int32   int
#define uint8   unsigned char
#define uint16  unsigned short
#define uint32  unsigned int
#define boolean uint8

#define u8 uint8_t
#define u16 uint16_t
#define u32 uint32_t

#define CPU_NMI  cpunmi=1;
#define CPU_IRQ  cpuirq=1;
#define NES_RAM_SPEED  0
#define NES_SKIP_FRAME  2




typedef struct
{
    unsigned char id[3];
    unsigned char ctrl_z;
    unsigned char num_16k_rom_banks;
    unsigned char num_8k_vrom_banks;
    unsigned char flags_1;
    unsigned char flags_2;
    unsigned char reserved[8];
} NES_header;

extern uint8_t nes_frame_cnt;
extern int MapperNo;
extern int NES_scanline;
extern NES_header *RomHeader;
extern int VROM_1K_SIZE;
extern int VROM_8K_SIZE;
extern uint8_t cpunmi;
extern uint8_t cpuirq;
extern uint8_t PADdata;
extern uint8_t PADdata1;
extern uint8_t lianan_biao;



void cpu6502_init(void);
void run6502(uint32_t);
uint8_t nes_load_rom(void);
void nes_sram_free(void);
uint8_t nes_sram_malloc(uint32_t romsize);
void nes_set_window(void);
void nes_get_gamepadval(void);
void nes_emulate_frame(void);
void debug_6502(uint16_t reg0, uint8_t reg1);

void nes_i2s_dma_tx_callback(void);
int nes_sound_open(int samples_per_sync, int sample_rate);
void nes_sound_close(void);
void nes_apu_fill_buffer(int samples, uint16_t *wavebuf);

uint32_t get_crc32(uint8_t *buf, uint32_t len);
#endif


