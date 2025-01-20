=============
开关 (Switch)
=============

开关控件是一个自定义的开关按钮控件，常用于启用或禁用某些功能或选项，允许开发者通过切换按钮的状态来控制应用程序的行为。

用法
----

创建控件
~~~~~~~~

开发者可以调用 :cpp:any:`gui_switch_create` 或者 :cpp:any:`gui_switch_create_from_ftl` 从内存地址创建开关控件，使用:cpp:any:`gui_switch_create_from_fs` 从文件系统创建开关控件。
可以参考`gui_switch_t`中的具体参数，例如base、switch_picture、on_pic_addr、off_pic_addr等。

开/关
~~~~~

开发者可以使用以下API来打开或关闭开关控件，并触发事件(由gui_obj_add_event_cb创建)。:cpp:any:`gui_switch_turn_on` 或者 :cpp:any:`gui_switch_turn_off`。

更新开关状态
~~~~~~~~~~~~

如果已经打开/关闭了开关控件，开发者可以通过以下API将状态更新为打开或关闭。:cpp:any:`gui_switch_is_on` 或者 :cpp:any:`gui_switch_is_off`。

改变开关控件状态
~~~~~~~~~~~~~~~~

开发者可以调用 :cpp:any:`gui_switch_change_state` 来设置开关控件的状态并改变UI，但不会触发任何事件(由gui_obj_add_event_cb创建)。

示例
----

.. code-block:: c

    #include "root_image_hongkong/ui_resource.h"
    #include "gui_switch.h"
    #include "gui_img.h"

    static gui_img_t *img;

    static void img_animate(gui_img_t *img)
    {
        gui_log("%f\n", img->animate->progress_percent);
        if (img->animate->progress_percent < 0.5f)
        {
            GET_BASE(img)->y = img->animate->progress_percent * 2 * 100 - 100;
        }
        else if (img->animate->progress_percent >= 0.5f)
        {
            GET_BASE(img)->y = (1 - img->animate->progress_percent) * 2 * 100 - 100;
        }
        if (img->animate->progress_percent == 1.0f)
        {
            img->animate->current_repeat_count = 0;
            img->base.not_show = true;
        }

    }

    static void reset_animate()
    {
        img->animate->animate = true;
        img->base.not_show = false;
        img->animate->current_frame = 0;
        img->animate->current_repeat_count = 0;
        img->animate->progress_percent = 0;
    }

    static void callback_disturb_on()
    {
        reset_animate();
        img->draw_img->data = WURAOKAI_BIN;
    }

    static void callback_disturb_off()
    {
        reset_animate();
        img->draw_img->data = WURAOGUAN_BIN;
    }

    static void callback_mute_on()
    {
        reset_animate();
        img->draw_img->data = JINGYINKAI_BIN;
    }

    static void callback_mute_off()
    {
        reset_animate();
        img->draw_img->data = JINGYINGUAN_BIN;
    }

    static void callback_call_on()
    {
        reset_animate();
        img->draw_img->data = DIANHUAKAI_BIN;
    }

    static void callback_call_off()
    {
        reset_animate();
        img->draw_img->data = DIANHUAGUAN_BIN;
    }

    static void callback_bright_on()
    {
        reset_animate();
        img->draw_img->data = LIANGDUKAI_BIN;
    }

    static void callback_bright_off()
    {
        reset_animate();
        img->draw_img->data = LIANGDUGUAN_BIN;
    }

    static void callback_watch_on()
    {
        reset_animate();
        img->draw_img->data = SHIZHONGKAI_BIN;
    }

    static void callback_watch_off()
    {
        reset_animate();
        img->draw_img->data = SHIZHONGGUAN_BIN;
    }

    static void callback_set_on()
    {
        reset_animate();
        img->draw_img->data = SHEZHIKAI_BIN;
    }

    static void callback_set_off()
    {
        reset_animate();
        img->draw_img->data = SHEZHIGUAN_BIN;
    }

    void page_tb_control0(void *parent)
    {
        // gui_img_creat_from_mem(parent, "parent", CONTROLMENU_0_BIN, 0, 0, 0, 0);
        gui_switch_t *sw_no_disturb  = gui_switch_create(parent, 10, 108, 169, 98, NO_DISTURB_OFF_BIN,
                                                        NO_DISTURB_ON_BIN);
        gui_switch_t *sw_mute        = gui_switch_create(parent, 190, 108, 169, 98, MUTE_OFF_BIN,
                                                        MUTE_ON_BIN);
        gui_switch_t *sw_call        = gui_switch_create(parent, 10, 220, 169, 98, CALL_OFF_BIN,
                                                        CALL_ON_BIN);
        gui_switch_t *sw_bright      = gui_switch_create(parent, 190, 220, 169, 98, BRIGHT_OFF_BIN,
                                                        BRIGHT_ON_BIN);
        gui_switch_t *sw_watch       = gui_switch_create(parent, 10, 332, 169, 98, WATCH_OFF_BIN,
                                                        WATCH_ON_BIN);
        gui_switch_t *sw_set         = gui_switch_create(parent, 190, 332, 169, 98, SET_OFF_BIN,
                                                        SET_ON_BIN);
        img =  gui_img_create_from_mem(GET_BASE(parent)->parent, 0, WURAOKAI_BIN, 0, 0, 0, 0);
        gui_img_set_animate(img, 1000, 1, img_animate, img);
        img->animate->animate = false;
        img->base.not_show = true;

        gui_obj_add_event_cb(sw_no_disturb, (gui_event_cb_t)callback_disturb_on, GUI_EVENT_1, NULL);
        gui_obj_add_event_cb(sw_no_disturb, (gui_event_cb_t)callback_disturb_off, GUI_EVENT_2, NULL);
        gui_obj_add_event_cb(sw_mute, (gui_event_cb_t)callback_mute_on, GUI_EVENT_1, NULL);
        gui_obj_add_event_cb(sw_mute, (gui_event_cb_t)callback_mute_off, GUI_EVENT_2, NULL);
        gui_obj_add_event_cb(sw_call, (gui_event_cb_t)callback_call_on, GUI_EVENT_1, NULL);
        gui_obj_add_event_cb(sw_call, (gui_event_cb_t)callback_call_off, GUI_EVENT_2, NULL);
        gui_obj_add_event_cb(sw_bright, (gui_event_cb_t)callback_bright_on, GUI_EVENT_1, NULL);
        gui_obj_add_event_cb(sw_bright, (gui_event_cb_t)callback_bright_off, GUI_EVENT_2, NULL);
        gui_obj_add_event_cb(sw_watch, (gui_event_cb_t)callback_watch_on, GUI_EVENT_1, NULL);
        gui_obj_add_event_cb(sw_watch, (gui_event_cb_t)callback_watch_off, GUI_EVENT_2, NULL);
        gui_obj_add_event_cb(sw_set, (gui_event_cb_t)callback_set_on, GUI_EVENT_1, NULL);
        gui_obj_add_event_cb(sw_set, (gui_event_cb_t)callback_set_off, GUI_EVENT_2, NULL);
    }

.. raw:: html

   <br>
   <div style="text-align: center"><img src="https://docs.realmcu.com/HoneyGUI/image/widgets/switch.gif" width= "400" /></div>
   <br>

API
---

.. doxygenfile:: gui_switch.h
