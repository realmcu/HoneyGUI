/*********************************************************************************************************
 *               Copyright(c) 2022, vcare Corporation. All rights reserved.
 **********************************************************************************************************
 * @file     vcHr02AHci.c
 * @brief    VC9202A drvier
 * @details  100hz_mode(pee) + gsenosr_wear + hr_wear
 * @author
 * @date
 * @version  v2.04
 *********************************************************************************************************/
#include "vcHr02AHci.h"
#include "biometrics_common.h"
#include "trace.h"
vcHr02_t vcHr02;
/* fdy-dev ID */
static uint8_t FoundryID;
static uint8_t DeviceID;
/* regsister or param */
static uint8_t ledCurrent[SLOTCHNU], ledMaxCurrent[SLOTCHNU] = {0x6f, 0x6f, 0x6f};
static uint8_t adjustResLvL[SLOTCHNU], resMaxLvL[SLOTCHNU] = {7, 7, 7}, constResMaxLvL = 7;
static AdRes_t adjustResP_t[SLOTCHNU] = {0};
static uint8_t pdResSet[SLOTCHNU], ppgGain[SLOTCHNU];
/* envVal limit max r  */
static uint8_t recordEnv[SLOTCHNU] = {0, 0, 0};
static uint16_t count_pdRes[SLOTCHNU], count_QaPdRes[SLOTCHNU];
static uint8_t adjustFlag[2][64];
static uint8_t slotCount, sampleIndex, totalSlot = 0, readIndex = 0, readCnt = 0;
static uint8_t fifoWriteIndex = 0, fifoReadIndex = 0x80, fifoIntDiv = 0;
static uint8_t enFifo, enSlot[SLOTCHNU];
/* clock calc */
static uint32_t lastRtcValue = 0;
static uint16_t oscCnt = 0, cntCalOsc = 0;
static uint32_t intDiffRtc[8] = {0}, intDiffAdj[8] = {0};
static uint8_t oscDifSetting = 2;
/* other param */
//static bool slot0EnvIsExceedFlag, slot1EnvIsExceedFlag;
//static uint8_t psBias = 0;
static uint8_t envBias = 0;
//static bool psBiasReadInPdFlag = false;
/*biometrics struct param*/
static BioParam_t g_stBioParam = {0};
static vcComMode_t g_bioSwitchMode = VCWORK_MODE_HRWORK;
static unsigned char g_isBioMode = 0;
static uint8_t BioInitMark = 1;
/* 2022.07.15 fifoIntMs(800) and ic status */
static uint16_t g_fifoIntMs = 0;
static bool isVCxxSample = false;
static bool isVCxxCheckNomal = false;
static vcHr02WearStatus_e lastWearStatus = VCHR02WEARST_UNWEAR;
/* 20221202,add code:gsensor_hold, heart-1_drop */
volatile static BioWearStatus_e  g_gsensor_hold_sta;
volatile static BioWearStatus_e  g_hr_algo_drop_sta;

/* func */
static uint8_t resReg2resLv(AdRes_t adjustRes_t);
static AdRes_t resLv2resReg(uint8_t resLv);


void vcHr02_heart_algo_wear_status_set(int sta_value)
{
    if (-1 == sta_value)
    {
        g_hr_algo_drop_sta = BIO_WEAR_UNWEAR;
    }
}

unsigned int vcHr02_wear_gsensor_actuating_quantity(int x_axis, int y_axis, int z_axis)
{
    static unsigned int abs_sum_last = 0;
    unsigned int ret_atc = 0;
    unsigned int abs_sum_current = 0;
    if (x_axis < 0)
    {
        x_axis = -x_axis;
    }
    if (y_axis < 0)
    {
        y_axis = -y_axis;
    }
    if (z_axis < 0)
    {
        z_axis = -z_axis;
    }
    abs_sum_current = x_axis + y_axis + z_axis;
    if (abs_sum_last && abs_sum_current)
    {
        if (ABS(abs_sum_last, abs_sum_current) >= 20)
        {
            ret_atc = 1;

            g_gsensor_hold_sta = BIO_WEAR_ISWEAR;
            g_hr_algo_drop_sta = BIO_WEAR_ISWEAR;
        }
        else
        {
            g_gsensor_hold_sta = BIO_WEAR_NULL;
        }
    }
    abs_sum_last = abs_sum_current;
    return ret_atc;
}
/******************************************************************************
 * @brief
 *      Get the fifoIntDiv and slotNum Configuration
 ******************************************************************************/
static void vcHr02ConfigInit(vcHr02_t *pvcHr02, uint8_t isBioSwitch)
{
    static vcComMode_t last_mode = VCWORK_MODE_NULL;

    fifoIntDiv = (pvcHr02->regConfig[3] & 0x3f);
    enFifo = (fifoIntDiv != 0) ? 1 : 0;
    /* Get Slot0 Status */
    /* Get Slot1 Status */
    enSlot[0] = pvcHr02->regConfig[0] & 0x01;
    enSlot[1] = (pvcHr02->regConfig[0] & 0x02) >> 1;
    totalSlot = (pvcHr02->regConfig[0] & 0x03) == 3 ? 2 : ((pvcHr02->regConfig[0] & 0x03) == 0 ? 0 : 1);
    if ((FoundryID == VC013_D_NORM_ID) || (FoundryID == VC018_D_NORM_ID) ||
        (FoundryID == VC015_E_NORM_ID) || (FoundryID == VC018_E_NORM_ID))
    {
        resMaxLvL[0] = 17;
        resMaxLvL[1] = 17;
        resMaxLvL[2] = 17;
        constResMaxLvL = 17;
    }
    else
    {
        resMaxLvL[0] = 7;
        resMaxLvL[1] = 7;
        resMaxLvL[2] = 7;
        constResMaxLvL = 7;
    }
    pvcHr02->adjustInfo[0].step = 4;
    pvcHr02->adjustInfo[0].directionLast = VCHR02ADJUSTDIRECTION_NULL;
    pvcHr02->adjustInfo[0].direction = VCHR02ADJUSTDIRECTION_NULL;

    pvcHr02->adjustInfo[1].step = 4;
    pvcHr02->adjustInfo[1].directionLast = VCHR02ADJUSTDIRECTION_NULL;
    pvcHr02->adjustInfo[1].direction = VCHR02ADJUSTDIRECTION_NULL;

    pvcHr02->wearStatus = lastWearStatus; // VCHR02WEARST_UNWEAR;
    pvcHr02->vcPsFlag = 0;
    pvcHr02->vcFifoReadFlag = 0;
    /* 20221202,init param.default is_wear status */
    g_gsensor_hold_sta = BIO_WEAR_NULL;
    g_hr_algo_drop_sta = BIO_WEAR_NULL;

    ledMaxCurrent[0] = 0x6f;
    ledMaxCurrent[1] = 0x6f;
    recordEnv[0] = 0;
    recordEnv[1] = 0;

    if (BioInitMark || pvcHr02->workMode == VCWORK_MODE_HR100HZ || last_mode == VCWORK_MODE_HR100HZ)
    {
        BioConfig_t bioCfg =
        {
            .icSerial = BIO_IC_SERIAL,
            .bioMode = BIO_MODE_BASE, // BIO_MODE_NONE,BIO_MODE_BASE,BIO_MODE_PLUS,BIO_MODE_PLMX
#if ENV_PRE_IS_4BIT
            .width = 4,
#else
            .width = 8,
#endif
            .bioTime = g_fifoIntMs,
            .psTh = BIO_PSTH_MAX,
            .envMinTh = BIO_ENVTH_MIN,
            .envMaxTh = BIO_ENVTH_MAX,
            .bioIsWearCnt = BIO_ISWEAR_CNT,
            .bioUnWearCnt = BIO_UNWEAR_CNT,
            .bioDebounceCnt = BIO_BOUNCE_CNT,
            .bioInput = BIO_SENSE_PARAM,
            .psInPut = BIO_PS_PARAM,
            .bioThMax = BIOMETRICS_MAX,
            .bioThMin = BIOMETRICS_MIN,
        };
        if (pvcHr02->workMode == VCWORK_MODE_HR100HZ)
        {
            bioCfg.bioMode = BIO_MODE_BASE;
        }
#if (VCWEAR_STATUS_DETECTION_MODE == VCWEAR_STATUS_DETECTION_OFF)
        bioCfg.bioMode = BIO_MODE_NONE;
#endif
        bio_arithemtic_config(&bioCfg, &g_stBioParam);
        BioInitMark = 0;
        char verInfo[64] = {0};
        bio_get_software_version(verInfo);
    }
    /*20220628,biometrics function,default unwear & disable ppgSlot */
    if (pvcHr02->initDisablePpgFlag)
    {
        pvcHr02->initDisablePpgFlag = false;
        vcHr02_bio_disable_sample_slot();
    }
    bio_mode_init(&g_stBioParam);
    if (!isBioSwitch) /* extern init need clear biometrics switch infomation */
    {
        g_bioSwitchMode = VCWORK_MODE_HRWORK;
        g_isBioMode = 0;
        bio_clear_bio_finished_status();
    }

    if (last_mode != pvcHr02->workMode)
    {
        last_mode = pvcHr02->workMode;
    }
}

/******************************************************************************
 * @brief
 *      Initialize vcHr02's register Settings
 ******************************************************************************/
static vcHr02Ret_t vcHr02RegInit(vcHr02_t *pvcHr02, uint8_t isBioSwitch)
{
    uint8_t i = 0;
    uint8_t regConfig[17] = {0x01, 0x3F, 0x8A, 0x00, 0x03, 0x1F, 0x00,
                             0x00, 0x80, 0x00, /* 07-Gain,CUR */
                             0x37, 0x37, 0x07, /* 10-PDHS,res */
                             0x16, 0x56, 0x16, /* 13-envT,sampleT,AFE*/
                             0x00
                            };
    uint8_t regConfig_exc[4] = {0};
    vcHr02Ret_t ret = VCHR02RET_ISOK;
    ret |= vcHr02ReadRegisters(0x00, (uint8_t *)&DeviceID, 1);
    DBG_DIRECT("DeviceID:%d", DeviceID);
    if (DeviceID != VCHR02_DEV_ID)
    {
        return VCHR02RET_ISERR;
    }
    ret |= vcHr02ReadRegisters(0x40, (uint8_t *)&FoundryID, 1);

    regConfig[4] = ((20000 / pvcHr02->vcSampleRate) - 1) >> 8;
    regConfig[5] = (20000 / pvcHr02->vcSampleRate) - 1;
    if (!g_fifoIntMs)
    {
        g_fifoIntMs = 800;
    }
    uint8_t fifoDiv = (g_fifoIntMs / (1000 / pvcHr02->vcSampleRate)) - 1;
    regConfig[3] = regConfig[3] | fifoDiv;
    regConfig[6] = fifoDiv;

#if (VCHR02_DEV_TYPE == VCHR02ISVC32S)
    regConfig[9] = 0x50; // CUR = 40mA
    regConfig[12] = 0x17;
#elif (VCHR02_DEV_TYPE == VCHR02ISVC51B)
    regConfig[9] = 0x5A;  // CUR = 60mA
    regConfig[12] = 0x17; // PDRES = 0x17;
#elif (VCHR02_DEV_TYPE == VCHR02ISVC52S)
    regConfig[9] = 0x30;  // CUR = 10mA
    regConfig[12] = 0x17; // PDRES = 0x17;
#elif (VCHR02_DEV_TYPE == VCHR02ISVC52T)
    regConfig[9] = 0x50;  // modify by Tatum,Using our demo for black testing 2020.04.12
    regConfig[12] = 0x37; // PDRES = 0x37;
#elif (VCHR02_DEV_TYPE == VCHR02ISVC9201)
    regConfig[9] = 0x5A;  // CUR = 5mA//0x20
    regConfig[12] = 0x17; // PDRES = 0x17;
#elif (VCHR02_DEV_TYPE == VCHR02ISVP35)
    regConfig[9] = 0x5A;  // CUR = 5mA//0x20
    regConfig[12] = 0x17; // PDRES = 0x17;
#elif (VCHR02_DEV_TYPE == VCHR02ISVP60A1)
    regConfig[9] = 0x50; // CUR = 40mA
    regConfig[12] = 0x17;
#elif (VCHR02_DEV_TYPE == VCHR02ISVP60A2)
    regConfig[9] = 0x47;  // CUR = 20mA
    regConfig[12] = 0x17; // PDRES = 0x17;
#else
    regConfig[9] = 0X5A;  // CUR = 60mA
    regConfig[12] = 0x17; // PDRES = 0x17;
#endif

    switch (pvcHr02->workMode)
    {
    case VCWORK_MODE_HRWORK:
        regConfig[0] = 0x45;
#if HW_HALVE_POWER_CAP
        regConfig[10] = 0x54;
        regConfig[13] = 0x03;   //
        regConfig[16] = 0x01;
#endif
        break;
    case VCWORK_MODE_HR100HZ:
        regConfig[0] = 0x45;
#if HW_HALVE_POWER_CAP
        regConfig[10] = 0x54;
        regConfig[13] = 0x03;   //
        regConfig[16] = 0x01;
#endif
        break;
    case VCWORK_MODE_SPO2WORK:
        regConfig[0] = 0x47;
        regConfig[1] = 0x2F;
        regConfig[6] = 0;
        regConfig[7] = 0x80;
        // regConfig[13] = 0x96;
        regConfig[13] = 0xB6;
        regConfig[14] = 0x76;
        regConfig[16] = 0x04; // OverSample
        break;
    case VCWORK_MODE_CROSSTALKTEST:
        regConfig[0] = 0x45;
        regConfig[7] = 0xEF;  // solt0 maxLedCur
        regConfig[8] = 0xEF;  // solt1 maxLedCur
        regConfig[10] = 0x77; // solt0 PDResMax
        regConfig[11] = 0x77; // solt1 PDResMax
#if HW_HALVE_POWER_CAP
        regConfig[10] = 0x74;//0x77;
        regConfig[13] = 0x03;   //0x16
        regConfig[16] = 0x01;
#endif
        break;
    case VCWORK_MODE_HR_NOISEWORK:
        regConfig[0] = 0x41;
        regConfig[10] = 0x57;
        regConfig[13] = 0x16;
        regConfig[16] = 0x00;
#if HW_HALVE_POWER_CAP
        regConfig[10] = 0x54;
        regConfig[13] = 0x03;
        regConfig[16] = 0x01;
#endif
        break;
    case VCWORK_MODE_LPDETECTION:
        regConfig[0] = 0x44;
        regConfig[1] = 0x30;
        break;
    case VCWORK_MODE_WEAR_BIOMETRICS:
        regConfig[0] = 0x47;
        regConfig[10] = 0x37;
        regConfig[11] = 0x37;
        regConfig[14] = 0x96;
#if HW_HALVE_POWER_CAP
        regConfig[10] = 0x34;
        regConfig[13] = 0x03;
        regConfig[16] = 0x01;
#endif
        break;
    default:
        break;
    }
    /* 9201D/E/F new config */
    if ((FoundryID == VC013_D_NORM_ID) || (FoundryID == VC018_D_NORM_ID) ||
        (FoundryID == VC015_E_NORM_ID) || (FoundryID == VC018_E_NORM_ID))
    {
        regConfig_exc[0] = 0x00; /* ATIOSET_DTOSET */
        regConfig_exc[1] = 0x00 | (1 << 3);
        if ((FoundryID == VC013_D_NORM_ID) || (FoundryID == VC018_D_NORM_ID)) /*  */
        {
            regConfig_exc[2] = 0x00;
            regConfig_exc[3] = 0x00;
        }
        else if ((FoundryID == VC015_E_NORM_ID) || (FoundryID == VC018_E_NORM_ID)) /* BIO_CFG */
        {
            regConfig_exc[2] = 0;
            regConfig_exc[3] = 0xA2; /* 0xE0->0xE2->A2 */
        }
        switch (pvcHr02->workMode)
        {
        case VCWORK_MODE_HRWORK:
            regConfig[16] = regConfig[16] | (0 << 6);
            break;
        case VCWORK_MODE_SPO2WORK:
            regConfig[16] = (0 << 7) | (0 << 6) | (0 << 3) | (4 << 0); // OverSample solt0 2^4 solt1 2^4 = 0xc4
            break;
        case VCWORK_MODE_HR100HZ:
            regConfig_exc[3] = 0; /* 0xE0->0xE2->A2 */
            break;
        default:
            break;
        }
    } /* end of 9201D/E/F new config */
    for (i = 0; i < 17; i++)
    {
        pvcHr02->regConfig[i] = regConfig[i];
    }
    for (i = 0; i < 4; i++)
    {
        pvcHr02->regConfig[17 + i] = regConfig_exc[i];
    }
    if ((FoundryID == VC013_D_NORM_ID) || (FoundryID == VC018_D_NORM_ID) ||
        (FoundryID == VC015_E_NORM_ID) || (FoundryID == VC018_E_NORM_ID))
    {
        ret |= vcHr02WriteRegisters(VCREG11, pvcHr02->regConfig, 21);
    }
    else
    {
        ret |= vcHr02WriteRegisters(VCREG11, pvcHr02->regConfig, 17);
    }
    pvcHr02->regConfig[0] = 0x80 | regConfig[0];
    ret |= vcHr02WriteRegisters(VCREG11, pvcHr02->regConfig, 1);
    vcHr02ConfigInit(pvcHr02, isBioSwitch);
    return ret;
}

/*****************************************************************************
 * @brief
 *      Soft reset to make all registor
 *      Write 8'h5A to 8'h3B cause soft reset.
 *****************************************************************************/
vcHr02Ret_t vcHr02SoftReset(vcHr02_t *pvcHr02)
{
    uint8_t Reset = 0x5A;
    vcHr02Ret_t ret = VCHR02RET_ISOK;

    ret |= vcHr02WriteRegisters(VCREG10, (uint8_t *)&Reset, VCREG10_SIZE);
    oscCnt = 0;
    pvcHr02->oscCheckFinishFlag = 0;
    fifoWriteIndex = 0;
    fifoReadIndex = 0x80;
    return ret;
}

/******************************************************************************
 * @brief
 *      shut down the logic channel, enter sleep work mode
 ******************************************************************************/
vcHr02Ret_t vcHr02StopSample(vcHr02_t *pvcHr02)
{
    /* Shut down all logic solt and goto sleep mode */
    uint8_t modeCtrl = 0x00;
    vcHr02Ret_t ret = VCHR02RET_ISOK;
    ret |= vcHr02WriteRegisters(VCREG11, (uint8_t *)&modeCtrl, VCREG11_SIZE);
    isVCxxSample = false;
    return ret;
}

/******************************************************************************
 * @brief
 *      IC start work sample
 ******************************************************************************/
vcHr02Ret_t vcHr02StartSample(vcHr02_t *pvcHr02)
{
    vcHr02Ret_t ret = VCHR02RET_ISOK;
    /* external call */
    vcHr02StopSample(pvcHr02);
    vcHr02SoftReset(pvcHr02);
    ret |= vcHr02RegInit(pvcHr02, 0);
    isVCxxSample = true;
    if (ret == VCHR02RET_ISOK)
    {
        isVCxxCheckNomal = true;
    }
    return ret;
}

/******************************************************************************
 * @brief
 * Function: Added adjust flag when interrupt happend.
 *            Provides it for algorithmic processing.
 *****************************************************************************/
static vcHr02Ret_t vcHr02AddAdjustFlag(uint8_t slotNum)
{
    vcHr02Ret_t ret = VCHR02RET_ISOK;
    /* If fifoIntDiv is 0,FIFO Function disable */
    if (fifoIntDiv == 0)
    {
        adjustFlag[slotNum][0] = 1;
    }
    else
    {
        ret |= vcHr02ReadRegisters(VCREG3, (uint8_t *)&fifoWriteIndex, VCREG3_SIZE);
        sampleIndex = (fifoWriteIndex - 0x80) / 2 / totalSlot;
        if (sampleIndex == 0)
        {
            sampleIndex = totalSlot == 1 ? 63 : 31;
        }
        else
        {
            sampleIndex = sampleIndex - 1;
        }
        adjustFlag[slotNum][sampleIndex] = 1;
    }
    return ret;
}

/******************************************************************************
 * @brief
 *      Read data from FIFO
 *      If there is an adjustment, an adjustment flag is added
 *****************************************************************************/
static vcHr02Ret_t vcHr02OutPutPPGData(vcHr02_t *pvcHr02, uint8_t StartIndex, uint16_t StopIndex,
                                       uint8_t IndexFlag)
{
    uint16_t i = 0;
    uint8_t dataLength = 0;
    uint8_t sampleData[128];
    vcHr02Ret_t ret = VCHR02RET_ISOK;

    dataLength = StopIndex - StartIndex;
    ret |= vcHr02ReadRegisters(StartIndex, sampleData, dataLength);

    for (i = StartIndex; i < StopIndex; i = i + 2)
    {
        pvcHr02->sampleData.ppgValue[(i - StartIndex) / 2 + IndexFlag] = ((sampleData[(i - StartIndex)] <<
                                                                           8) | sampleData[(i - StartIndex) + 1]);
        slotCount = ((i - 0x80) % (totalSlot * 2)) / 2;
        sampleIndex = (i - 0x80) / (totalSlot * 2);
        if (adjustFlag[slotCount][sampleIndex] == 1)
        {
            pvcHr02->sampleData.ppgValue[(i - StartIndex) / 2 + IndexFlag] |= 0x1000;
            adjustFlag[slotCount][sampleIndex] = 0;
        }
    }
    return ret;
}

/******************************************************************************
 * @brief
 *      Clock Calibration
 *****************************************************************************/
uint16_t vcHr02CalculateOSCFreq(vcHr02_t *pvcHr02)
{
    uint8_t clkAdjCnt[4] = {0}, newFreDiv[2] = {0}, i = 0;
    uint32_t rtcCount[2];
    uint16_t adjCount[2], freDiv[2] = {0};
    uint16_t newDiv, f20kOsc = 0;
    bool diffCountIsOk = true;
    bool diffIsOK;

    /* Get McuRtcCount */
    rtcCount[0] = vcHr02GetRtcCountFromMCU();
    vcHr02ReadRegisters(VCREG7, clkAdjCnt, 4);
    /* Get McuRtcCount */
    rtcCount[1] = vcHr02GetRtcCountFromMCU();

    adjCount[0] = clkAdjCnt[0] << 8 | clkAdjCnt[1];
    adjCount[1] = clkAdjCnt[2] << 8 | clkAdjCnt[3];
    if (adjCount[1] < adjCount[0] || rtcCount[1] < rtcCount[0])
    {
        cntCalOsc = 0;
        return 0;
    }

    intDiffAdj[2 * cntCalOsc] = adjCount[0];
    intDiffAdj[2 * cntCalOsc + 1] = adjCount[1];
    intDiffRtc[2 * cntCalOsc] = rtcCount[0];
    intDiffRtc[2 * cntCalOsc + 1] = rtcCount[1];
    cntCalOsc += 1;
    if (cntCalOsc >= 3)
    {
        cntCalOsc = 0;
        for (i = 0; i < 2; i++)
        {
            diffCountIsOk = (((intDiffRtc[2 * i + 2] - intDiffRtc[2 * i]) - (intDiffRtc[2 * i + 3] -
                                                                             intDiffRtc[2 * i + 1]) <= oscDifSetting) ||
                             ((intDiffRtc[2 * i + 3] - intDiffRtc[2 * i + 1]) - (intDiffRtc[2 * i + 2] - intDiffRtc[2 * i]) <=
                              oscDifSetting)) && diffCountIsOk;
            diffCountIsOk = (((intDiffAdj[2 * i + 2] - intDiffAdj[2 * i]) - (intDiffAdj[2 * i + 3] -
                                                                             intDiffAdj[2 * i + 1]) <= 2) ||
                             ((intDiffAdj[2 * i + 3] - intDiffAdj[2 * i + 1]) - (intDiffAdj[2 * i + 2] - intDiffAdj[2 * i]) <=
                              2)) && diffCountIsOk;
            diffCountIsOk = (!(intDiffRtc[2 * i + 2] == intDiffRtc[2 * i])) &&
                            diffCountIsOk; /* avoid divisor==0, diffCountIsOk set false */
        }
        if (diffCountIsOk)
        {
            for (i = 0; i < 2; i++)
            {
                freDiv[i] = ((uint32_t)(intDiffAdj[2 * i + 2] - intDiffAdj[2 * i])) * pvcHr02->mcuOscValue /
                            pvcHr02->vcSampleRate / ((uint32_t)(intDiffRtc[2 * i + 2] - intDiffRtc[2 * i])) - 1;
                if (freDiv[i] < (20000 / pvcHr02->vcSampleRate * 0.75f) ||
                    freDiv[i] > (20000 / pvcHr02->vcSampleRate * 1.25f))
                {
                    return 0;
                }
            }
            diffIsOK = (freDiv[1] - freDiv[0] <= 20) || (freDiv[0] - freDiv[1] <= 20);
            if (diffIsOK)
            {
                newDiv = (freDiv[1] + freDiv[0]) / 2;
                f20kOsc = (newDiv + 1) * pvcHr02->vcSampleRate;
                newFreDiv[0] = newDiv >> 8;
                newFreDiv[1] = newDiv;
                vcHr02WriteRegisters(VCREG15, newFreDiv, VCREG15_SIZE);
                pvcHr02->oscCheckFinishFlag = 1;
                oscCnt = 0;
            }
        }
        else
        {
            return 0;
        }
    }
    return f20kOsc;
}

/*****************************************************************************
 * @brief
 *      Wear monitoring.
 *
 *****************************************************************************/
static vcHr02Ret_t vcHr02WearStatusDetection(vcHr02_t *pvcHr02, bool onlyCheckWear)
{
    vcHr02Ret_t ret = VCHR02RET_ISOK;
    g_stBioParam.workMode = pvcHr02->workMode;
    if (pvcHr02->workMode == VCWORK_MODE_WEAR_BIOMETRICS)
    {
        g_stBioParam.isBioMode = 1;
    }
    else
    {
        g_stBioParam.isBioMode = 0;
    }
    g_stBioParam.irqType = pvcHr02->intReason;
    g_stBioParam.enableFun = pvcHr02->regConfig[0];
    g_stBioParam.pi[0] = pvcHr02->sampleData.currentValue[0];
    g_stBioParam.pi[1] = pvcHr02->sampleData.currentValue[1];
    g_stBioParam.prl[0] = adjustResLvL[0];
    g_stBioParam.prl[1] = adjustResLvL[1];
    g_stBioParam.pg[0] = pvcHr02->sampleData.preValue[0];
    g_stBioParam.pg[1] = pvcHr02->sampleData.preValue[1];
    g_stBioParam.env = pvcHr02->sampleData.envValue[2];
    g_stBioParam.ps = pvcHr02->sampleData.psValue;
    g_stBioParam.BioVal = pvcHr02->sampleData.bioVal;
    /* 20221202 gsensor_heart-1 wear status */
#if (ENABLE_GSENSOR_WEAR)
    g_stBioParam.gsensor_hold_sta = g_gsensor_hold_sta;
#if (ENABLE_HR_ALGO_WEAR)
    g_stBioParam.hr_algo_drop_sta = g_hr_algo_drop_sta;
#else
    g_stBioParam.hr_algo_drop_sta = BIO_WEAR_ISWEAR;
#endif
#else
    g_stBioParam.gsensor_hold_sta = BIO_WEAR_ISWEAR;
    g_stBioParam.hr_algo_drop_sta = BIO_WEAR_ISWEAR;
#endif
    BioWearRet_e bioRet = bio_get_wear_status(&g_stBioParam, onlyCheckWear);
    extern int bio_get_edge_result(void);
    extern unsigned short int bio_get_calc_result(void);
    if (bio_get_bio_finished_status())
    {
        switch (g_stBioParam.wearStatus)
        {
        case BIO_WEAR_NULL:
            break;
        case BIO_WEAR_UNWEAR:
            pvcHr02->wearStatus = VCHR02WEARST_UNWEAR;
            break;
        case BIO_WEAR_ISWEAR:
            pvcHr02->wearStatus = VCHR02WEARST_ISWEAR;
            break;
        default:
            break;
        }
        lastWearStatus = pvcHr02->wearStatus;
        if (bioRet == BIORET_ISWEAR_TO_UNWEAR)
        {
            ret = VCHR02RET_ISWEARTOUNWEAR;
        }
        else if (bioRet == BIORET_UNWEAR_TO_ISWEAR)
        {
            ret = VCHR02RET_UNWEARTOISWEAR;
        }
    }
    else
    {
        pvcHr02->wearStatus = lastWearStatus;
    }

    return ret;
}

/****************************************************************************
 * @brief
 *      FIFO Read.(Or GetValue)
 *
 *****************************************************************************/
static vcHr02Ret_t vcHr02ReadFifo(vcHr02_t *pvcHr02, uint8_t *len)
{
    uint32_t rtcValue = 0;
    uint8_t dateLength = 0;
    float cacheTime = 64.0f;
    vcHr02Ret_t ret = VCHR02RET_ISOK;
    ret |= vcHr02ReadRegisters(VCREG3, (uint8_t *)&fifoWriteIndex, VCREG3_SIZE);
    /*
    uint32_t vcHr02TimeDiff;
    rtcValue = vcHr02GetRtcCountFromMCU();
    if (rtcValue > lastRtcValue)
    {
        vcHr02TimeDiff = rtcValue - lastRtcValue;
    }
    else
    {
        vcHr02TimeDiff = (0xFFFFFF - lastRtcValue) + rtcValue;
    }
    if((vcHr02TimeDiff)>(((cacheTime/pvcHr02->vcSampleRate) *0.8f* pvcHr02->mcuOscValue)/ totalSlot))
    */
    rtcValue = vcHr02GetRtcCountFromMCU();
    if ((rtcValue - lastRtcValue) > (((cacheTime / pvcHr02->vcSampleRate) * 0.8f *
                                      pvcHr02->mcuOscValue) / totalSlot))
    {
        pvcHr02->vcFifoReadFlag = 0;
        fifoWriteIndex = 0;
        fifoReadIndex = 0x80;
        lastRtcValue = rtcValue;
        return VCHR02RET_FIFOISOVERFLOW;
    }
    /* check fifoReadIndex usable */
    if (fifoWriteIndex < 0x80)
    {
        pvcHr02->vcFifoReadFlag = 0;

        fifoReadIndex = fifoReadIndex + (fifoIntDiv + 1) * 2;
        if (fifoReadIndex < 128)
        {
            fifoReadIndex += 0X80;
        }
        lastRtcValue = rtcValue;
        return VCHR02RET_FIFOISOVERFLOW;
    }
    if (fifoIntDiv != 0)
    {
        if (fifoWriteIndex > fifoReadIndex)
        {
            dateLength = (fifoWriteIndex - fifoReadIndex) / 2;
            vcHr02OutPutPPGData(pvcHr02, fifoReadIndex, fifoWriteIndex, 0);
        }
        else
        {
            dateLength = (fifoWriteIndex + 0x80 - fifoReadIndex) / 2;
            if (dateLength == 64)
            {
                dateLength = 0;
                pvcHr02->vcFifoReadFlag = 0;
                return VCHR02RET_ISERR;
            }
            vcHr02OutPutPPGData(pvcHr02, fifoReadIndex, 256, 0);
            if (fifoWriteIndex != 0x80)
            {
                vcHr02OutPutPPGData(pvcHr02, 0x80, fifoWriteIndex, (256 - fifoReadIndex) / 2);
            }
        }
        fifoReadIndex = fifoWriteIndex;
    }
    else
    {
        /* Not use FIFO, the sampleData is storage in 0x80 */
        dateLength = totalSlot;
        vcHr02OutPutPPGData(pvcHr02, fifoReadIndex, fifoReadIndex + totalSlot * 2, 0);
    }
    /* Return the array length for data processing */
    *len = dateLength;
    lastRtcValue = rtcValue;
    return ret;
}

/****************************************************************************
 * @brief
 *      Adjust PDRes Value and modify the PDResMax
 *
 *****************************************************************************/
static vcHr02Ret_t vcHr02EnvAdjust(vcHr02_t *pvcHr02, uint8_t slotNum)
{
    vcHr02Ret_t ret = VCHR02RET_ISOK;
    if ((pvcHr02->sampleData.envValue[slotNum] > VCENV_TH) && enSlot[slotNum])
    {
        recordEnv[slotNum] = pvcHr02->sampleData.envValue[slotNum];
        if (adjustResLvL[slotNum] > 0)
        {
            adjustResLvL[slotNum]--;
            resMaxLvL[slotNum] = adjustResLvL[slotNum];
            /* Added logical adjustment flag bit */
            vcHr02AddAdjustFlag(slotNum);
            adjustResP_t[slotNum] = resLv2resReg(adjustResLvL[slotNum]);
            pvcHr02->regConfig[slotNum + 10] = (adjustResP_t[slotNum].resVal << 4) | pdResSet[slotNum];
            ret |= vcHr02WriteRegisters((VCREG20 + slotNum), (uint8_t *) & (pvcHr02->regConfig[slotNum + 10]),
                                        1);
        }
        else
        {
            return VCHR02RET_ENVCURRENTISLARGE;
        }
    }
    if (pvcHr02->sampleData.envValue[2] > VCENV_TH)
    {
        return VCHR02RET_PSENVCURRENTISLARGE;
    }
    return ret;
}

/*****************************************************************************
 * @brief
 *      Adjust ledCurrent Value and modify the ledCurrentMax value
 *
 *****************************************************************************/
static vcHr02Ret_t vcHr02OvloadAdjust(vcHr02_t *pvcHr02, uint8_t slotNum)
{
    uint8_t overLoad = 0;
    uint8_t ovload[SLOTCHNU];
    vcHr02Ret_t ret = VCHR02RET_ISOK;

    ret |= vcHr02ReadRegisters(VCREG1, (uint8_t *)&overLoad, VCREG1_SIZE);

    /* Get Slot0,Slot1 and Slot2's Overload status */
    ovload[0] = overLoad & 0x01;
    ovload[1] = (overLoad & 0x02) >> 1;
    ovload[2] = (overLoad & 0x04) >> 2;

    /* Adjust led emitter and record max_led_emitter */
    if ((overLoad & (pvcHr02->regConfig[0] & 0x07)) != 0)
    {
        if (ovload[slotNum] && enSlot[slotNum])
        {
            if (ledCurrent[slotNum] != 0)
            {
                ledCurrent[slotNum] = ledCurrent[slotNum] - 1;
                ledMaxCurrent[slotNum] = ledCurrent[slotNum];
                vcHr02AddAdjustFlag(slotNum);
                pvcHr02->regConfig[slotNum + 7] = (ledCurrent[slotNum] | ppgGain[slotNum]);
                ret |= vcHr02WriteRegisters((VCREG17 + slotNum), (uint8_t *) & (pvcHr02->regConfig[slotNum + 7]),
                                            1);
            }
            else
            {
                return VCHR02RET_SOLTISOVERLOAD;
            }
        }
        /* After the calibration of infrared channel parameters,
        the current and resistance will not be adjusted */
        if (ovload[2])
        {
            return VCHR02RET_PSISOVERLOAD;
        }
    }
    return ret;
}

/*****************************************************************************
 * @brief Adjust ledCurrent Value and PDRes value
 *              "lowRes-offset-res" equivalent to "resLv"
 *        "lowRes-offset-res" equivalent to "resLv"
 *****************************************************************************/
static AdRes_t resLv2resReg(uint8_t resLv)
{
    AdRes_t ret_t;
    memset(&ret_t, 0x0, sizeof(ret_t)); //add by wmm for cppcheck
    if ((FoundryID == VC013_D_NORM_ID) || (FoundryID == VC015_E_NORM_ID) ||
        (FoundryID == VC018_D_NORM_ID) || (FoundryID == VC018_E_NORM_ID))
    {
        if (resLv > 17)
        {
            return ret_t;
        }
        if (resLv >= 16)
        {
            ret_t.lowRes = 0;
            ret_t.offset = 2;
            ret_t.resVal = resLv - 10;
        }
        else if (resLv >= 14)
        {
            ret_t.lowRes = 0;
            ret_t.offset = 1;
            ret_t.resVal = resLv - 8;
        }
        else if (resLv >= 6)
        {
            ret_t.lowRes = 0;
            ret_t.offset = 0;
            ret_t.resVal = resLv - 6;
        }
        else
        {
            ret_t.lowRes = 1;
            ret_t.offset = 0;
            ret_t.resVal = resLv;
        }
    }
    else
    {
        ret_t.resVal = resLv;
        ret_t.lowRes = 0;
        ret_t.offset = 0;
    }
    return ret_t;
}
static uint8_t resReg2resLv(AdRes_t adRes_t)
{
    uint8_t resLv;
    if ((FoundryID == VC013_D_NORM_ID) || (FoundryID == VC015_E_NORM_ID) ||
        (FoundryID == VC018_D_NORM_ID) || (FoundryID == VC018_E_NORM_ID))
    {
        switch (adRes_t.offset)
        {
        case 2:
            if (adRes_t.lowRes != 0)
            {
                break;
            }
            resLv = adRes_t.resVal + 10;
            break;
        case 1:
            if (adRes_t.lowRes != 0)
            {
                break;
            }
            resLv = adRes_t.resVal + 8;
            break;
        case 0:
            if (adRes_t.lowRes == 0)
            {
                resLv = adRes_t.resVal + 6;
            }
            else
            {
                resLv = adRes_t.resVal;
            }
            break;
        default:
            break;
        }
    }
    else
    {
        resLv = adRes_t.resVal;
    }
    return resLv;
}

/*****************************************************************************
 * @brief
 *      adjust current function for vp35,2021.9.28. reserved.
 *
 *****************************************************************************/
#define CUR_STEP_VAILD_RANG (300) /* (-x,x) */
#define CUR_STEP_OFFSET 400
#define REDUCE_CUR_STEP_MIN_H (2047 - CUR_STEP_OFFSET + CUR_STEP_VAILD_RANG)
#define REDUCE_CUR_STEP_MIN_L (2047 - CUR_STEP_OFFSET - CUR_STEP_VAILD_RANG)
#define REDUCE_CUR_STEP_MAX_H (2047 + CUR_STEP_OFFSET + CUR_STEP_VAILD_RANG)
#define REDUCE_CUR_STEP_MAX_L (2047 + CUR_STEP_OFFSET - CUR_STEP_VAILD_RANG)
uint16_t firstPPG[2];
#if 0
static void vcHr02_calibration_adjust_current_step(vcHr02_t *pvcHr02)
{
    uint8_t ppdData[2];
    for (uint8_t ch_n = 0; ch_n < 2; ch_n++)
    {
        if (pvcHr02->adjustInfo[ch_n].lastIsAdjustCur)
        {
            pvcHr02->adjustInfo[ch_n].lastIsAdjustCur = false;
            uint8_t addr;
            if (enFifo)
            {
                addr = pvcHr02->adjustInfo[ch_n].lastAdjustCurAddr + 2 * totalSlot;
            }
            else
            {
                addr = pvcHr02->adjustInfo[ch_n].lastAdjustCurAddr;
            }
            if (addr < 0x80)
            {
                addr += 0x80;
            }
            vcHr02ReadRegisters(addr, ppdData, 2);
            firstPPG[ch_n] = ((uint16_t)ppdData[0] << 8) + ppdData[1];

            if (pvcHr02->adjustInfo[ch_n].directionStep == VCHR02ADJUSTDIRECTION_UP)
            {
                /* UP step reduce */
                if (firstPPG[ch_n] >= REDUCE_CUR_STEP_MAX_H)
                {
                    pvcHr02->adjustInfo[ch_n].step = ((pvcHr02->adjustInfo[ch_n].step + 1) >= 32) ? 32 :
                                                     (pvcHr02->adjustInfo[ch_n].step + 1);
                }
                else if (firstPPG[ch_n] <= REDUCE_CUR_STEP_MAX_L)
                {
                    pvcHr02->adjustInfo[ch_n].step = (pvcHr02->adjustInfo[ch_n].step > 1) ?
                                                     (pvcHr02->adjustInfo[ch_n].step - 1) : 1;
                }
            }
            else if (pvcHr02->adjustInfo[ch_n].directionStep == VCHR02ADJUSTDIRECTION_DOWN)
            {
                /* DOWN step reduce */
                if (firstPPG[ch_n] >= REDUCE_CUR_STEP_MIN_H)
                {
                    pvcHr02->adjustInfo[ch_n].step = (pvcHr02->adjustInfo[ch_n].step > 1) ?
                                                     (pvcHr02->adjustInfo[ch_n].step - 1) : 1;
                }
                else if (firstPPG[ch_n] <= REDUCE_CUR_STEP_MIN_L)
                {
                    pvcHr02->adjustInfo[ch_n].step = ((pvcHr02->adjustInfo[ch_n].step + 1) >= 32) ? 32 :
                                                     (pvcHr02->adjustInfo[ch_n].step + 1);
                }
            }
//          else
//          {
//              DEBUG_LOG(" direction = NULL ");
//          }
            pvcHr02->adjustInfo[ch_n].directionStep = VCHR02ADJUSTDIRECTION_NULL;
        }
    } /* end of for(chn;;)  */
}

#endif
/*****************************************************************************
 * @brief
 *      Adjust err deal,while resLvL is too large
 *
 *****************************************************************************/
static bool vcHr02AdjustErrDealWithResLarge(vcHr02AdjustDirection dir, uint8_t resLvL, uint8_t cur)
{
    static vcHr02AdjustDirection lastDir = VCHR02ADJUSTDIRECTION_NULL;
    static uint8_t lastResL = 0;
    static uint8_t lastCur[3] = {0};
    static uint8_t logCnt = 0;
    if (lastDir != dir && lastResL == resLvL)
    {
        lastCur[0] = lastCur[1];
        lastCur[1] = lastCur[2];
        lastCur[2] = cur;
        if (lastCur[0] == lastCur[2])
        {
            logCnt++;
        }
    }
    else
    {
        logCnt = 0;
    }
    lastDir = dir;
    lastResL = resLvL;
    if (logCnt >= 5)
    {
        logCnt = 0;
        return true;
    }
    else
    {
        return false;
    }
}

/*****************************************************************************
 * @brief
 *      Adjust ledCurrent Value and PDRes value
 *
 *****************************************************************************/
static vcHr02Ret_t vcHr02PPgLedCurAndPdResAdjust(vcHr02_t *pvcHr02, uint8_t slotNum, uint8_t ledcur,
                                                 uint8_t resLvL)
{
//  uint8_t reg_read_data;
    uint8_t reg_write_data;

    uint8_t oldPdResLvL, oldLedCurrent;
    uint8_t newPdResLvL, newLedCurrent;
    vcHr02Ret_t ret = VCHR02RET_ISOK;

    oldLedCurrent = ledCurrent[slotNum];
    newLedCurrent = oldLedCurrent;
    oldPdResLvL = adjustResLvL[slotNum];
    newPdResLvL = oldPdResLvL;

//  slot0EnvIsExceedFlag = false;
//  slot1EnvIsExceedFlag = false;
    /* adjust res */
    if ((oldLedCurrent == ledcur) && (pvcHr02->workMode != VCWORK_MODE_HR_NOISEWORK))
    {
        if (oldPdResLvL == resLvL)
        {
            newPdResLvL = oldPdResLvL;
//          slot0EnvIsExceedFlag = (slotNum == 0) ? true : false;
//          slot1EnvIsExceedFlag = (slotNum == 1) ? true : false;
            /* BIO MODE dont adjust,set mark */
            ADJUST_FAIL_MARK();
            return VCHR02RET_PPGCANNOTADJUSTABLE;
        }
        else /*adjust res */
        {
            if (pvcHr02->adjustInfo[slotNum].direction == VCHR02ADJUSTDIRECTION_UP)
            {
                newPdResLvL = (oldPdResLvL >= constResMaxLvL) ? constResMaxLvL : (oldPdResLvL + 1);
            }
            else
            {
                newPdResLvL = (oldPdResLvL < 1) ? 0 : (oldPdResLvL - 1);
            }
            newLedCurrent = oldLedCurrent;
            vcHr02AddAdjustFlag(slotNum);
        }
    }
    else /* adjust cur */
    {
        if (pvcHr02->adjustInfo[slotNum].directionLast == VCHR02ADJUSTDIRECTION_NULL)
        {
            pvcHr02->adjustInfo[slotNum].step *= 16;
        }
        else if (pvcHr02->adjustInfo[slotNum].direction == pvcHr02->adjustInfo[slotNum].directionLast)
        {
            if ((pvcHr02->adjustInfo[slotNum].step == 1) || (pvcHr02->adjustInfo[slotNum].step == 2))
            {
                pvcHr02->adjustInfo[slotNum].step = (pvcHr02->adjustInfo[slotNum].step + 1) * 16;
            }
            else
            {
                pvcHr02->adjustInfo[slotNum].step *= VCADJUST_INCREASE;
            }
        }
        else
        {
            pvcHr02->adjustInfo[slotNum].step *= VCADJUST_DECREASE;
        }
        pvcHr02->adjustInfo[slotNum].step = pvcHr02->adjustInfo[slotNum].step >> 4;

        pvcHr02->adjustInfo[slotNum].step = (pvcHr02->adjustInfo[slotNum].step <= VCADJUST_STEP_MIN) ?
                                            VCADJUST_STEP_MIN : pvcHr02->adjustInfo[slotNum].step;
        pvcHr02->adjustInfo[slotNum].step = (pvcHr02->adjustInfo[slotNum].step >= VCADJUST_STEP_MAX) ?
                                            VCADJUST_STEP_MAX : pvcHr02->adjustInfo[slotNum].step;

        if (pvcHr02->adjustInfo[slotNum].direction == VCHR02ADJUSTDIRECTION_UP)
        {
            newLedCurrent = ((oldLedCurrent + pvcHr02->adjustInfo[slotNum].step) > ledcur) ? ledcur :
                            oldLedCurrent + pvcHr02->adjustInfo[slotNum].step;
        }
        else
        {
            newLedCurrent = (oldLedCurrent < pvcHr02->adjustInfo[slotNum].step) ? ledcur : oldLedCurrent -
                            pvcHr02->adjustInfo[slotNum].step;
        }
        vcHr02AddAdjustFlag(slotNum);
        newPdResLvL = oldPdResLvL;
        pvcHr02->adjustInfo[slotNum].directionLast = pvcHr02->adjustInfo[slotNum].direction;
        /* data logging for vp35 */
        pvcHr02->adjustInfo[slotNum].directionStep = pvcHr02->adjustInfo[slotNum].direction;
        pvcHr02->adjustInfo[slotNum].lastIsAdjustCur = true;
    }
    if (vcHr02AdjustErrDealWithResLarge(pvcHr02->adjustInfo[slotNum].direction, newPdResLvL,
                                        newLedCurrent))
    {
        if (newPdResLvL >= 13)
        {
            newPdResLvL--;
        }
    }
    /* adjust is finshed */
    ledCurrent[slotNum] = newLedCurrent;
    adjustResLvL[slotNum] = newPdResLvL;
    adjustResP_t[slotNum] = resLv2resReg(adjustResLvL[slotNum]);

    if ((FoundryID == VC013_D_NORM_ID) || (FoundryID == VC015_E_NORM_ID) ||
        (FoundryID == VC018_D_NORM_ID) || (FoundryID == VC018_E_NORM_ID))
    {
        reg_write_data = ((adjustResP_t[1].offset << 6) | (adjustResP_t[0].offset << 4) | (envBias) |
                          (adjustResP_t[2].lowRes << 2) | (adjustResP_t[1].lowRes << 1) | (adjustResP_t[0].lowRes));
        ret |= vcHr02WriteRegisters(VCREG28, (uint8_t *)&reg_write_data, 1);
    }
    pvcHr02->regConfig[slotNum + 7] = ledCurrent[slotNum] | ppgGain[slotNum];
    ret |= vcHr02WriteRegisters((VCREG17 + slotNum), (uint8_t *) & (pvcHr02->regConfig[slotNum + 7]),
                                1);
    pvcHr02->regConfig[slotNum + 10] = (adjustResP_t[slotNum].resVal << 4) | pdResSet[slotNum];
    ret |= vcHr02WriteRegisters((VCREG20 + slotNum), (uint8_t *) & (pvcHr02->regConfig[slotNum + 10]),
                                1);

    return ret;
}

/*****************************************************************************
 * @brief
 *      Get the PPG saturation direction
 *
 *****************************************************************************/
static vcHr02Ret_t vcHr02PpgAdjust(vcHr02_t *pvcHr02, uint8_t slotNum)
{
    uint8_t ppgDataAddress, ppgDataAddress_EnFifo;
    uint8_t ppgDate[2];
    uint16_t ppgCurValue[2];
    uint8_t IndexGap;
    vcHr02Ret_t ret = VCHR02RET_ISOK;

    /* Get fifoWriteIndex Value */
    ret |= vcHr02ReadRegisters(VCREG3, (uint8_t *)&fifoWriteIndex, VCREG3_SIZE);
    if (totalSlot == 1)
    {
        /* Determine whether the FIFO is overflowing */
        ppgDataAddress_EnFifo = fifoWriteIndex - totalSlot * 2;
        ppgDataAddress_EnFifo = ppgDataAddress_EnFifo < 0x80 ? ppgDataAddress_EnFifo + 0x80 :
                                ppgDataAddress_EnFifo;
        ppgDataAddress = enFifo ? ppgDataAddress_EnFifo : 0x80;
    }
    else
    {
        /* Determine whether the FIFO is overflowing */
        ppgDataAddress_EnFifo = fifoWriteIndex - totalSlot * 2 + slotNum * 2;
        ppgDataAddress_EnFifo = ppgDataAddress_EnFifo < 0x80 ? ppgDataAddress_EnFifo + 0x80 :
                                ppgDataAddress_EnFifo;
        ppgDataAddress = enFifo ? ppgDataAddress_EnFifo : 0x80 + slotNum * 2;
    }
    ret |= vcHr02ReadRegisters(ppgDataAddress, ppgDate, 2);
    ppgCurValue[slotNum] = (ppgDate[0] << 8) | ppgDate[1];
    if (fifoIntDiv != 0)
    {
        IndexGap = (fifoWriteIndex > readIndex) ? (fifoWriteIndex - readIndex) :
                   (fifoWriteIndex - readIndex + 0x80);
        if (IndexGap != 2 * totalSlot)
        {
            pvcHr02->adjustInfo[slotNum].directionLast = VCHR02ADJUSTDIRECTION_NULL;
        }
        readCnt += 1;
        if (readCnt == totalSlot)
        {
            readCnt = 0;
            readIndex = fifoWriteIndex;
        }
    }
    /* PPG supersaturation */
    if (ppgCurValue[slotNum] > 4095 - VCPPG_TH * 32) // 32
    {
        /* If the LED luminous current reaches the maximum current,
        it can only be adjusted by increasing the PD resistance */
        pvcHr02->adjustInfo[slotNum].lastAdjustCurAddr = ppgDataAddress;
        pvcHr02->adjustInfo[slotNum].direction = VCHR02ADJUSTDIRECTION_UP;
        ret |= vcHr02PPgLedCurAndPdResAdjust(pvcHr02, slotNum, ledMaxCurrent[slotNum], resMaxLvL[slotNum]);
    }
    /* Lower saturation */
    else if (ppgCurValue[slotNum] < VCPPG_TH * 32) // 32
    {
        pvcHr02->adjustInfo[slotNum].lastAdjustCurAddr = ppgDataAddress;
        pvcHr02->adjustInfo[slotNum].direction = VCHR02ADJUSTDIRECTION_DOWN;
        ret |= vcHr02PPgLedCurAndPdResAdjust(pvcHr02, slotNum, 0, 0);
    }
    return ret;
}

/*****************************************************************************
 * @brief
 *      Prejude the ENVvalue and modify the PD maximum resistance.
 *      Don't need to write to register
 *
 *****************************************************************************/
static vcHr02Ret_t vcHr02AdjustPDResMax(vcHr02_t *pvcHr02, uint8_t slotNum)
{
    uint8_t minEnv[SLOTCHNU];
    vcHr02Ret_t ret = VCHR02RET_ISOK;

    if (resMaxLvL[slotNum] == constResMaxLvL)
    {
        return VCHR02RET_PDRESISMAX;
    }
    minEnv[slotNum] = ((recordEnv[slotNum] / 2) < VCENV_TH) ? (recordEnv[slotNum] / 2) : VCENV_TH;
    if (pvcHr02->sampleData.envValue[slotNum] < minEnv[slotNum])
    {
        if (pvcHr02->sampleData.envValue[slotNum] < 1)
        {
            count_pdRes[slotNum] = 0;
            count_QaPdRes[slotNum] = (count_QaPdRes[slotNum] == 30 * pvcHr02->vcSampleRate) ? 0 :
                                     count_QaPdRes[slotNum] + 1;
            resMaxLvL[slotNum] = (count_QaPdRes[slotNum] == 30 * pvcHr02->vcSampleRate) ? constResMaxLvL :
                                 resMaxLvL[slotNum];
        }
        else
        {
            count_QaPdRes[slotNum] = 0;
            count_pdRes[slotNum] += 1;
            resMaxLvL[slotNum] = (count_pdRes[slotNum] == 120 * pvcHr02->vcSampleRate) ? resMaxLvL[slotNum] +
                                 1 : resMaxLvL[slotNum];
        }
    }
    else
    {
        count_QaPdRes[slotNum] = 0;
        count_pdRes[slotNum] = 0;
    }
    return ret;
}

/****************************************************************************
 * @brief
 *      Before getsample value,prejude Status.
 *
 *****************************************************************************/
static vcHr02Ret_t vcHr02ReadStatus(vcHr02_t *pvcHr02, bool checkWear)
{
    uint8_t vcHr02SlotCnt = 0;
    vcHr02Ret_t ret = VCHR02RET_ISOK;
    /* Clear FifoReadFlag */
    pvcHr02->vcFifoReadFlag = 0;
    pvcHr02->vcPsFlag = 0;
    if ((pvcHr02->regConfig[1] & 0x10) && (pvcHr02->intReason & 0x10))
    {
        pvcHr02->vcPsFlag = 1;
    }
    if (pvcHr02->vcHr02Status & VCREG1_CONFLICT)
    {
        return VCHR02RET_ISCONFLICT;
    }
    if (pvcHr02->vcHr02Status & VCREG1_INSAMPLE)
    {
        return VCHR02RET_ISINSAMPLE;
    }
    if (pvcHr02->regConfig[1] & 0x10)
    {
        if (pvcHr02->intReason & 0x10)
        {
            ret |= vcHr02WearStatusDetection(pvcHr02, checkWear);
        }
    }
    for (vcHr02SlotCnt = 0; vcHr02SlotCnt < totalSlot; vcHr02SlotCnt++)
    {
        slotCount = (totalSlot == 2) ? vcHr02SlotCnt : (enSlot[0] ? 0 :
                                                        1); /* slotCount=ENABLE CHANEL SINGNAL */
        vcHr02AdjustPDResMax(pvcHr02, slotCount);
        ledCurrent[slotCount] = pvcHr02->regConfig[7 + slotCount] & 0x7f;
        ppgGain[slotCount] = pvcHr02->regConfig[7 + slotCount] & 0x80;
        adjustResP_t[slotCount].resVal = (pvcHr02->regConfig[10 + slotCount] & 0x70) >> 4;
        pdResSet[slotCount] = pvcHr02->regConfig[10 + slotCount] & 0x8F;
        if (pvcHr02->intReason & VCREG1_INTENV)
        {
            ret |= vcHr02EnvAdjust(pvcHr02, slotCount);
        }
        if (pvcHr02->intReason & VCREG1_INTOV)
        {
            ret |= vcHr02OvloadAdjust(pvcHr02, slotCount);
        }
        if (pvcHr02->intReason & VCREG1_INTPPG)
        {
            ret |= vcHr02PpgAdjust(pvcHr02, slotCount);
        }
        else
        {
            pvcHr02->adjustInfo[slotCount].directionLast = VCHR02ADJUSTDIRECTION_NULL;
            pvcHr02->adjustInfo[slotCount].direction = VCHR02ADJUSTDIRECTION_NULL;
        }
    }
    if (pvcHr02->intReason & VCREG1_INTFIFO)
    {
        if (pvcHr02->workMode == VCWORK_MODE_SPO2WORK)
        {
            ret |= vcHr02WearStatusDetection(pvcHr02, checkWear);
            pvcHr02->vcPsFlag = 1;
        }
        pvcHr02->vcFifoReadFlag = 1;
    }
    return ret;
}

/*****************************************************************************
 * @brief
 *      Read Pre bio and EnvValue
 *
 *****************************************************************************/
static vcHr02Ret_t vcHr02ReadReg(vcHr02_t *pvcHr02)
{
    uint8_t vcRegisterValue[6] = {0};
    vcHr02Ret_t ret = VCHR02RET_ISOK;

    ret |= vcHr02ReadRegisters(VCREG1, vcRegisterValue, 6);

    pvcHr02->vcHr02Status = vcRegisterValue[0];
    pvcHr02->intReason = vcRegisterValue[1];

    pvcHr02->sampleData.envValue[0] = vcRegisterValue[3] >> 4;
    pvcHr02->sampleData.preValue[0] = vcRegisterValue[3] & 0x0F;
    pvcHr02->sampleData.envValue[1] = vcRegisterValue[4] >> 4;
    pvcHr02->sampleData.preValue[1] = vcRegisterValue[4] & 0x0F;
    pvcHr02->sampleData.envValue[2] = vcRegisterValue[5] >> 4;
    pvcHr02->sampleData.psValue = vcRegisterValue[5] & 0x0F;
    /* to code env + pre 4bit->8bit */
    if ((FoundryID == VC013_D_NORM_ID) || (FoundryID == VC018_D_NORM_ID) ||
        (FoundryID == VC015_E_NORM_ID) || (FoundryID == VC018_E_NORM_ID))
    {
#if !ENV_PRE_IS_4BIT
        uint8_t envPsReg[9] = {0};
        ret |= vcHr02ReadRegisters(VCREG33, envPsReg, 9);
        pvcHr02->sampleData.envValue[0] = envPsReg[0];
        pvcHr02->sampleData.preValue[0] = envPsReg[3];
        pvcHr02->sampleData.envValue[1] = envPsReg[1];
        pvcHr02->sampleData.preValue[1] = envPsReg[4];
        pvcHr02->sampleData.envValue[2] = envPsReg[2];
        pvcHr02->sampleData.psValue = envPsReg[5];

#endif
        uint8_t tmpLowRes;
        vcHr02ReadRegisters(VCREG28, &tmpLowRes, 1);
        adjustResP_t[0].lowRes = (tmpLowRes >> 0) & 0x01;
        adjustResP_t[0].offset = (tmpLowRes >> 4) & 0x03;
        adjustResP_t[1].lowRes = (tmpLowRes >> 1) & 0x01;
        adjustResP_t[1].offset = (tmpLowRes >> 6) & 0x03;
        adjustResP_t[2].lowRes = (tmpLowRes >> 2) & 0x01;
        adjustResP_t[2].offset = 0;
        envBias = (tmpLowRes & 0x08);
        if ((FoundryID == VC015_E_NORM_ID) || (FoundryID == VC018_E_NORM_ID))
        {
            uint8_t bioreg[2];
            vcHr02ReadRegisters(VCREG44, bioreg, VCREG44_SIZE);
            pvcHr02->sampleData.bioVal = ((uint16_t)bioreg[0] << 8) + bioreg[1];
        }
    }

    ret |= vcHr02ReadRegisters(VCREG17, vcRegisterValue, 6);
    pvcHr02->sampleData.pdResValue[0] = (vcRegisterValue[3] >> 4) & 0x07;
    pvcHr02->sampleData.currentValue[0] = vcRegisterValue[0] & 0x7F;
    pvcHr02->sampleData.pdResValue[1] = (vcRegisterValue[4] >> 4) & 0x07;
    pvcHr02->sampleData.currentValue[1] = vcRegisterValue[1] & 0x7F;
    pvcHr02->sampleData.pdResValue[2] = (vcRegisterValue[5] >> 4) & 0x07;
    pvcHr02->sampleData.currentValue[2] = vcRegisterValue[2] & 0x7F;

    /* resVal+lowRes -> resLvL */
    adjustResP_t[0].resVal = pvcHr02->sampleData.pdResValue[0];
    adjustResP_t[1].resVal = pvcHr02->sampleData.pdResValue[1];
    adjustResP_t[2].resVal = pvcHr02->sampleData.pdResValue[2];
    adjustResLvL[0] = resReg2resLv(adjustResP_t[0]);
    adjustResLvL[1] = resReg2resLv(adjustResP_t[1]);
    adjustResLvL[2] = resReg2resLv(adjustResP_t[2]);

    return ret;
}

/*****************************************************************************
 * @brief
 *       get sample result,contain wearStatus,adjust param,read fifo data
 *****************************************************************************/
vcHr02Ret_t vcHr02GetSampleValues(vcHr02_t *pvcHr02, uint8_t *DataLen)
{
    vcHr02Ret_t ret = VCHR02RET_ISOK;

    // int16_t secondCnt = pvcHr02->vcSampleRate*180/(fifoIntDiv+1);//3min
    int16_t secondCnt = pvcHr02->vcSampleRate * 10 / (fifoIntDiv + 1); // 10s
    vcHr02ReadReg(pvcHr02);

    switch (pvcHr02->workMode)
    {
    case VCWORK_MODE_HR100HZ:
    case VCWORK_MODE_HRWORK:
        ret |= vcHr02ReadStatus(pvcHr02, false);

        if (!pvcHr02->oscCheckFinishFlag)
        {
            vcHr02CalculateOSCFreq(pvcHr02);
        }
        if (pvcHr02->vcFifoReadFlag == 1)
        {
            oscCnt += 1;
            if (oscCnt >= secondCnt)
            {
                /* Consider the 20k Adjust to three mintures called */
                vcHr02CalculateOSCFreq(pvcHr02);
            }
            if (pvcHr02->oscCheckFinishFlag)
            {
#if (VCWEAR_READ_FIFO_MODE == VCWEAR_READ_FIFO_BY_INT)
                ret |= vcHr02ReadFifo(pvcHr02, DataLen);
#endif
            }
        }
        /* mutex fifoFlag_vsPsFlag */
        if (pvcHr02->vcFifoReadFlag)
        {
            pvcHr02->vcPsFlag = 0;
        }
        break;

    case VCWORK_MODE_SPO2WORK:
#if VP3X_ADJUST_CURT_STEP                       /*--- VP35_I_STEP_ADJ ---*/
        if (pvcHr02->intReason & VCREG1_INTPPG) // VCREG1_INTFIFO
        {
            vcHr02_calibration_adjust_current_step(pvcHr02);
        }
#endif
        ret |= vcHr02ReadStatus(pvcHr02, false);
        /* calc clk */
        if (!pvcHr02->oscCheckFinishFlag)
        {
            vcHr02CalculateOSCFreq(pvcHr02);
        }
        if (pvcHr02->vcFifoReadFlag == 1)
        {
            oscCnt += 1;
            if (oscCnt >= secondCnt)
            {
                /* Consider the 20k Adjust to three mintures called */
                vcHr02CalculateOSCFreq(pvcHr02);
            }
            if (pvcHr02->oscCheckFinishFlag)
            {
#if (VCWEAR_READ_FIFO_MODE == VCWEAR_READ_FIFO_BY_INT)
                ret |= vcHr02ReadFifo(pvcHr02, DataLen);
#endif
            }
        }
        /* mutex fifoFlag_vsPsFlag */
        // DEBUG_LOG("SPO2.flagfifo[%d]ps[%d]",pvcHr02->vcFifoReadFlag,pvcHr02->vcPsFlag);
        if (pvcHr02->vcFifoReadFlag)
        {
            pvcHr02->vcPsFlag = 0;
        }
        break;

    case VCWORK_MODE_CROSSTALKTEST:
        ledCurrent[0] = pvcHr02->regConfig[7] & 0x7f;
        if (pvcHr02->intReason & VCREG1_INTOV)
        {
            ret |= vcHr02OvloadAdjust(pvcHr02, 0);
        }
        pvcHr02->sampleData.maxLedCur = ledMaxCurrent[0];
        break;

    case VCWORK_MODE_HR_NOISEWORK:
        ret |= vcHr02ReadStatus(pvcHr02, false);
#if (VCWEAR_READ_FIFO_MODE == VCWEAR_READ_FIFO_BY_INT)
        if (pvcHr02->vcFifoReadFlag == 1)
        {
            ret |= vcHr02ReadFifo(pvcHr02, DataLen);
        }
#endif
        break;
    case VCWORK_MODE_LPDETECTION:
        if (!pvcHr02->oscCheckFinishFlag)
        {
            vcHr02CalculateOSCFreq(pvcHr02);
        }
        oscCnt += 1;
        if (oscCnt >= secondCnt)
        {
            /* Consider the 20k Adjust to five mintures called */
            vcHr02CalculateOSCFreq(pvcHr02);
        }
        ret |= vcHr02WearStatusDetection(pvcHr02, true);
        pvcHr02->vcPsFlag = 1;
        break;
    case VCWORK_MODE_WEAR_BIOMETRICS:
        ret |= vcHr02ReadStatus(pvcHr02, false);
        if (pvcHr02->vcFifoReadFlag)
        {
            pvcHr02->vcPsFlag = 0;
        }
        break;
    default:
        break;
    }
    return ret;
}

/*****************************************************************************
 * @brief
 *       timer callback.reserved.
 *****************************************************************************/
vcHr02Ret_t vcHr02TimeReadValues(vcHr02_t *pvcHr02, uint8_t *DataLen)
{
    vcHr02Ret_t ret = VCHR02RET_ISOK;

    /* Obtian the env and prevalue */
    vcHr02ReadReg(pvcHr02);
    ret |= vcHr02WearStatusDetection(pvcHr02, false);

    oscCnt += 1;
    if (oscCnt >= 180)
    {
        vcHr02CalculateOSCFreq(pvcHr02);
    }
    if (pvcHr02->oscCheckFinishFlag)
    {
        ret |= vcHr02ReadFifo(pvcHr02, DataLen);
    }
    return ret;
}

/*----------------------------------------------------------------------------------------------*/
/******************************************************************************
 * @brief
 *      disable sample slot,heart and spo2 slot
 *
 *****************************************************************************/
void vcHr02_bio_disable_sample_slot(void)
{
    extern vcHr02_t vcHr02;
    uint8_t enableSlot = 0;
    uint8_t psDiv1Hz = vcHr02.regConfig[3] & 0x3f;
    uint8_t enPs = (vcHr02.regConfig[1] | 0x10) & 0xf0;
    if (vcHr02.workMode == VCWORK_MODE_SPO2WORK)
    {
        vcHr02WriteRegisters(VCREG16, (uint8_t *)&psDiv1Hz, VCREG16_SIZE);
        vcHr02WriteRegisters(VCREG12, (uint8_t *)&enPs, VCREG12_SIZE);
        vcHr02.regConfig[1] = enPs;
    }
    vcHr02ReadRegisters(VCREG11, (uint8_t *)&enableSlot, VCREG11_SIZE);
    enableSlot = (enableSlot & 0xF8) | 0x04;
    vcHr02WriteRegisters(VCREG11, (uint8_t *)&enableSlot, VCREG11_SIZE);
}

static void vcHr02_bio_switch_regInit(vcHr02_t *pvcHr02)
{
    vcHr02StopSample(pvcHr02);
    vcHr02SoftReset(pvcHr02);
    vcHr02RegInit(pvcHr02, 1);
}

/******************************************************************************
 * @brief
 * 9202,Biometrics. config enter bio_gi measure mode and exit regain mode
 *
 *****************************************************************************/
void vcHr02_bio_switch_mode_init(BioSwitchMode_e scMode, unsigned char isDisablePPG)
{
    extern vcHr02_t vcHr02;
    vcHr02_t *pvcHr02 = &vcHr02;
    switch (scMode)
    {
    case BIO_SWITCH_NONE:
        pvcHr02->initDisablePpgFlag = false;
        vcHr02_bio_switch_regInit(pvcHr02);
        break;
    case BIO_SWITCH_ENTER:
        if (0 == g_isBioMode)
        {
            pvcHr02->initDisablePpgFlag = false;
            g_bioSwitchMode = pvcHr02->workMode;
            pvcHr02->workMode = VCWORK_MODE_WEAR_BIOMETRICS;
            g_isBioMode = 1;
            vcHr02_bio_switch_regInit(pvcHr02);
        }
//      else
//      {
//          DEBUG_LOG("BIO_SWITCH_ENTER FAIL[%d]->[%d]_%s", pvcHr02->workMode, VCWORK_MODE_WEAR_BIOMETRICS, __func__);
//      }
        break;
    case BIO_SWITCH_EXIT:
        if (1 == g_isBioMode)
        {
            pvcHr02->initDisablePpgFlag = (isDisablePPG ? true : false);
            pvcHr02->workMode = g_bioSwitchMode;
            g_isBioMode = 0;
            vcHr02_bio_switch_regInit(pvcHr02);
        }
//      else
//      {
//          DEBUG_LOG("BIO_SWITCH_EXIT FAIL[%d]->[%d]_%s", pvcHr02->workMode, g_bioSwitchMode, __func__);
//      }
        break;
    default:
        break;
    }
}

/*********************************************************************
 * EXTERN FUNCTION INTERFACE,20220715
 **********************************************************************/
void drvVCxxStartSample(vcComMode_t mode, uint16_t fifoIntMs)
{
    uint16_t ret = VCHR02RET_ISOK;
    ret |= vcHr02SoftReset(&vcHr02);
    vcHr02.vcSampleRate = 16;
    vcHr02.mcuOscValue = 32000;
    vcHr02.mcuSclRate = 400;
    vcHr02.workMode = mode;
    if (mode == VCWORK_MODE_HR100HZ)
    {
        vcHr02.vcSampleRate = 100;
    }
    if (mode == VCWORK_MODE_CROSSTALKTEST)
    {
        vcHr02.initDisablePpgFlag = false;
    }
    else
    {
        vcHr02.initDisablePpgFlag = true;
    }
    g_fifoIntMs = fifoIntMs;
    ret |= vcHr02StartSample(&vcHr02);
}

void drvVCxxStopSample(void)
{
    vcHr02StopSample(&vcHr02);
    vcHr02SoftReset(&vcHr02);
    bio_arithemtic_config(NULL, NULL);
    BioInitMark = 1;
}
void drvVCxxSoftReset(void)
{
    vcHr02SoftReset(&vcHr02);
}

void drvVCxxPPGSlotSwitch(unsigned char ledPower)
{
    unsigned char enableSlot = 0, enPs = 0, PsDiv = 0;
    vcHr02ReadRegisters(VCREG11, (uint8_t *)&enableSlot, VCREG11_SIZE);
    switch (vcHr02.workMode)
    {
    case VCWORK_MODE_HRWORK:
        if (ledPower)
        {
            enableSlot = (enableSlot & 0xF8) | 0x05;
        }
        else
        {
            enableSlot = (enableSlot & 0xF8) | 0x04;
        }
        break;
    case VCWORK_MODE_SPO2WORK:
        if (ledPower)
        {
            enableSlot = (enableSlot & 0xF8) | 0x07;
            PsDiv = 0;
            enPs = 0x2f; /* clear ps irq */
        }
        else
        {
            enableSlot = (enableSlot & 0xF8) | 0x04;
            PsDiv = vcHr02.regConfig[3] & 0x3f;
            enPs = (vcHr02.regConfig[1] | 0x10) & 0xf0;
        }
        vcHr02WriteRegisters(VCREG16, (uint8_t *)&PsDiv, VCREG16_SIZE);
        vcHr02WriteRegisters(VCREG12, (uint8_t *)&enPs, VCREG12_SIZE);
        vcHr02.regConfig[1] = enPs;
        break;
    default:
        break;
    }
    vcHr02WriteRegisters(VCREG11, (uint8_t *)&enableSlot, VCREG11_SIZE);
}

uint16_t drvVCxxWearStatusDetection(void)
{
    vcHr02Ret_t ret = VCHR02RET_ISOK;
//  vcHr02_t *pvcHr02 = &vcHr02;
//  uint8_t vcHr02SlotCnt = 0;
    ret |= vcHr02ReadReg(&vcHr02);
    ret |= vcHr02ReadStatus(&vcHr02, true);

    return ret;
}

bool drvVCxxIsPPGenable(void)
{
    uint8_t VC11CtrlRegister = 0;
    vcHr02ReadRegisters(VCREG11, &VC11CtrlRegister, 1);
    // UTE_MODULE_LOG(UTE_LOG_SYSTEM_LVL, "%s,VC11CtrlRegister=0x%x,result=%d", __func__,VC11CtrlRegister,(VC11CtrlRegister & VCXX_CTRL_ENABLE_PPG));
    if (VC11CtrlRegister & 0x01)
    {
        return true;
    }
    else
    {
        return false;
    }
}

void drvVCxxReducePsSampleToSavePower(bool isPowerOn)
{
    drvVCxxPPGSlotSwitch(0);
    g_stBioParam.iswearCnt = BIO_ISWEAR_CNT;
    g_stBioParam.unwearCnt = BIO_UNWEAR_CNT;
    if (!isPowerOn)
    {
        vcHr02SoftReset(&vcHr02);
        vcHr02.wearStatus = VCHR02WEARST_UNWEAR;
        vcHr02StopSample(&vcHr02); // hrs need to be turn off when power off
    }
}

bool drvVCxxIsWear(void)
{
    if (vcHr02.wearStatus == VCHR02WEARST_ISWEAR)
    {
        return true;
    }
    else
    {
        return false;
    }
}
uint8_t drvVCxxGetIntFlag(void)
{
    uint8_t flag = 0;
    if (vcHr02.vcFifoReadFlag)
    {
        flag |= VCXX_INT_FLAG_FIFO_READ;
    }
    if (vcHr02.vcPsFlag)
    {
        flag |= VCXX_INT_FLAG_PS;
    }
    return flag;
}

bool drvVCxxGetSampleValues(vcComSampleData_t *sampleData, uint8_t *size)
{
    vcHr02Ret_t ret = VCHR02RET_ISOK;
    ret = vcHr02GetSampleValues(&vcHr02, size);
    memcpy(sampleData, &vcHr02.sampleData, sizeof(vcComSampleData_t));
    if (VCHR02RET_UNWEARTOISWEAR == ret)
    {
        return true;
    }
    else
    {
        return false;
    }
}

void drvVCxxGetTestData(uint16_t *pre, uint16_t *current, uint8_t *psvalue)
{
    *pre = vcHr02.sampleData.preValue[0];
    *current = vcHr02.sampleData.currentValue[0];
    *psvalue = vcHr02.sampleData.psValue;
}

void drvVCxxGetBioTestData(uint16_t *pBioVal)
{
    *pBioVal = vcHr02.sampleData.bioVal;
}

//uint8_t drvVCxxGetWorkMode(void)
vcComMode_t drvVCxxGetWorkMode(void)
{
    if (VCWORK_MODE_WEAR_BIOMETRICS == vcHr02.workMode)
    {
        return g_bioSwitchMode;
    }
    else
    {
        return vcHr02.workMode;
    }
}

bool drvVCxxIsSample(void)
{
    return isVCxxSample;
}

bool drvVCxxIsOscCheckFlag(void)
{
    return isVCxxCheckNomal;
}
