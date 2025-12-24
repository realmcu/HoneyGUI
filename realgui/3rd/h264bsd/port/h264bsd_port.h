/*
 * File      : h264bsd_port.h
 */
#ifndef __H264BSD_PORT_H__
#define __H264BSD_PORT_H__


#include <stdio.h>
#include <stdlib.h>

#ifdef __cplusplus
extern "C" {
#endif

void *h264bsd_malloc(size_t size);
void *h264bsd_realloc(void *ptr, size_t size);
void h264bsd_free(void *ptr);

#ifdef __cplusplus
}
#endif

#endif
