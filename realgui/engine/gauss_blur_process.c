#include "gauss_blur_process.h"
#include "gui_api.h"
#include "gui_obj.h"

void (*blur_prepare)(gui_rect_t *rect, void **mem) = NULL;
void (*blur_depose)(void **mem) = NULL;

typedef struct gauss_blur_process_item
{
    gauss_blur_param *param;
    struct gauss_blur_process_item *next;
} T_PROCESS_ITEM;

static T_PROCESS_ITEM *process_list = NULL;
static T_PROCESS_ITEM *list_tail = NULL;

void gauss_blur_post_process_add(gauss_blur_param *param)
{
    bool new_list = false;
    if (process_list == NULL)
    {
        process_list = (T_PROCESS_ITEM *)gui_malloc(sizeof(T_PROCESS_ITEM));
        list_tail = process_list;
        new_list = true;
    }
    gauss_blur_param *new_param = (gauss_blur_param *)gui_malloc(sizeof(gauss_blur_param));
    memcpy(new_param, param, sizeof(gauss_blur_param));

    if (new_list)
    {
        list_tail->param = new_param;
        list_tail->next = NULL;
    }
    else
    {
        T_PROCESS_ITEM *new_item = (T_PROCESS_ITEM *)gui_malloc(sizeof(T_PROCESS_ITEM));
        list_tail->next = new_item;
        new_item->param = new_param;
        new_item->next = NULL;
        list_tail = new_item;
    }
}

void gauss_blur_post_process_end(void)
{
    T_PROCESS_ITEM *current = process_list;
    while (current != NULL)
    {
        if (current->param != NULL)
        {
            if (blur_depose != NULL)
            {
                blur_depose(&current->param->cache_mem);
            }
            gui_free(current->param);
        }
        T_PROCESS_ITEM *next = current->next;
        gui_free(current);
        current = next;
    }
    process_list = NULL;
    list_tail = NULL;
}

void gauss_blur_post_process_handle(void)
{
    T_PROCESS_ITEM *current = process_list;
    while (current != NULL)
    {
        struct acc_engine *acc = gui_get_acc();
        gauss_blur_param *param = (gauss_blur_param *)current->param;
        if (acc->blur != NULL)
        {
            if (blur_prepare != NULL)
            {
                blur_prepare(&param->area, &param->cache_mem);
            }
            if (param->blur_degree != 0)
            {
                acc->blur(gui_get_dc(), &param->area, param->blur_degree, param->cache_mem);
            }
        }
        current = current->next;
    }
}

void gauss_blur_pre_process_handle(gauss_blur_param *param)
{
    struct acc_engine *acc = gui_get_acc();
    if (acc->blur != NULL)
    {
        if (param->blur_degree != 0)
        {
            acc->blur(gui_get_dc(), &param->area, param->blur_degree, param->cache_mem);
        }
    }
}