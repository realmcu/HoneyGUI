#include "gui_post_process.h"
#include "gui_api.h"
#include "gui_obj.h"

void (*blur_prepare)(gui_rect_t *rect, void **mem) = NULL;
void (*blur_depose)(void **mem) = NULL;

typedef struct post_process_item
{
    post_process_event *event;
    struct post_process_item *next;
} T_PROCESS_ITEM;

static T_PROCESS_ITEM *process_list = NULL;
static T_PROCESS_ITEM *list_tail = NULL;

void post_process_add(post_process_event *event)
{
    bool new_list = false;
    if (process_list == NULL)
    {
        process_list = (T_PROCESS_ITEM *)gui_malloc(sizeof(T_PROCESS_ITEM));
        list_tail = process_list;
        new_list = true;
    }
    post_process_event *new_event = (post_process_event *)gui_malloc(sizeof(post_process_event));
    memcpy(new_event, event, sizeof(post_process_event));

    if (new_list)
    {
        list_tail->event = new_event;
        list_tail->next = NULL;
    }
    else
    {
        T_PROCESS_ITEM *new_item = (T_PROCESS_ITEM *)gui_malloc(sizeof(T_PROCESS_ITEM));
        list_tail->next = new_item;
        new_item->event = new_event;
        new_item->next = NULL;
        list_tail = new_item;
    }
}

void post_process_end(void)
{
    T_PROCESS_ITEM *current = process_list;
    while (current != NULL)
    {
        if (current->event != NULL)
        {
            if (current->event->param != NULL)
            {
                if (current->event->type == POST_PROCESS_BLUR)
                {
                    if (blur_depose != NULL)
                    {
                        post_process_blur_param *param = (post_process_blur_param *)current->event->param;
                        blur_depose(&param->cache_mem);
                    }
                }
                gui_free(current->event->param);
            }
            gui_free(current->event);
        }
        T_PROCESS_ITEM *next = current->next;
        gui_free(current);
        current = next;
    }
    process_list = NULL;
    list_tail = NULL;
}

void post_process_handle(void)
{
    T_PROCESS_ITEM *current = process_list;
    while (current != NULL)
    {
        switch (current->event->type)
        {
        case POST_PROCESS_BLUR:
            {
                struct acc_engine *acc = gui_get_acc();
                post_process_blur_param *param = (post_process_blur_param *)current->event->param;
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
                break;
            }
        default:
            break;
        }
        current = current->next;
    }
}

void pre_process_handle(post_process_event *event)
{
    switch (event->type)
    {
    case POST_PROCESS_BLUR:
        {
            struct acc_engine *acc = gui_get_acc();
            post_process_blur_param *param = (post_process_blur_param *)event->param;
            if (acc->blur != NULL)
            {
                if (param->blur_degree != 0)
                {
                    acc->blur(gui_get_dc(), &param->area, param->blur_degree, param->cache_mem);
                }
            }
            break;
        }
    default:
        break;
    }
}