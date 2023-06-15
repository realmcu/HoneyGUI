/**
* \addtogroup Philips Sensing Platform Interface Library
* \{
*/
/**
*  \file    psp.h
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
/*  Multiple inclusion protection                                             */
/*============================================================================*/

#ifndef __PSP_H__
#define __PSP_H__

/*============================================================================*/
/*  C++ protection                                                            */
/*============================================================================*/

#ifdef __cplusplus
extern "C" {
#endif

/*============================================================================*/
/* Include files                                                              */
/*============================================================================*/

#include "fx_datatypes.h"

/*============================================================================*/
/* Constant definitions                                                       */
/*============================================================================*/

/* input metrics */
#define PSP_METRIC_ID_AGE                           ( 0x03 )
#define PSP_METRIC_ID_PROFILE                       ( 0x04 )
#define PSP_METRIC_ID_HEIGHT                        ( 0x05 )
#define PSP_METRIC_ID_WEIGHT                        ( 0x06 )
#define PSP_METRIC_ID_SLEEPPREFERENCE               ( 0x09 )
#define PSP_METRIC_ID_TIME                          ( 0x1E )
#define PSP_METRIC_ID_ACCELERATION                  ( 0x2B )
#define PSP_METRIC_ID_SKINCONDUCTANCE               ( 0x3D )
#define PSP_METRIC_ID_GYRO                          ( 0x79 )
#define PSP_METRIC_ID_PRESSURE                      ( 0x7A )
#define PSP_METRIC_ID_PPG_INFRARED                  ( 0x7B )
#define PSP_METRIC_ID_PPG_RED                       ( 0x7C )
#define PSP_METRIC_ID_PPG_MOTION                    ( 0x7D )
#define PSP_METRIC_ID_PPG_GREEN                     ( 0x7E )
#define PSP_METRIC_ID_PPG_AMBIENT                   ( 0x7F )

/* extracted metrics */
#define PSP_METRIC_ID_HEARTRATE                     ( 0x20 )
#define PSP_METRIC_ID_RESTINGHEARTRATE              ( 0x21 )
#define PSP_METRIC_ID_SKINPROXIMITY                 ( 0x22 )
#define PSP_METRIC_ID_ENERGYEXPENDITURE             ( 0x23 )
#define PSP_METRIC_ID_SPEED                         ( 0x24 )
#define PSP_METRIC_ID_MOTIONCADENCE                 ( 0x25 )
#define PSP_METRIC_ID_ACTIVITYTYPE                  ( 0x26 )
#define PSP_METRIC_ID_HEARTBEATS                    ( 0x27 )
#define PSP_METRIC_ID_VO2MAX                        ( 0x28 )
#define PSP_METRIC_ID_FITNESSINDEX                  ( 0x29 )
#define PSP_METRIC_ID_RESPIRATIONRATE               ( 0x2A )
#define PSP_METRIC_ID_LOWPOWERHEARTRATE             ( 0x2D )
#define PSP_METRIC_ID_ACTIVITYCOUNT                 ( 0x2E )
#define PSP_METRIC_ID_PRIVATEDATA                   ( 0x2F )
#define PSP_METRIC_ID_SLEEPSTAGES                   ( 0x30 )
#define PSP_METRIC_ID_COMPRESSED_ACCELERATION       ( 0x34 )
#define PSP_METRIC_ID_COMPRESSED_PPG                ( 0x35 )
#define PSP_METRIC_ID_HEARTRHYTHMTYPE               ( 0x3B )
#define PSP_METRIC_ID_LOWPOWERENERGYEXPENDITURE     ( 0x3C )
#define PSP_METRIC_ID_STRESSLEVELSKINCONDUCTANCE    ( 0x3E )
#define PSP_METRIC_ID_COGNITIVEZONE                 ( 0x3F )
#define PSP_METRIC_ID_STRESSLEVELHEARTRATE          ( 0x40 )
#define PSP_METRIC_ID_SPO2                          ( 0x41 )
#define PSP_METRIC_ID_FALL                          ( 0x42 )

/** @defgroup PSP_ERROR PSP_ERROR
 *  @{ */
#define PSP_ERROR_NONE                              ( 0x0000 ) /**< @brief no error */
#define PSP_ERROR_NOT_ENOUGH_MEMORY                 ( 0x9001 ) /**< @brief too little memory provided */
#define PSP_ERROR_INITIALISATION_FAILED             ( 0x9002 ) /**< @brief initialisation failed */
#define PSP_ERROR_SIZE_CONFLICT                     ( 0x9003 ) /**< @brief metric data conflicts its size definition */
#define PSP_ERROR_METRIC_NOT_SUPPORTED              ( 0x9004 ) /**< @brief undefined metric id used */
#define PSP_ERROR_MODULE_NOT_INITIALISED            ( 0x9005 ) /**< @brief invalid instance provided */
#define PSP_ERROR_MEMORY_CORRUPTED                  ( 0x9006 ) /**< @brief library corrupted */
#define PSP_ERROR_INVALID_PARAMS                    ( 0x9007 ) /**< @brief provided parameters invalid */
#define PSP_ERROR_MULTIPLE_SET                      ( 0x9008 ) /**< @brief trying to set metric multiple times */
#define PSP_ERROR_DATA_INCOMPLETE                   ( 0x9009 ) /**< @brief not all metric data set as required */
/** @} */

#define PSP_MAX_UPDATED_METRICS                     ( 32 )
#define PSP_MAX_REQUIRED_METRICS                    ( 16 )

/*============================================================================*/
/* Type definitions                                                           */
/*============================================================================*/

typedef FX_UINT16         PSP_ERROR;
typedef FX_UINT08         PSP_METRIC_ID;

/* Parameter definition ------------------------------------------------------*/

/** @brief   PSP Instance Parameter structure type. */
typedef struct psp_inst_params_tag
{
    FX_VOID              *pMem;         /**< Pointer to memory block used for instance initialisation */
    FX_UINT08            *pSourceID;    /**< Pointer to memory block containing the SourceID */
    FX_UINT32             memorySize;   /**< Size of memory block appointed by pMem field */
    FX_UINT16             sourceIDSize; /**< Size of memory block appointed by pSourceID field */
    FX_SINT16
    calCoefSpO2[3];   /**< array of SpO2 calibration coefficients: a, b, c  , scaled with factor 100 */
    FX_UINT08
    alarmRateFall;    /**< false alarm rate for fall:  0: once per day, 1: once per week, 2: once per month */

} PSP_INST_PARAMS, * PPSP_INST_PARAMS;

/* Internal data definition --------------------------------------------------*/

/** @brief PSP Instance Parameter structure containing the actual PSP Instance. */
typedef struct psp_inst_tag *PPSP_INST;

#ifdef PSP_DYNAMIC_BUILD
#ifdef PSP_BUILD
#define PSP_API       FX_INTF_EXPORT
#else
#define PSP_API       FX_INTF_IMPORT
#endif /* PSP_BUILD */
#else
#define PSP_API       extern
#endif

/*============================================================================*/
/* Extern data declarations                                                   */
/*============================================================================*/

/*============================================================================*/
/* Extern function prototypes                                                 */
/*============================================================================*/

/*----------------------------------------------------------------------------*/
/**
 *  @brief        PSP_GetVersionInfo
 *
 *                Fills teh memory block, appointed by pData, with version information
 *                that is unique for each version of the library.
 *
 *                The array has to be allocated/maintained by the application.
 *
 *  @param        [in]  pData : Pointer to memory block where the version
 *                              information will be written to.
 *  @param        [in]  pSize : Length of provided memory block, must be at least 20
 *
 *  @param        [out] pData : Pointer to memory block that now contains version information
 *  @param        [out] pSize : Length of the provided version information
 *
 *  @return       Value of type \ref PSP_ERROR identifying the error ( \ref PSP_ERROR_NONE if succesfull )
 *
 *  @note
 *
**/
/*----------------------------------------------------------------------------*/

PSP_API PSP_ERROR PSP_GetVersionInfo
(
    FX_UINT08   *const pData,
    FX_UINT16   *const pSize
);

/*----------------------------------------------------------------------------*/
/**
 *  @brief        PSP_GetDefaultParams
 *
 *                Fills a structure, appointed by pParams, with default data,
 *                e.g. minimal size of provided memory block,  to be used in
 *                function PSP_Initialise
 *
 *                The structure has to be allocated/maintained by the application.
 *
 *  @param        [in]  pParams : Pointer to 'empty' parameter struct to be filled with default data
 *
 *  @param        [out] pParams : Pointer to 'filled' parameter struct
 *                                memorySize contains the size needed for an instance.
 *                                sourceIDSize contains the maximum size that is used for source id.
 *
 *  @return       Value of type \ref PSP_ERROR identifying the error ( \ref PSP_ERROR_NONE if succesfull )
 *
 *  @note
 *
 **/
/*----------------------------------------------------------------------------*/

PSP_API PSP_ERROR PSP_GetDefaultParams
(
    PSP_INST_PARAMS *const pParams
);

/* ---------------------------------------------------------------------------*/
/**
 * @brief PSP_Initialise
 *
 *                Creates an @c PSP instance. Allocates and initialises the
 *                resources needed using the data provided by the @c pParams
 *                structure. The actual used memory block from the parameters
 *                must be maintained and not modified during the lifetime of the instance.
 *
 *                On successful return, @p ppInst identifies the created PSP
 *                instance. This pointer must be used in subsequent calls to
 *                functions relating to this instance.
 *
 *                To release and cleanup the created instance the
 *                {@link #PSP_Terminate()} function must be called.
 *
 *                The PSP instance is allocated/maintained by the application.
 *
 *  @param        [in] pParams  : Pointer to a structure that contains parameters
 *                                used for creation of an PSP instance.
 *  @param        [in]  ppInst  : Pointer to an PSP instance pointer.
 *
 *  @param        [out] ppInst  : On success, a valid pointer to an PSP instance pointer.
 *  @param        [out] pParams : Pointer to a structure that contains updated
 *                                information in the created instance ( e.g. actual memory used
                                  from the memory pool )
 *
 *  @return       Value of type \ref PSP_ERROR identifying the error ( \ref PSP_ERROR_NONE if succesfull )
 *
 *  @note
 **/
/*----------------------------------------------------------------------------*/

PSP_API  PSP_ERROR PSP_Initialise
(
    PSP_INST_PARAMS *const pParams,
    PPSP_INST        *const ppInst
);

/*----------------------------------------------------------------------------*/
/**
 *  @brief        PSP_Terminate
 *
 *                Releases and cleans up the resources used by the PSP instance.
 *
 *                After this function the memory block reserved for this instance,
 *                done with PSP_Initialise function, can be released by the application
 *
 *  @param        [in]  ppInst : Pointer to an PSP instance pointer.
 *
 *  @param        [out] ppInst : On success, pointer to PSP instance pointer is set to NULL.
 *
 *  @return       Value of type \ref PSP_ERROR identifying the error ( \ref PSP_ERROR_NONE if succesfull )
 *
 *  @note
 **/

/*----------------------------------------------------------------------------*/

PSP_API PSP_ERROR PSP_Terminate
(
    PPSP_INST        *const ppInst
);

/*----------------------------------------------------------------------------*/
/**
 *
 *  @brief        PSP_Process
 *
 *                Performs the actual processing on provided data/metrics for
 *                producing the enabled metric(s)
 *
 *                Data/metrics can be provided with function: PSP_SetMetric
 *                Which data/metrics should be set can be queried
 *                with function: PSP_ListRequestedMetrics
 *
 *                Metrics can be enabled with function PSP_EnableMetric
 *
 *                After PSP_Process metrics can be updated, which metrics are updated
 *                can be queried with function:  FX_ListUpdatedMetrics
 *
 *                The actual (updated) metric data can be retrieved with
 *                function: PSP_GetMetric
 *
 *  @param        [in] pInst   : Pointer to a PSP instance.
 *
 *  @return       Value of type \ref PSP_ERROR identifying the error ( \ref PSP_ERROR_NONE if succesfull )
 *
 *  @note
 *
 **/
/*----------------------------------------------------------------------------*/

PSP_API PSP_ERROR PSP_Process
(
    PPSP_INST const pInst
);

/* ---------------------------------------------------------------------------*/
/**
 *
 *  @brief        PSP_EnableMetrics
 *
 *                Enables metrics specified in provided list
 *
 *  @param        [in] pInst           : Pointer to an PSP instance.
 *  @param        [in] numberOfMetrics : number of metrics in the list.
 *  @param        [in] pMetricIdList   : List of metric id's.
 *
 *  @return       Value of type \ref PSP_ERROR identifying the error ( \ref PSP_ERROR_NONE if succesfull )
 *
 *  @note         The function will only enable 'known' metrics.
 *
 **/
/*----------------------------------------------------------------------------*/

PSP_API PSP_ERROR PSP_EnableMetrics
(
    PPSP_INST             const pInst,
    FX_UINT08             const numberOfMetrics,
    PSP_METRIC_ID const *const pMetricIdList
);

/*----------------------------------------------------------------------------*/
/**
 *
 *  @brief        PSP_DisableMetrics
 *
 *                Disables metrics specified in provided list
 *
 *  @param        [in] pInst           : Pointer to an PSP instance.
 *  @param        [in] numberOfMetrics : number of metrics in the list.
 *  @param        [in] pMetricIdList   : List of metric id's.
 *
 *  @return       Value of type \ref PSP_ERROR identifying the error ( \ref PSP_ERROR_NONE if succesfull )
 *
 *  @note         The function will disable any supported metrics.
 *
 **/
/*----------------------------------------------------------------------------*/

PSP_API PSP_ERROR PSP_DisableMetrics
(
    PPSP_INST             const pInst,
    FX_UINT08             const numberOfMetrics,
    PSP_METRIC_ID const *const pMetricIdList
);

/*----------------------------------------------------------------------------*/
/**
 *
 *  @brief        PSP_ListUpdatedMetrics
 *
 *                Returns list if mectric id's that are updated as consequence
 *                of PSP_Process function call(s)
 *
 *                This list can dynamically change after PSP_Process function.
 *
 *  @param        [in]  pInst            : Pointer to an PSP instance.
 *  @param        [in]  pMetricIdList    : 'empty' list of metricId.
 *  @param        [in]  pNumberOfMetrics : length of the 'empty' metricId list, maximal PSP_MAX_UPDATED_METRICS
 *
 *  @param        [out] pMetricIdList    : filled list of metricId.
 *  @param        [out] pNumberOfMetrics : actual valid/written number of metricId's in the list
 *
 *  @return       Value of type \ref PSP_ERROR identifying the error ( \ref PSP_ERROR_NONE if succesfull )
 *
 *  @note
 *
 **/
/*----------------------------------------------------------------------------*/

PSP_API PSP_ERROR PSP_ListUpdatedMetrics
(
    PPSP_INST       const pInst,
    PSP_METRIC_ID *const pMetricIdList,
    FX_UINT08      *const pNumberOfMetrics
);

/*----------------------------------------------------------------------------*/
/**
 *
 *  @brief        PSP_ListRequiredMetrics
 *
 *                Returns list of metric id's that are required to be set with
 *                PSP_SetMetric before next call to PSP_Process function
 *
 *                This list can dynamically change after functions:
 *                PSP_EnableMetrics / PSP_DisableMetrics / PSP_Process functions
 *
 *  @param        [in]  pInst            : Pointer to an PSP instance.
 *  @param        [in]  pMetricIdList    : 'empty' list of metricId.
 *  @param        [in]  pNumberOfMetrics : length of the 'empty' metricId list, maximal PSP_MAX_REQUIRED_METRICS.
 *
 *  @param        [out] pMetricIdList    : filled list of metricId.
 *  @param        [out] pNumberOfMetrics : actual valid/written number of metricId's in the list
 *
 *  @return       Value of type \ref PSP_ERROR identifying the error ( \ref PSP_ERROR_NONE if succesfull )
 *
 *  @note
 *
 **/
/*----------------------------------------------------------------------------*/

PSP_API PSP_ERROR PSP_ListRequiredMetrics
(
    PPSP_INST       const pInst,
    PSP_METRIC_ID *const pMetricIdList,
    FX_UINT08      *const pNumberOfMetrics
);

/*----------------------------------------------------------------------------*/
/**
 *
 *  @brief        PSP_SetMetric
 *
 *                Feed metric data
 *
 *  @param        [in] pInst      : Pointer to an PSP instance.
 *  @param        [in] metricId   : metric id.
 *  @param        [in] pData      : Pointer to location filled with metric data.
 *  @param        [in] size       : The size ( in bytes ) of the pData location ( metric data ).
 *
 *  @return       Value of type \ref PSP_ERROR identifying the error ( \ref PSP_ERROR_NONE if succesfull )
 *
 *  @note         The metric data consists of metricId,length,index,quality,data
 *
 **/
/*----------------------------------------------------------------------------*/

PSP_API PSP_ERROR PSP_SetMetric
(
    PPSP_INST            const pInst,
    PSP_METRIC_ID        const metricId,
    FX_UINT08    const *const pData,
    FX_UINT16            const size
);

/*----------------------------------------------------------------------------*/
/**
 *
 *  @brief        PSP_GetMetric
 *
 *                Get metric data
 *
 *  @param        [in] pInst           : Pointer to an PSP instance.
 *  @param        [in] metricId        : metric id.
 *  @param        [in] pData           : Pointer to location where metric data will be written to.
 *  @param        [in] pSize           : The size ( in bytes ) of the pData location.
 *
 *  @param        [out] pData          : Pointer to location filled with metric data.
 *  @param        [out] pSize          : The size ( in bytes ) of the pData location ( metric data ).
 *
 *  @return       Value of type \ref PSP_ERROR identifying the error ( \ref PSP_ERROR_NONE if succesfull )
 *
 *  @note         The metric data consists of metricId,length,index,quality,data
 *
 **/
/*----------------------------------------------------------------------------*/

PSP_API PSP_ERROR PSP_GetMetric
(
    PPSP_INST       const pInst,
    PSP_METRIC_ID   const metricId,
    FX_UINT08      *const pData,
    FX_UINT16      *const pSize
);

/*============================================================================*/
/*  End of C++ protection                                                     */
/*============================================================================*/

#ifdef __cplusplus
}
#endif

/*============================================================================*/
/*  End of multiple inclusion protection                                      */
/*============================================================================*/

#endif

/*============================================================================*/
/*  End of file                                                               */
/*============================================================================*/
/**
* \}
*/
