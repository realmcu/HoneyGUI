#ifndef __APP_CALCULATOR_H__
#define __APP_CALCULATOR_H__

#ifdef __cplusplus
extern "C" {
#endif
#include <gui_app.h>

extern void *gui_calculator_create(void *parent,  const char *name,
                                   int16_t x, int16_t y, int16_t w, int16_t h);
extern gui_app_t *get_app_calculator(void);


#ifdef __cplusplus
}
#endif
#endif


