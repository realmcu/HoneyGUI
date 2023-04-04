#ifndef __RTK_GUI_RESOURCE_H__
#define __RTK_GUI_RESOURCE_H__


#ifdef __cplusplus
extern "C" {
#endif

#if defined(_WIN32)
#include "root_image_sim_vscode/sim_vscode.h"
#elif defined (GUI_RTL8762D)
#include "root_image_8762d/resource_8762d.h"
#elif defined (GUI_RTL8772F)
#include "root_image_8772f/resource_8772f.h"
#elif defined (GUI_RTL8762G)
#include "root_image_8762g/resource_8762d.h"
#elif defined (GUI_KEIL_SIM)
#include "root_image_sim_keil/sim_keil.h"
#endif


#ifdef __cplusplus
}
#endif

#endif
