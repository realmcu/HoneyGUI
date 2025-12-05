=======
Qbcode
=======

The qbcode widget can be used to create or display qrcode and barcode. You can set the border, location and size attribute for qbcode widget. Qbcode widget support text and binary data for qrcode encode, and support text for barcode encode. The qrcode widget follows the QR Code Model 2 standard, supports various specifications from 1 to 40, and supports 4 error correction levels and 4 character encoding modes. By default, the error correction level of QR codes is set to the highest level. For barcode, it supports the code128 format.

Usage
-----
Create Widget
~~~~~~~~~~~~~~
To create a qbcode widget, you can use the function :cpp:any:`gui_qbcode_create`. Whether a qrcode or barcode is created depends on the ``encodeType`` parameter. The ``encodeType`` enumeration types are as follows:


.. literalinclude:: ../../../realgui/widget/gui_qbcode/gui_qbcode.h
   :language: c
   :start-after: /* T_QBCODE_ENCODE_TYPE structure start */
   :end-before: /* T_QBCODE_ENCODE_TYPE structure end */


Set Size and Border
~~~~~~~~~~~~~~~~~~~~
Use the function :cpp:any:`gui_qbcode_config` to set the size and border of qbcode.

Qrcode Encode Notes
~~~~~~~~~~~~~~~~~~~~
The encoded data for qrcode can exceeded the max bytes by default, which can modify the max version is 15. You can modify the version according to the `document <https://www.qrcode.com/zh/about/version.html>`_ .

Example
--------

.. literalinclude:: ../../../example/widget/qbcode/example_gui_qbcode.c
   :language: c
   :start-after: /* gui qbcode widget example start*/
   :end-before: /* gui qbcode widget example end*/


.. raw:: html

   <br>
   <div style="display: flex; justify-content: space-between;">
      <div style="width: 40%; text-align: center;">
         <img src="https://foruda.gitee.com/images/1753435459893270800/0b7b33c4_10737458.png" style="width: 100%;">
         <p style="margin: 5px 0 0 0; text-align: center;">Qrcode</p>
      </div>
      <div style="width: 40%; text-align: center;">
         <img src="https://foruda.gitee.com/images/1753435473847626594/f6192e1b_10737458.png" style="width: 100%;">
         <p style="margin: 5px 0 0 0; text-align: center;">Barcode</p>
      </div>
   </div>
   <br>



API
-----

.. doxygenfile:: gui_qbcode.h

