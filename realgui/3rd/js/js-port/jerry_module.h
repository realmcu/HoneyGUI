#ifndef JERRY_MODULE_H__
#define JERRY_MODULE_H__

#ifdef __cplusplus
extern "C"
{
#endif

char *js_module_dirname(char *path);
char *js_module_normalize_path(const char *directory, const char *filename);

int   js_module_init(void);

#ifdef __cplusplus
}
#endif

#endif

