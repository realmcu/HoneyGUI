#include "gui_common.h"
#include "app_gui_main.h"

gui_obj_t *obj_stack[GUI_OBJ_STACK_SIZE_MAX] = {NULL};
int8_t obj_stack_index = 0;

gui_obj_t *get_current_active_widget(void)
{
    gui_app_t *app = get_app_watch_ui();
    gui_list_t *node = NULL;

    gui_list_for_each(node, &(app->screen.child_list))
    {
        gui_obj_t *find_obj = gui_list_entry(node, gui_obj_t, brother_list);
        if (find_obj->not_show == false)
        {
            gui_log("find_obj = 0x%x, find_obj.name = %s\n", (uint32_t *)find_obj, find_obj->name);
            return find_obj;
        }
    }
    return NULL;
}

void push_current_widget(void *obj)
{

    if (obj_stack_index < GUI_OBJ_STACK_SIZE_MAX)
    {
        obj_stack[obj_stack_index] = (gui_obj_t *)obj;
    }
    else
    {
        GUI_ASSERT(obj_stack_index >= GUI_OBJ_STACK_SIZE_MAX);
        return;
    }
    obj_stack_index++;

    gui_log("push_current_widget: obj_stack = 0x%x obj_stack_index = %d\n",
            (uint32_t *)obj_stack[obj_stack_index - 1], obj_stack_index);
}

gui_obj_t *pop_current_widget(void)
{
    obj_stack_index--;
    if (obj_stack_index < GUI_OBJ_STACK_SIZE_MAX && obj_stack_index >= 0)
    {
        gui_log("pop_current_widget obj_stack = %x, name = %s, obj_stack_index = %d\n",
                (uint32_t *)obj_stack[obj_stack_index], obj_stack[obj_stack_index]->name,
                obj_stack_index);
        return obj_stack[obj_stack_index];
    }
    else
    {
        GUI_ASSERT(obj_stack_index < 0);
    }
    return NULL;
}