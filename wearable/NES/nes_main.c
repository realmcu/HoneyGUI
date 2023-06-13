#include "nes_main.h"
#include "nes_ppu.h"
#include "nes_mapper.h"
#include "nes_apu.h"
#include "rtthread.h"
#include "ROM_GAME.h"

extern uint8_t frame;
uint8_t nes_frame_cnt;
int MapperNo;
int NES_scanline;
int VROM_1K_SIZE;
int VROM_8K_SIZE;
u32 NESrom_crc32;

uint8_t PADdata0 =
    0xff;            //PAD 1 VALUE [7:0], right->7, left->6, down->5, up->4, start->3, select->2, B->1, A->0
uint8_t PADdata1 =
    0xff;            //PAD 1 VALUE [7:0], right->7, left->6, down->5, up->4, start->3, select->2, B->1, A->0
uint8_t *NES_RAM;
uint8_t *NES_SRAM;
NES_header *RomHeader;
MAPPER *NES_Mapper;
MapperCommRes *MAPx;


uint8_t *spr_ram;
ppu_data *ppu;
uint8_t *VROM_banks;
uint8_t *VROM_tiles;

apu_t *apu;
u16 *wave_buffers;
u16 *i2sbuf1;
u16 *i2sbuf2;

u16 num = 0;
uint8_t *romfile = (uint8_t *)rom_game;


uint8_t system_task_return = 0;


uint8_t nes_sram_malloc(u32 romsize)
{
    NES_RAM = rt_malloc(1024 * 3); //if ((u32)NES_RAM % 1024)
    NES_RAM = NES_RAM + 1024 - (int)NES_RAM % 1024;
    rt_kprintf("NES_RAM:0x%x;\n", NES_RAM);
    NES_SRAM = rt_malloc(0X2000);
    rt_kprintf("NES_SRAM:0x%x\n", NES_SRAM);
    RomHeader = rt_malloc(sizeof(NES_header));
    rt_kprintf("RomHeader:0x%x\n", RomHeader);
    NES_Mapper = rt_malloc(sizeof(MAPPER));
    rt_kprintf("NES_Mapper:0x%x\n", NES_Mapper);
    spr_ram = rt_malloc(0X100);
    rt_kprintf("spr_ram:0x%x\n", spr_ram);
    ppu = rt_malloc(sizeof(ppu_data));
    rt_kprintf("ppu:0x%x\n", ppu);
    apu = rt_malloc(sizeof(apu_t));
    rt_kprintf("apu:0x%x\n", apu);
    wave_buffers = rt_malloc(APU_PCMBUF_SIZE * 2);
    rt_kprintf("wave_buffers:0x%x\n", wave_buffers);
    i2sbuf1 = rt_malloc(APU_PCMBUF_SIZE * 2);
    rt_kprintf("i2sbuf1:0x%x\n", i2sbuf1);
    i2sbuf2 = rt_malloc(APU_PCMBUF_SIZE * 2);
    rt_kprintf("i2sbuf2:0x%x\n", i2sbuf2);


    if (!NES_RAM || !NES_SRAM || !RomHeader || !NES_Mapper || !spr_ram || !ppu || !apu ||
        !wave_buffers || !i2sbuf1 || !i2sbuf2 || !romfile)
    {
        //nes_sram_free();
        rt_kprintf("[NES]MALLOC FAIL!!\n");
        return 1;
    }
    memset(NES_SRAM, 0, 0X2000);
    memset(RomHeader, 0, sizeof(NES_header));
    memset(NES_Mapper, 0, sizeof(MAPPER));
    memset(spr_ram, 0, 0X100);
    memset(ppu, 0, sizeof(ppu_data));
    memset(apu, 0, sizeof(apu_t));
    memset(wave_buffers, 0, APU_PCMBUF_SIZE * 2);
    memset(i2sbuf1, 0, APU_PCMBUF_SIZE * 2);
    memset(i2sbuf2, 0, APU_PCMBUF_SIZE * 2);
    return 0;
}

uint8_t nes_load_rom(void)
{
    uint8_t *p;
    uint8_t i;
    uint8_t res = 0;
    p = (uint8_t *)romfile;
    if (strncmp((char *)p, "NES", 3) == 0)
    {
        RomHeader->ctrl_z = p[3];
        RomHeader->num_16k_rom_banks = p[4];
        RomHeader->num_8k_vrom_banks = p[5];
        RomHeader->flags_1 = p[6];
        RomHeader->flags_2 = p[7];
        if (RomHeader->flags_1 & 0x04)
        {
            p += 512;
        }
        if (RomHeader->num_8k_vrom_banks > 0)
        {
            VROM_banks = p + 16 + (RomHeader->num_16k_rom_banks * 0x4000);
#if NES_RAM_SPEED==1
            VROM_tiles = VROM_banks;
#else
            VROM_tiles = rt_malloc(RomHeader->num_8k_vrom_banks * 8 * 1024);
            if (VROM_tiles == 0)
            {
                VROM_tiles = VROM_banks;
            }
            compile(RomHeader->num_8k_vrom_banks * 8 * 1024 / 16, VROM_banks, VROM_tiles);
#endif
        }
        else
        {
            VROM_banks = rt_malloc(8 * 1024);
            VROM_tiles = rt_malloc(8 * 1024);
            if (!VROM_banks || !VROM_tiles)
            {
                res = 1;
            }
        }
        VROM_1K_SIZE = RomHeader->num_8k_vrom_banks * 8;
        VROM_8K_SIZE = RomHeader->num_8k_vrom_banks;
        MapperNo = (RomHeader->flags_1 >> 4) | (RomHeader->flags_2 & 0xf0);
        if (RomHeader->flags_2 & 0x0E)
        {
            MapperNo = RomHeader->flags_1 >> 4;
        }
        rt_kprintf("use map:%d\r\n", MapperNo);
        for (i = 0; i < 255; i++)
        {
            if (MapTab[i] == MapperNo)
            {
                break;
            }
            if (MapTab[i] == -1)
            {
                res = 3;
            }
        }
        if (res == 0)
        {
            switch (MapperNo)
            {
            case 1:
                MAP1 = rt_malloc(sizeof(Mapper1Res));
                if (!MAP1) { res = 1; }
                break;
            case 4:
            case 6:
            case 16:
            case 17:
            case 18:
            case 19:
            case 21:
            case 23:
            case 24:
            case 25:
            case 64:
            case 65:
            case 67:
            case 69:
            case 85:
            case 189:
                MAPx = rt_malloc(sizeof(MapperCommRes));
                if (!MAPx) { res = 1; }
                break;
            default:
                break;
            }
        }
    }
    return res;
}

void nes_sram_free(void)
{

}



#include "rtthread.h"
#include "platform_utils.h"
#include "lcd_st7789_spi.h"

uint8_t nes_xoff = 0;
void nes_set_window(void)
{
    u16 xoff = 0, yoff = 0;
    u16 lcdwidth = 240, lcdheight = 240;

    {
        lcdwidth = 240;
        lcdheight = 240;
        nes_xoff = (256 - 240) / 2;
        xoff = 0;
    }

    lcd_st7789_set_window(xoff, yoff, xoff + lcdwidth - 1, yoff + lcdheight - 1);
}


void nes_get_gamepadval(void)
{
    PADdata0 = 0;
    PADdata1 = 0;
}


void scanline_draw(int LineNo)
{
    uint16 i;
    uint16 sx, ex;
    do_scanline_and_draw(ppu->dummy_buffer);
    sx = nes_xoff + 8;
    ex = 256 + 8 - nes_xoff;

    uint16_t tmp;
    uint16_t line[256];

    for (i = sx; i < ex; i++)
    {
        tmp = NES_Palette[ppu->dummy_buffer[i]];

        line[i] = WAP16(tmp);
    }
    lcd_scanline((uint8_t *)(line + sx), (ex - sx) * 2);


}

void debug_6502(u16 reg0, uint8_t reg1)
{
    //rt_kprintf("6502 error:%x,%d\r\n",reg0,reg1);
}
void nes_apu_fill_buffer(int samples, uint16_t *wavebuf)
{
    //TODO for sound
}

void nes_emulate_frame(void)
{
    uint8_t nes_frame = 0;

    nes_set_window();
    while (1)
    {
        PPU_start_frame();
        for (NES_scanline = 0; NES_scanline < 240; NES_scanline++)
        {
            run6502(113 * 256);
            NES_Mapper->HSync(NES_scanline);

            if (nes_frame == 0)
            {
                //rt_kprintf("NES_scanline = %d \n", NES_scanline);
                scanline_draw(NES_scanline);
            }
            else
            {
                do_scanline_and_dont_draw(NES_scanline);
            }
        }
        NES_scanline = 240;
        run6502(113 * 256);
        NES_Mapper->HSync(NES_scanline);
        start_vblank();
        if (NMI_enabled())
        {
            cpunmi = 1;
            run6502(7 * 256);
        }
        NES_Mapper->VSync();
        // LINES 242-261
        for (NES_scanline = 241; NES_scanline < 262; NES_scanline++)
        {
            run6502(113 * 256);
            NES_Mapper->HSync(NES_scanline);
        }
        end_vblank();
        //nes_get_gamepadval();
        //apu_soundoutput();
        nes_frame_cnt++;
        nes_frame++;
        nes_set_window();

        if (nes_frame > NES_SKIP_FRAME)
        {
            nes_frame = 0;
        }
        if (system_task_return)
        {
            break;
        }
        lcd_st7789_set_window(0, 0, 240 - 1, 240 - 1);
    }
}

static void nes_game_entry(void *parameter)
{
    uint8_t res = 0;
    nes_sram_malloc(0);
    NESrom_crc32 = get_crc32(romfile + 16, sizeof(rom_game) - 16);
    res = nes_load_rom();

    if (res == 0)
    {
        cpu6502_init();
        Mapper_Init();
        PPU_reset();
        apu_init();
        nes_emulate_frame();
    }
    else
    {
        rt_kprintf("NES LOAD FAIL!");
    }

    while (1)
    {
        //do nothing
    }
}

int nes_game(void)
{
    rt_thread_t tid;

    tid = rt_thread_create("nes_game",
                           nes_game_entry, RT_NULL,
                           1024, 25, 10);
    if (tid != RT_NULL)
    {
        rt_thread_startup(tid);
    }

    return 0;
}

MSH_CMD_EXPORT(nes_game, lcd display test);
//INIT_APP_EXPORT(nes_game);
