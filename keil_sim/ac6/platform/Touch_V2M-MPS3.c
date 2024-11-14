/*-----------------------------------------------------------------------------
 * Name:    Touch_V2M-MPS3.c
 * Purpose: Touchscreen (STMPE811) interface for V2M-MPS3 evaluation board
 * Rev.:    1.0.0
 * Note(s): possible defines select the used communication interface:
 *            __USE_SECURE  - use register in secure address space
 *                          - use register in non-secure address space (default)
 *----------------------------------------------------------------------------*/

/* Copyright (c) 2013 - 2017 ARM LIMITED

   All rights reserved.
   Redistribution and use in source and binary forms, with or without
   modification, are permitted provided that the following conditions are met:
   - Redistributions of source code must retain the above copyright
     notice, this list of conditions and the following disclaimer.
   - Redistributions in binary form must reproduce the above copyright
     notice, this list of conditions and the following disclaimer in the
     documentation and/or other materials provided with the distribution.
   - Neither the name of ARM nor the names of its contributors may be used
     to endorse or promote products derived from this software without
     specific prior written permission.
   *
   THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
   AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
   IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
   ARE DISCLAIMED. IN NO EVENT SHALL COPYRIGHT HOLDERS AND CONTRIBUTORS BE
   LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
   CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
   SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
   INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
   CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
   ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
   POSSIBILITY OF SUCH DAMAGE.
   ---------------------------------------------------------------------------*/

#include <stdbool.h>
#include "RTE_Components.h"             /* Component selection*/
#include CMSIS_device_header
#include "Board_Touch.h"                /* ::Board Support:Touchscreen*/
#include "GLCD_Config.h"

/*------------------- FPGA I/O ----------------------------------------------*/
typedef struct /* see "Application Note AN519 Example IoT Kit Subsystem design for V2M-MPS2" */
{
    __IOM  uint32_t LED;                       /* Offset: 0x000 (R/W)  LED connections */
    uint32_t RESERVED0[1];
    __IOM  uint32_t BUTTON;                    /* Offset: 0x008 (R/W)  Buttons */
    uint32_t RESERVED1[1];
    __IOM  uint32_t CLK1HZ;                    /* Offset: 0x010 (R/W)  1Hz up counter */
    __IOM  uint32_t CLK100HZ;                  /* Offset: 0x014 (R/W)  100Hz up counter */
    __IOM  uint32_t COUNTER;                   /* Offset: 0x018 (R/W)  Cycle Up Counter */
    __IOM  uint32_t
    PRESCALE;                  /* Offset: 0x01C (R/W)  Reload value for prescale counter */
    __IOM  uint32_t PSCNTR;                    /* Offset: 0x020 (R/W)  32-bit Prescale counter */
    uint32_t RESERVED3[1];
    __IOM  uint32_t SWITCHES;                  /* Offset: 0x028 (R/W)  Switches */
    uint32_t RESERVED4[8];
    __IOM  uint32_t MISC;                      /* Offset: 0x04C (R/W)  Miscellaneous control */
} IOTKIT_FPGAIO_TypeDef;

/* IOTKIT_FPGAIO_LED Register Definitions */
#define IOTKIT_FPGAIO_LED_LED_Pos               0                                                    /* IOTKIT_FPGAIO LED.LED: LED position */
#define IOTKIT_FPGAIO_LED_LED_Msk              (0x1FFUL /*<< IOTKIT_FPGAIO_LED_LED_Pos*/)            /* IOTKIT_FPGAIO LED.LED: LED mask */

/* IOTKIT_FPGAIO_BUTTON Register Definitions */
#define IOTKIT_FPGAIO_BUTTON_Buttons_Pos        0                                                    /* IOTKIT_FPGAIO BUTTON.Buttons: BUTTON position */
#define IOTKIT_FPGAIO_BUTTON_Buttons_Msk       (0x3UL /*<< IOTKIT_FPGACTRL_BUTTON_Buttons_Pos*/)     /* IOTKIT_FPGAIO BUTTON.Buttons: BUTTON mask */

/* IOTKIT_FPGAIO_SWITCHES Register Definitions */
#define IOTKIT_FPGAIO_SWITCHES_Switches_Pos     0                                                    /* IOTKIT_FPGAIO SWITCHES.Switches: SWITCHES position */
#define IOTKIT_FPGAIO_SWITCHES_Switches_Msk    (0x7UL /*<< IOTKIT_FPGACTRL_SWITCHES_Switches_Pos*/)  /* IOTKIT_FPGAIO SWITCHES.Switches: SWITCHES mask */

/* IOTKIT_FPGAIO_MISC Register Definitions */
#define IOTKIT_FPGAIO_MISC_SHIELD1SPInCS_Pos    9                                                    /* IOTKIT_FPGAIO MISC.SHIELD1SPInCS: SHIELD_1_SPI_nCS position */
#define IOTKIT_FPGAIO_MISC_SHIELD1SPInCS_Msk   (0x1UL << IOTKIT_FPGAIO_MISC_SHIELD1SPInCS_Pos)       /* IOTKIT_FPGAIO MISC.SHIELD1SPInCS: SHIELD_1_SPI_nCS mask */

#define IOTKIT_FPGAIO_MISC_SHIELD0SPInCS_Pos    8                                                    /* IOTKIT_FPGAIO MISC.SHIELD0SPInCS: SHIELD_0_SPI_nCS position */
#define IOTKIT_FPGAIO_MISC_SHIELD0SPInCS_Msk   (0x1UL << IOTKIT_FPGAIO_MISC_SHIELD0SPInCS_Pos)       /* IOTKIT_FPGAIO MISC.SHIELD0SPInCS: SHIELD_0_SPI_nCS mask */

#define IOTKIT_FPGAIO_MISC_ADCSPInCS_Pos        7                                                    /* IOTKIT_FPGAIO MISC.ADCSPInCS: ADC_SPI_nCS position */
#define IOTKIT_FPGAIO_MISC_ADCSPInCS_Msk       (0x1UL << IOTKIT_FPGAIO_MISC_ADCSPInCS_Pos)           /* IOTKIT_FPGAIO MISC.ADCSPInCS: ADC_SPI_nCS mask */



/*------------------- I2C ----------------------------------------------*/
typedef struct /* see Document ? */
{
    union
    {
        __OM  uint32_t CONTROLS;                 /* Offset: 0x000 ( /W)  CONTROL Set Register */
        __IM  uint32_t CONTROL;                  /* Offset: 0x000 (R/ )  CONTROL Status Register */
    };
    __OM  uint32_t CONTROLC;                   /* Offset: 0x004 ( /W)  CONTROL Clear Register */
} IOTKIT_I2C_TypeDef;

/* IOTKIT_I2C_CONTROLS Register Definitions */
#define IOTKIT_I2C_CONTROLS_SDA_Pos            1                                                     /* IOTKIT_I2C CONTROLS.SDA: SDA position */
#define IOTKIT_I2C_CONTROLS_SDA_Msk           (1UL << IOTKIT_I2C_CONTROLS_SDA_Pos)                   /* IOTKIT_I2C CONTROLS.SDA: SDA mask */

#define IOTKIT_I2C_CONTROLS_SCL_Pos             0                                                    /* IOTKIT_I2C CONTROLS.SCL: SCL position */
#define IOTKIT_I2C_CONTROLS_SCL_Msk            (1UL /*<< IOTKIT_I2C_CONTROLS_SCL_Pos*/)              /* IOTKIT_I2C CONTROLS.SCL: SCL mask */

/* IOTKIT_I2C_CONTROL Register Definitions */
#define IOTKIT_I2C_CONTROL_SDA_Pos              1                                                    /* IOTKIT_I2C CONTROL.SDA: SDA position */
#define IOTKIT_I2C_CONTROL_SDA_Msk             (1UL << IOTKIT_I2C_CONTROL_SDA_Pos)                   /* IOTKIT_I2C CONTROL.SDA: SDA mask */

#define IOTKIT_I2C_CONTROL_SCL_Pos              0                                                    /* IOTKIT_I2C CONTROL.SCL: SCL position */
#define IOTKIT_I2C_CONTROL_SCL_Msk             (1UL /*<< IOTKIT_I2C_CONTROL_SCL_Pos*/)               /* IOTKIT_I2C CONTROL.SCL: SCL mask */

/* IOTKIT_I2C_CONTROLC Register Definitions */
#define IOTKIT_I2C_CONTROLC_SDA_Pos             1                                                    /* IOTKIT_I2C CONTROLC.SDA: SDA position */
#define IOTKIT_I2C_CONTROLC_SDA_Msk            (1UL << IOTKIT_I2C_CONTROLC_SDA_Pos)                  /* IOTKIT_I2C CONTROLC.SDA: SDA mask */

#define IOTKIT_I2C_CONTROLC_SCL_Pos             0                                                    /* IOTKIT_I2C CONTROLC.SCL: SCL position */
#define IOTKIT_I2C_CONTROLC_SCL_Msk            (1UL /*<< IOTKIT_I2C_CONTROLC_SCL_Pos*/)              /* IOTKIT_I2C CONTROLC.SCL: SCL mask */





#define IOTKIT_FPGAIO_BASE                      (0x49302000)      /* FPGA - IO ( Sytem Ctrl + I/O) */
#define IOTKIT_I2C0_BASE                        (0x49200000)        /* FPGA - SBCon I2C (Touch) */

#define IOTKIT_FPGAIO   ((IOTKIT_FPGAIO_TypeDef           *) IOTKIT_FPGAIO_BASE)
#define IOTKIT_I2C0     ((IOTKIT_I2C_TypeDef              *) IOTKIT_I2C0_BASE)

#if defined (RTE_CMSIS_RTOS) && !defined (RTE_CMSIS_RTOS2)
#include "cmsis_os.h"               /* ARM::CMSIS:RTOS:Keil RTX */
#elif defined (RTE_CMSIS_RTOS2)
#include "cmsis_os2.h"              /* ARM::CMSIS:RTOS:Keil RTX5 */
#endif

#define TSC_I2C_ADDR      0x41            /* 7-bit I2C address */
#define TSC_I2C_CID       0x0811

/*-- STMPE811 Register addresses ---------------------------------------------*/
#define STMPE811_CHIP_ID        0x00
#define STMPE811_ID_VER         0x02
#define STMPE811_SYS_CTRL1      0x03
#define STMPE811_SYS_CTRL2      0x04
#define STMPE811_SPI_CFG        0x08
#define STMPE811_INT_CTRL       0x09
#define STMPE811_INT_EN         0x0A
#define STMPE811_INT_STA        0x0B
#define STMPE811_GPIO_EN        0x0C
#define STMPE811_GPIO_INT_STA   0x0D
#define STMPE811_ADC_INT_EN     0x0E
#define STMPE811_ADC_INT_STA    0x0F
#define STMPE811_GPIO_SET_PIN   0x10
#define STMPE811_GPIO_CLR_PIN   0x11
#define STMPE811_GPIO_MP_STA    0x12
#define STMPE811_GPIO_DIR       0x13
#define STMPE811_GPIO_ED        0x14
#define STMPE811_GPIO_RE        0x15
#define STMPE811_GPIO_FE        0x16
#define STMPE811_GPIO_AF        0x17
#define STMPE811_ADC_CTRL1      0x20
#define STMPE811_ADC_CTRL2      0x21
#define STMPE811_ADC_CAPT       0x22
#define STMPE811_ADC_DATA_CH0   0x30
#define STMPE811_ADC_DATA_CH1   0x32
#define STMPE811_ADC_DATA_CH2   0x34
#define STMPE811_ADC_DATA_CH3   0x36
#define STMPE811_ADC_DATA_CH4   0x38
#define STMPE811_ADC_DATA_CH5   0x3A
#define STMPE811_ADC_DATA_CH6   0x3C
#define STMPE811_ADC_DATA_CH7   0x3E
#define STMPE811_TSC_CTRL       0x40
#define STMPE811_TSC_CFG        0x41
#define STMPE811_WDW_TR_X       0x42
#define STMPE811_WDW_TR_Y       0x44
#define STMPE811_WDW_BL_X       0x46
#define STMPE811_WDW_BL_Y       0x48
#define STMPE811_FIFO_TH        0x4A
#define STMPE811_FIFO_STA       0x4B
#define STMPE811_FIFO_SIZE      0x4C
#define STMPE811_TSC_DATA_X     0x4D
#define STMPE811_TSC_DATA_Y     0x4F
#define STMPE811_TSC_DATA_Z     0x51
#define STMPE811_TSC_FRACTION_Z 0x56
#define STMPE811_TSC_DATA_XYZ   0x57
#define STMPE811_TSC_DATA       0xD7
#define STMPE811_TSC_I_DRIVE    0x58
#define STMPE811_TSC_SHIELD     0x59
#define STMPE811_TEMP_CTRL      0x60
#define STMPE811_TEMP_DATA      0x61
#define STMPE811_TEMP_TH        0x62


#if defined __USE_SECURE
#define FPGASYS_PORT   IOTKIT_SECURE_FPGAIO
#define I2C_PORT       IOTKIT_SECURE_I2C0
#else
#define FPGASYS_PORT   IOTKIT_FPGAIO
#define I2C_PORT       IOTKIT_I2C0
#endif

#define I2C_TYPE         IOTKIT_I2C_TypeDef

/*-- I2C interface -----------------------------------------------------------*/
/* Local Function */

#define SDA                (1 << 1)
#define SCL                (1 << 0)
#define TSC_TSU            (1 * 15)          /* Setup delay 600nS min */

#define I2C_HIGH(pin)      (i2c->CONTROLS = pin)
#define I2C_LOW(pin)       (i2c->CONTROLC = pin)
#define I2C_GET(pin)       ((i2c->CONTROL >> (pin - 1)) & 0x01)
#define I2C_DLY()          I2C_delay(TSC_TSU)

/* Function to delay n*ticks (25MHz = 40nS per tick) */
/* Used for I2C drivers */
static void I2C_delay(unsigned int tick)
{
    uint32_t end;
    uint32_t start;

    start = FPGASYS_PORT->COUNTER;
    end   = start + (tick);

    if (end >= start)
    {
        while (FPGASYS_PORT->COUNTER >= start && FPGASYS_PORT->COUNTER < end);
    }
    else
    {
        while (FPGASYS_PORT->COUNTER >= start);
        while (FPGASYS_PORT->COUNTER < end);
    }
}

/* Clear the I2C interface as there is no RESET control */
static void I2C_clear(I2C_TYPE *i2c)
{
    uint32_t loop;

    for (loop = 0; loop < 5; loop++)
    {
        /* Start/Stop bit */
        I2C_LOW(SDA);
        I2C_DLY();
        I2C_LOW(SCL);
        I2C_DLY();
        I2C_HIGH(SCL);
        I2C_DLY();
        I2C_HIGH(SDA);
        I2C_DLY();
    }

}

/* transmit start bit */
void I2C_txStart(I2C_TYPE *i2c);
void I2C_txStart(I2C_TYPE *i2c)
{
    /* HIGH to LOW on SDA while SCL is HIGH */
    I2C_HIGH((SCL | SDA));
    I2C_DLY();
    I2C_LOW(SDA);
    I2C_DLY();

    I2C_LOW(SCL);
    I2C_DLY();
}

/* transmit stop bit */
void I2C_txStop(I2C_TYPE *i2c);
void I2C_txStop(I2C_TYPE *i2c)
{
    /* LOW to HIGH on SDA while SCL is HIGH */
    I2C_LOW(SDA);
    I2C_DLY();
    I2C_HIGH(SCL);
    I2C_DLY();
    I2C_HIGH(SDA);
    I2C_DLY();
}

/* transmit a data bit */
void I2C_txBit(I2C_TYPE *i2c, uint8_t bit);
void I2C_txBit(I2C_TYPE *i2c, uint8_t bit)
{

    if (bit != 0)
    {
        I2C_HIGH(SDA);
    }
    else
    {
        I2C_LOW(SDA);
    }
    I2C_DLY();

    I2C_HIGH(SCL);
    I2C_DLY();

    I2C_LOW(SCL);
    /*  I2C_DLY(); */
}

/* read a data bit */
uint8_t I2C_rxBit(I2C_TYPE *i2c);
uint8_t I2C_rxBit(I2C_TYPE *i2c)
{
    uint8_t bit;

    I2C_HIGH(SDA);
    I2C_DLY();

    I2C_HIGH(SCL);
    I2C_DLY();

    bit = I2C_GET(SDA);

    I2C_LOW(SCL);
    /*  I2C_DLY(); */

    return (bit);
}

/* Read the acknowledge bit */
uint32_t I2C_rxAck(I2C_TYPE *i2c);
uint32_t I2C_rxAck(I2C_TYPE *i2c)
{
    uint8_t ack;

    ack = I2C_rxBit(i2c);

    return (ack);
}

/* Write the acknowledge bit */
void I2C_txAck(I2C_TYPE *i2c, uint8_t ack);
void I2C_txAck(I2C_TYPE *i2c, uint8_t ack)
{

    I2C_txBit(i2c, ack);

}


/* Write 8 bits of data to the serial bus */
void I2C_txByte(I2C_TYPE *i2c, uint8_t data);
void I2C_txByte(I2C_TYPE *i2c, uint8_t data)
{
    uint8_t numBits;

    for (numBits = 0; numBits < 8; numBits++)
    {
        I2C_txBit(i2c, data & 0x80);
        data <<= 1;
    }

}

/* Read 8 bits of data from the serial bus */
uint8_t I2C_rxByte(I2C_TYPE *i2c);
uint8_t I2C_rxByte(I2C_TYPE *i2c)
{
    uint8_t numBits;
    uint8_t data = 0;

    for (numBits = 0; numBits < 8; numBits++)
    {
        data <<= 1;
        data |= I2C_rxBit(i2c);
    }

    return data;
}

void I2C_tx(I2C_TYPE *i2c, uint8_t sadr, uint8_t *data, uint8_t num, bool xferPending);
void I2C_tx(I2C_TYPE *i2c, uint8_t sadr, uint8_t *data, uint8_t num, bool xferPending)
{
    uint32_t numBytes;

    I2C_txStart(i2c);
    I2C_txByte(i2c, sadr);
    I2C_rxAck(i2c);
    for (numBytes = 0; numBytes < num; numBytes++)
    {
        I2C_txByte(i2c, data[numBytes]);
        I2C_rxAck(i2c);
    }
    if (!xferPending)
    {
        I2C_txStop(i2c);
    }
}

void I2C_rx(I2C_TYPE *i2c, uint8_t sadr, uint8_t *data, uint8_t num, bool xferPending);
void I2C_rx(I2C_TYPE *i2c, uint8_t sadr, uint8_t *data, uint8_t num, bool xferPending)
{
    uint32_t numBytes;

    I2C_txStart(i2c);
    I2C_txByte(i2c, sadr);
    I2C_rxAck(i2c);
    data[0] = I2C_rxByte(i2c);
    for (numBytes = 1; numBytes < num; numBytes++)
    {
        I2C_txAck(i2c, 0);
        data[numBytes] = I2C_rxByte(i2c);
    }
    I2C_txAck(i2c, 1);

    if (!xferPending)
    {
        I2C_txStop(i2c);
    }
}



/*-----------------------------------------------------------------------------
  I2C_Initialize: Initialize I2C Interface.
 *----------------------------------------------------------------------------*/
static void I2C_Initialize(void)
{

    I2C_clear(I2C_PORT);  /* Configure I2C Pins */
}

/*-----------------------------------------------------------------------------
  I2C_Uninitialize: De-initialize I2C Interface.
 *----------------------------------------------------------------------------*/
static void I2C_Uninitialize(void)
{

    I2C_clear(I2C_PORT);  /* Unconfigure SCL and SDA pins */
}

/*-----------------------------------------------------------------------------
  I2C_MasterTransmit: Transmitting data as I2C Master.
 *----------------------------------------------------------------------------*/
static int32_t I2C_MasterTransmit(uint8_t        addr,
                                  uint8_t       *data,
                                  uint8_t        num,
                                  bool           xfer_pending)
{

    if (!data || !num || (addr > 0x7F))
    {
        return -1;    /* Invalid parameters */
    }

    /* transmit data */
    I2C_tx(I2C_PORT, (uint8_t)(addr << 1), data, num, xfer_pending);

    return 0;
}

/*-----------------------------------------------------------------------------
  I2C_MasterReceive: Receiving data as I2C Master
 *----------------------------------------------------------------------------*/
static int32_t I2C_MasterReceive(uint8_t        addr,
                                 uint8_t       *data,
                                 uint8_t        num,
                                 bool           xfer_pending)
{

    if (!data || !num || (addr > 0x7F))
    {
        return -1;    /* Invalid parameters */
    }

    /* receive data */
    I2C_rx(I2C_PORT, ((uint8_t)(addr << 1) | 0x01), data, num, xfer_pending);

    return 0;
}


/*-- Touch screen interface --------------------------------------------------*/
/**
  \fn          void delay_ms (int ms)
  \brief       Delay execution for a specified number of milliseconds
  \param[in]   ms     number of while loop iterations
*/
static void delay_ms(int ms)
{
#if defined (RTE_CMSIS_RTOS) && !defined (RTE_CMSIS_RTOS2)
    osDelay(ms);
#elif defined (RTE_CMSIS_RTOS2)
    if (osKernelGetState() == osKernelRunning)
    {
        osDelay(ms);
    }
    else
    {
        ms *= (SystemCoreClock / 1000);
        while (ms--) { __NOP(); __NOP(); __NOP(); __NOP(); __NOP(); __NOP(); }
    }
#else
    ms *= (SystemCoreClock / 1000);
    while (ms--) { __NOP(); __NOP(); __NOP(); __NOP(); __NOP(); __NOP(); }
#endif
}

/**
  \fn          bool Touch_ReadReg (uint8_t reg, uint8_t *val)
  \brief       Read data from Touchscreen controller register
  \param[in]   reg    Register to read
  \param[in]   val    Pointer to read data
  \returns
   - \b  0: function succeeded
   - \b -1: function failed
*/
static int32_t Touch_ReadReg(uint8_t reg, uint8_t *val)
{
    uint8_t data[1];

    data[0] = reg;
    I2C_MasterTransmit(TSC_I2C_ADDR, data, 1, true);
    I2C_MasterReceive(TSC_I2C_ADDR, val, 1, false);

    return 0;
}

/**
  \fn          bool Touch_WriteReg (uint8_t reg, uint8_t data)
  \brief       Write data to Touchscreen controller register
  \param[in]   reg    Register to write
  \param[in]   val    Data to write
  \returns
   - \b  0: function succeeded
   - \b -1: function failed
*/
static int32_t Touch_WriteReg(uint8_t reg, uint8_t val)
{
    uint8_t data[2];

    data[0] = reg;
    data[1] = val;
    I2C_MasterTransmit(TSC_I2C_ADDR, data, 2, false);

    return 0;
}

/**
  \fn          int32_t Touch_Initialize (void)
  \brief       Initialize touchscreen
  \returns
   - \b  0: function succeeded
   - \b -1: function failed
*/
int32_t Touch_Initialize(void)
{
    uint8_t Touch_ChipID[2];

    I2C_Initialize();

    Touch_ChipID[0] = STMPE811_CHIP_ID;
    I2C_MasterTransmit(TSC_I2C_ADDR, Touch_ChipID, 1, true);
    I2C_MasterReceive(TSC_I2C_ADDR, Touch_ChipID, 2, false);

    Touch_WriteReg(STMPE811_SYS_CTRL1,      0x02); /* Reset Touch-screen controller */
    delay_ms(10);

    Touch_WriteReg(STMPE811_SYS_CTRL2,      0x0C); /* Enable TSC and ADC */
    Touch_WriteReg(STMPE811_INT_EN,         0x07); /* Enable Touch detect, FIFO */
    Touch_WriteReg(STMPE811_ADC_CTRL1,      0x69); /* Set sample time , 12-bit mode */
    delay_ms(10);

    Touch_WriteReg(STMPE811_ADC_CTRL2,      0x01); /* ADC frequency 3.25 MHz */
    Touch_WriteReg(STMPE811_TSC_CFG,        0xC2); /* Detect delay 10us, Settle time 500us */
    Touch_WriteReg(STMPE811_FIFO_TH,        0x01); /* Threshold for FIFO */
    Touch_WriteReg(STMPE811_FIFO_STA,       0x01); /* FIFO reset */
    Touch_WriteReg(STMPE811_FIFO_STA,       0x00); /* FIFO not reset */
    Touch_WriteReg(STMPE811_TSC_FRACTION_Z, 0x07); /* Fraction z */
    Touch_WriteReg(STMPE811_TSC_I_DRIVE,    0x01); /* Drive 50 mA typical */
    Touch_WriteReg(STMPE811_GPIO_AF,        0x00); /* Pins are used for touchscreen */
    Touch_WriteReg(STMPE811_TSC_CTRL,       0x01); /* Enable TSC */
    Touch_WriteReg(STMPE811_INT_STA,        0xFF); /* Clear interrupt status */

    return 0;
}

/**
  \fn          int32_t Touch_Uninitialize (void)
  \brief       De-initialize touchscreen
  \returns
   - \b  0: function succeeded
   - \b -1: function failed
*/
int32_t Touch_Uninitialize(void)
{

    Touch_WriteReg(STMPE811_SYS_CTRL1, 0x02);  /* Reset Touch-screen controller    */

    I2C_Uninitialize();

    return 0;
}


#define TSC_XOFF           10          // X offset
#define TSC_YOFF           10          // Y offset
#define TSC_MAXVAL         37000       // 0x0FFF * 10 with TSC to LCD scaling

/**
  \fn          int32_t Touch_GetState (TOUCH_STATE *pState)
  \brief       Get touchscreen state
  \param[out]  pState  pointer to TOUCH_STATE structure
  \returns
   - \b  0: function succeeded
   - \b -1: function failed
*/
int32_t Touch_GetState(TOUCH_STATE *pState)
{
    uint8_t val;
    uint8_t num;
    uint8_t xyz[4];
    uint32_t X, Y, XPXL, YPXL, XPXLN, YPXLN;


    /* Read touch status */
    Touch_ReadReg(STMPE811_TSC_CTRL, &val);
    pState->pressed = (val & (1 << 7)) ? 1 : 0;

    if (pState->pressed)
    {
        val = STMPE811_TSC_DATA;

        /* If FIFO overflow, discard all samples except the last one */
        Touch_ReadReg(STMPE811_FIFO_SIZE, &num);
        if (num == 0) { return -1; }

        while (num--)
        {
            I2C_MasterTransmit(TSC_I2C_ADDR, &val, 1, true);
            I2C_MasterReceive(TSC_I2C_ADDR, xyz, 4, false);
        }
        X = (int16_t)((xyz[0] << 4) | ((xyz[1] & 0xF0) >> 4));
        Y = (int16_t)((xyz[2]) | ((xyz[1] & 0x0F) << 8));

        do
        {
            // Calculate the pixel position (X/Y are swapped)
            XPXL = GLCD_WIDTH - ((Y * 10) / (TSC_MAXVAL / GLCD_WIDTH)) + TSC_XOFF;
            YPXL = ((X * 10) / (TSC_MAXVAL / GLCD_HEIGHT)) - TSC_YOFF;
            XPXL = (XPXL & 0x80000000) ? 0 : XPXL;
            YPXL = (YPXL & 0x80000000) ? 0 : YPXL;
            XPXL = (XPXL >= GLCD_WIDTH) ? GLCD_WIDTH  - 1 : XPXL;
            YPXL = (YPXL >= GLCD_HEIGHT) ? GLCD_HEIGHT - 1 : YPXL;

        }
        while (0);

        pState->x = XPXL;
        pState->y = YPXL;
    }

    /* Clear interrupt flags */
    Touch_WriteReg(STMPE811_INT_STA, 0x1F);

    return 0;
}
