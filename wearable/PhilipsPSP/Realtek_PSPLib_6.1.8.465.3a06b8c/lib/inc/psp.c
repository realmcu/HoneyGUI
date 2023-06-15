/**
* \addtogroup Philips Sensing Platform Interface Library
* \{
*/
/**
*  \file    psp.c
*  \author  Philips Intellectual Property & Standards, The Netherlands
*  \brief   Sensing Platform Interface Library
*
*  \remarks (c) Copyright 2021 Koninklijke Philips N.V. All rights reserved.
*  This Software Library is provided to Realtek for use under and subject to the terms of the Evaluation License Agreement dated 26 April 2023.
*  Philips Sensing Platform, Philips and its logo are trademarks of Koninklijke Philips N.V..
*  All other names are the trademark or registered trademarks of their respective holders.
*  All rights are reserved. Reproduction in whole or in part is prohibited
*  without the prior written consent of the copyright holder.
*
*  This source code and any compilation or derivative thereof is the proprietary
*  information of KONINKLIJKE PHILIPS N.V. and is confidential in nature.
*  Under no circumstances is this software to be combined with any
*  Open Source Software in any way or placed under an Open Source License of
*  any type without the express written permission of KONINKLIJKE PHILIPS N.V.
*/

/*============================================================================*/
/* Include files                                                              */
/*============================================================================*/

#include "psp.h"
#include "psp_map.h"

/*============================================================================*/
/* Constant definitions                                                       */
/*============================================================================*/

/*============================================================================*/
/* Macro definitions                                                          */
/*============================================================================*/

/*============================================================================*/
/* Type definitions                                                           */
/*============================================================================*/

/*============================================================================*/
/* Static data definitions                                                    */
/*============================================================================*/

/*============================================================================*/
/* Extern data definitions                                                    */
/*============================================================================*/

/*============================================================================*/
/* Local function prototypes                                                  */
/*============================================================================*/

/*============================================================================*/
/* Extern function implementations                                            */
/*============================================================================*/

/*----------------------------------------------------------------------------*
 *                                                                            *
 *  Name        : PSP_GetVersionInfo                                          *
 *                                                                            *
 *----------------------------------------------------------------------------*/

PSP_ERROR PSP_GetVersionInfo
(
    FX_UINT08 *const pData,
    FX_UINT16 *const pSize
)
{
    return ((PSP_ERROR(*)
             (FX_UINT08 * const, FX_UINT16 * const))
            PSP_GetVersionInfo_Address)(pData, pSize);
}

/*----------------------------------------------------------------------------*
 *                                                                            *
 *  Name        : PSP_GetDefaultParams                                        *
 *                                                                            *
 *----------------------------------------------------------------------------*/

PSP_ERROR PSP_GetDefaultParams
(
    PSP_INST_PARAMS *const pParams
)
{
    return ((PSP_ERROR(*)
             (PSP_INST_PARAMS * const))
            PSP_GetDefaultParams_Address)(pParams);
}

/*----------------------------------------------------------------------------*
 *                                                                            *
 *  Name        : PSP_Initialise                                              *
 *                                                                            *
 *----------------------------------------------------------------------------*/

PSP_ERROR PSP_Initialise
(
    PSP_INST_PARAMS *const pParams,
    PPSP_INST        *const ppInst
)
{
    return ((PSP_ERROR(*)
             (PSP_INST_PARAMS * const, PPSP_INST * const))
            PSP_Initialise_Address)(pParams, ppInst);
}

/*----------------------------------------------------------------------------*
 *                                                                            *
 *  Name        : PSP_Terminate                                               *
 *                                                                            *
 *----------------------------------------------------------------------------*/

PSP_ERROR PSP_Terminate
(
    PPSP_INST *const ppInst
)
{
    return ((PSP_ERROR(*)
             (PPSP_INST * const))
            PSP_Terminate_Address)(ppInst);
}

/*----------------------------------------------------------------------------*
 *                                                                            *
 *  Name        : PSP_Process                                                 *
 *                                                                            *
 *----------------------------------------------------------------------------*/

PSP_ERROR PSP_Process
(
    PPSP_INST const pInst
)
{
    return ((PSP_ERROR(*)
             (PPSP_INST const))
            PSP_Process_Address)(pInst);
}

/*----------------------------------------------------------------------------*
 *                                                                            *
 *  Name        : PSP_EnableMetrics                                           *
 *                                                                            *
 *----------------------------------------------------------------------------*/

PSP_ERROR PSP_EnableMetrics
(
    PPSP_INST             const pInst,
    FX_UINT08             const numberOfMetrics,
    PSP_METRIC_ID const *const pMetricIdList
)
{
    return ((PSP_ERROR(*)
             (PPSP_INST const, FX_UINT08 const, PSP_METRIC_ID const * const))
            PSP_EnableMetrics_Address)(pInst, numberOfMetrics, pMetricIdList);
}

/*----------------------------------------------------------------------------*
 *                                                                            *
 *  Name        : PSP_DisableMetrics                                          *
 *                                                                            *
 *----------------------------------------------------------------------------*/

PSP_ERROR PSP_DisableMetrics
(
    PPSP_INST             const pInst,
    FX_UINT08             const numberOfMetrics,
    PSP_METRIC_ID const *const pMetricIdList
)
{
    return ((PSP_ERROR(*)
             (PPSP_INST const, FX_UINT08 const, PSP_METRIC_ID const * const))
            PSP_DisableMetrics_Address)(pInst, numberOfMetrics, pMetricIdList);
}

/*----------------------------------------------------------------------------*
 *                                                                            *
 *  Name        : PSP_ListUpdatedMetrics                                      *
 *                                                                            *
 *----------------------------------------------------------------------------*/

PSP_ERROR PSP_ListUpdatedMetrics
(
    PPSP_INST       const pInst,
    PSP_METRIC_ID *const pMetricIdList,
    FX_UINT08      *const pNumberOfMetrics
)
{
    return ((PSP_ERROR(*)
             (PPSP_INST const, PSP_METRIC_ID * const, FX_UINT08 * const))
            PSP_ListUpdatedMetrics_Address)(pInst, pMetricIdList, pNumberOfMetrics);
}

/*----------------------------------------------------------------------------*
 *                                                                            *
 *  Name        : PSP_ListRequiredMetrics                                     *
 *                                                                            *
 *----------------------------------------------------------------------------*/

PSP_ERROR PSP_ListRequiredMetrics
(
    PPSP_INST       const pInst,
    PSP_METRIC_ID *const pMetricIdList,
    FX_UINT08      *const pNumberOfMetrics
)
{
    return ((PSP_ERROR(*)
             (PPSP_INST const, PSP_METRIC_ID * const, FX_UINT08 * const))
            PSP_ListRequiredMetrics_Address)(pInst, pMetricIdList, pNumberOfMetrics);
}

/*----------------------------------------------------------------------------*
 *                                                                            *
 *  Name        : PSP_SetMetric                                               *
 *                                                                            *
 *----------------------------------------------------------------------------*/

PSP_ERROR PSP_SetMetric
(
    PPSP_INST           const pInst,
    PSP_METRIC_ID       const metricId,
    FX_UINT08   const *const pData,
    FX_UINT16           const size
)
{
    return ((PSP_ERROR(*)
             (PPSP_INST const, PSP_METRIC_ID const, FX_UINT08 const * const, FX_UINT16 const))
            PSP_SetMetric_Address)(pInst, metricId, pData, size);
}

/*----------------------------------------------------------------------------*
 *                                                                            *
 *  Name        : PSP_GetMetric                                               *
 *                                                                            *
 *----------------------------------------------------------------------------*/

PSP_ERROR PSP_GetMetric
(
    PPSP_INST     const pInst,
    PSP_METRIC_ID const metricId,
    FX_UINT08    *const pData,
    FX_UINT16    *const pSize
)
{
    return ((PSP_ERROR(*)
             (PPSP_INST const, PSP_METRIC_ID const, FX_UINT08 * const, FX_UINT16 * const))
            PSP_GetMetric_Address)(pInst, metricId, pData, pSize);
}

/*============================================================================*/
/*  End of file                                                               */
/*============================================================================*/
/**
* \}
*/
