=======================
二维码或条形码 (Qbcode)
=======================

二维码条形码控件可以用来生成或展示二维码和条形码，开发者可以设置二维码控件或者条形码控件的边框、位置和大小属性。生成二维码时，qbcode控件能处理文本和二进制数据；生成条形码时，则只能处理文本数据。二维码遵循QR Code Model 2标准，支持从1到40的各种规格，支持4种纠错等级和4种字符编码模式。默认情况下，二维码的纠错等级设置为最高级。对于条形码，它支持code128格式。

用法
-----
创建控件
~~~~~~~~
使用 :cpp:any:`gui_qbcode_create` 函数创建一个二维码或者条形码控件，具体创建的是二维码控件还是条形码控件，由参数 ``encodeType`` 决定。``encodeType`` 枚举类型如下：


.. literalinclude:: ../../../realgui/widget/gui_qbcode/gui_qbcode.h
   :language: c
   :start-after: /** @brief  T_QBCODE_ENCODE_TYPE structure */
   :end-before: /** @brief  T_QBCODE_DISPLAY_TYPE structure */


设置尺寸和边框
~~~~~~~~~~~~~~
使用 :cpp:any:`gui_qbcode_config` 函数来设置控件的尺寸和边框。

二维码编码注释
~~~~~~~~~~~~~~
二维码的编码数据默认情况下可以超过最大字节数，这时可以将最大版本修改为15，版本的修改可以参照 `文档 <https://www.qrcode.com/zh/about/version.html>`_ 。

示例
-----

.. literalinclude:: ../../../example/widget/qbcode/example_gui_qbcode.c
   :language: c
   :start-after: /* gui qbcode widget example start*/
   :end-before: /* gui qbcode widget example end*/


.. raw:: html

   <br>
   <div style="display: flex; justify-content: space-between;">
      <div style="width: 40%; text-align: center;">
         <img src="https://foruda.gitee.com/images/1753435459893270800/0b7b33c4_10737458.png" style="width: 100%;">
         <p style="margin: 5px 0 0 0; text-align: center;">二维码</p>
      </div>
      <div style="width: 40%; text-align: center;">
         <img src="https://foruda.gitee.com/images/1753435473847626594/f6192e1b_10737458.png" style="width: 100%;">
         <p style="margin: 5px 0 0 0; text-align: center;">条形码</p>
      </div>
   </div>
   <br>


API
-----

.. doxygenfile:: gui_qbcode.h


