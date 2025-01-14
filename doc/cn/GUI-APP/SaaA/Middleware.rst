
==========
中间件
==========


RVD导出了SaaA包。固件需要解析和播放它。

.. image:: https://foruda.gitee.com/images/1715938743160813708/833fbdab_10088396.png
   :alt: saaa.png
   :align: center

包
=======

.. list-table::
   :header-rows: 1

   * - 资源
     - XML
     - JavaScript
   * - 图片和字体文件等
     - 描述控件的初始嵌套树结构和特定参数
     - 自定义行为，如触发控件手势事件的行为、外围操作、打印日志等


* 包位于文件系统映像的 ``root/app`` 文件夹中，固件中的启动程序将遍历这些包并为每个包在屏幕上设置一个启动按钮。单击按钮启动相应的包。
启动器
========
* 启动器的实现在 ``realgui\SaaA\frontend_launcher.c`` 文件中。
* 它使用网格控件来布局应用程序的按钮。然后它迭代 ``app`` 文件夹，查找所有表示应用程序的XML文件。
* 启动器获取APP的标题和图标，并使用按钮控件来显示它们。注册按钮的点击事件是为了启动应用程序。

.. image:: https://foruda.gitee.com/images/1715938973907688018/ce054910_10088396.png
   :alt: launcher.png
   :align: center

XML
===
* APP包中的XML文件描述了控件的初始嵌套树结构和特定参数。
* 使用 ``realgui\3rd\ezXML`` 将XML转换为C语言数据格式。详细信息请参考 `ezXML SourceForge <https://ezxml.sourceforge.net/>`_ 。
* XML解析器的实现在该文件 ``realgui\DOM\XML_DOM.c``中。您可以在 ``XML syntax`` 页面上阅读语法说明。
* 根据语法规定，函数 ``foreach_create`` 使用递归策略遍历XML的每个标签，并将标签映射到控件，将标签的属性配置给控件。
* XML遍历完成后，在固件中实际上创建了一个C-APP，与直接使用C-APP API的结果没有区别。
* 然后将执行XML中提到的JavaScript文件。

.. image:: https://foruda.gitee.com/images/1727329150060233120/0353e15b_10088396.png
   :alt: xml.png
   :align: center

JavaScript
============
* JavaScript描述了自定义行为，例如触发控件手势事件、外围操作、打印日志等。
* 基于 ``realgui\3rd\js`` 上的JerryScript引擎，支持常见语法。详细信息请参考 `JerryScript <https://jerryscript.net/>`_ 。
* JavaScript解析器的实现在该文件夹 ``realgui\SaaA`` 中以.js开头的文件中。您可以在 ``JavaScript syntax`` 页面上阅读语法说明。
* 使用 ``DECLARE_HANDLER`` 将函数定义为JavaScript函数的C语言实现。
* 使用 ``REGISTER_METHOD`` 和 ``REGISTER_METHOD_NAME`` 将函数添加到JavaScript对象中，以便在脚本中使用它。
* 在JavaScript文件中，有一些变量定义、函数定义和函数调用。当应用程序启动时，如上所述，JavaScript文件将在XML解析结束时执行，其中的函数调用将被执行，主要是一些初始化行为和事件监听器的注册。
* 直到事件发生，那些事件的回调函数才会被执行。

.. image:: https://foruda.gitee.com/images/1727329518743167154/2b01e446_10088396.png
   :alt: js.png
   :align: center

示例
=======

进度条 API
------------

.. code-block:: javascript

   //Read and write the progress value of a progressbar tag called 'tag name'
   progressbar.getElementById('tag name')
   var progress = progressbar.progress(0.7)


定义一个进度条对象
-----------------------
实际上，该对象是添加到全局对象中的。使用全局对象的属性不需要显式调用全局对象。

.. code-block:: c

   jerry_value_t progress = jerry_create_object();
   js_set_property(global_obj, "progressbar", progress);

向进度条对象添加两个函数
-----------------------------

.. code-block:: c

   REGISTER_METHOD(progress, progress);
   REGISTER_METHOD(progress, getElementById);

定义两个函数
------------------

* ``progress`` 函数用于设置和读取进度条的进度。
* 输入形式参数在数组 ``args``中，其中第一个是进度数值。如果该参数存在，则表示需要设置进度。使用 ``jerry_get_number_value()`` 将 Javascript 参数转换为 C 语言变量。
* 返回值是要获取的进度，使用 ``jerry_create_number`` 将 C 语言变量转换为 Javascript变量。需要注意的是，这些 JavaScript 变量在 C 语言中的形式是无符号整数的索引。

.. code-block:: c

   DECLARE_HANDLER(progress)
   {
       gui_obj_t *obj = NULL;
       jerry_get_object_native_pointer(this_value, (void *)&obj, NULL);
       if (args_cnt >= 1 && jerry_value_is_number(args[0]))
       {
           gui_progressbar_set_percentage((void *)obj, jerry_get_number_value(args[0]));
       }
       float per = gui_progressbar_get_percentage((void *)obj);
       return jerry_create_number(per);
   }

* ``getElementById`` 函数用于获取标签的句柄。更多用法请参考 `getElementById on MDN <https://developer.mozilla.org/en-US/docs/Web/API/Document/getElementById>`_ 。
* 输入形式参数是标签的指定名称。使用 ``js_value_to_string`` 函数将 JS 形式的名称转换为 C 形式的 char 数组，并获取指针句柄，并将值赋给标签。与标准函数定义略有不同，此函数返回新实例化的标签。

.. code-block:: c

   DECLARE_HANDLER(getElementById)
   {
       if (args_cnt != 1 || !jerry_value_is_string(args[0]))
       {
           return jerry_create_undefined();
       }
       jerry_value_t global_obj = jerry_get_global_object();
       jerry_value_t app_property = js_get_property(global_obj, "app");
       gui_app_t *app = NULL;
       jerry_get_object_native_pointer(app_property, (void *)&app, NULL);
       gui_obj_t *widget = NULL;
       char *a = js_value_to_string(args[0]);
       gui_obj_tree_get_widget_by_name(&app->screen, a, &widget);
       gui_free(a);
       jerry_set_object_native_pointer(this_value, widget, NULL);
       jerry_release_value(global_obj);
       jerry_release_value(app_property);
       return jerry_create_undefined();
   }

灯控制
----------
本页展示了UI开关与外设开关之间的对应关系。

.. code-block:: javascript

   //IO P1_1 is set to low level
   var P1_1 = 9
   var LED1 = new Gpio(P1_1, 'out');
   LED1.writeSync(0)

灯开关数据
-----------------

.. list-table::
   :header-rows: 1

   * - 数据
     - 值类型
     - 简介
   * - gpio
     - 数字
     - 灯的索引
   * - 方向
     - 输出/输入
     - 信号方向
   * - 写入的值
     - 数字
     - 0 表示关闭 / 1 表示打开


- 更多信息请参考 `onoff npm package usage <https://www.npmjs.com/package/onoff#usage>`_ 。

GPIO 灯开关
-----------

- 获取 gpio 索引、方向和写入值；
- 使用 gpio 驱动的 ``drv_pin_mode()`` 和 ``drv_pin_write()`` 来操作灯。

MATTER 灯开关
-------------------

- 获取 gpio 索引、方向和写入值；
- 将数据转换为 MATTER 协议；
- 使用 ``matter_send_msg_to_app()`` 来操作灯。

MESH 灯开关
-----------------

- 获取 gpio 索引、方向和写入值；
- 将数据转换为 MESH 协议；
- 使用 ``matter_send_msg_to_app()`` 来操作灯。


下面的代码示例是基于 RTL87X2G 的 ``writeSync``  控制灯的实现。先获取 gpio 值和方向值，然后使用特定的驱动 API 来操作灯。

.. code-block:: c

   #ifdef RTL87x2G
   #define ENABLE_MATTER_SWITCH
   #define ENABLE_MESH_SWITCH
   #define ENABLE_GPIO_SWITCH
   #endif

   #if defined ENABLE_GPIO_SWITCH
   #include "rtl_gpio.h"
   #include "rtl_rcc.h"
   #include "drv_gpio.h"
   #include "drv_i2c.h"
   #include "drv_touch.h"
   #include "drv_lcd.h"
   #include "touch_gt911.h"
   #include "string.h"
   #include "trace.h"
   #include "utils.h"
   #endif

   #if defined ENABLE_MESH_SWITCH
   #include "app_msg.h"
   T_IO_MSG led_msg = {.type = IO_MSG_TYPE_LED_ON};
   T_IO_MSG led_off_msg = {.type = IO_MSG_TYPE_LED_OFF};
   #endif

   #if defined ENABLE_MATTER_SWITCH
   #endif

   DECLARE_HANDLER(writeSync)
   {
       gui_log("enter writeSync:%d\n", args[0]);
       if (args_cnt >= 1 && jerry_value_is_number(args[0]))
       {
           int write_value = jerry_get_number_value(args[0]);
           int gpio = -1;
           jerry_value_t v1;
           jerry_value_t v2;
           v1 = js_get_property(this_value, "gpio");
           v2 = js_get_property(this_value, "direction");
           gpio = jerry_get_number_value(v1);
           jerry_release_value(v1);
           char *direction = js_value_to_string(v2);
           jerry_release_value(v2);
           int mode = 0;

           if (gpio >= 0)
           {
               gui_log("gpio%d, %d, %d", gpio, mode, write_value);

               /**
                * GPIO
                */
               #ifdef ENABLE_GPIO_SWITCH
               if (!strcmp(direction, "out"))
               {
                   mode = PIN_MODE_OUTPUT;
               }
               else if (!strcmp(direction, "in"))
               {
                   mode = PIN_MODE_INPUT;
               }
               drv_pin_mode(gpio, mode);
               drv_pin_write(gpio, write_value);
               #endif

               /**
                * MESH
                */
               #ifdef ENABLE_MESH_SWITCH
               extern bool app_send_msg_to_apptask(T_IO_MSG *p_msg);
               if(write_value == 0){
                   led_msg.u.param = 0x64+gpio;
                   app_send_msg_to_apptask(&led_msg);}
               else
               {
                   led_off_msg.u.param = 0x64+gpio;
                   app_send_msg_to_apptask(&led_off_msg);
               }
               #endif

               /**
                * MATTER
                */
               #ifdef ENABLE_MATTER_SWITCH
               if (gpio >= 0)
               {
                   extern bool matter_send_msg_to_app(uint16_t sub_type, uint32_t param);
                   uint32_t param = gpio << 8 | write_value;
                   if (gpio != 49052)
                   {
                       //single
                       matter_send_msg_to_app(0, param);
                   }
                   else
                   {
                       //group
                       matter_send_msg_to_app(1, param);
                   }
               #endif
           }

           gui_free(direction);
       }
       return jerry_create_undefined();
   }




