LCD芯片
*************

以下表格列出了主流芯片支持的与LCD相关的接口。如果您想了解更多信息，请点击特定芯片的名称。

===============  =======  ========  =======  =======  =======
SOC              I8080    QSPI      RGB      MIPI     SPI      
---------------  -------  --------  -------  -------  ------- 
`RTL8762C`_      Y        NA        NA       NA       Y         
`RTL8762D`_      Y        Y         NA       NA       Y         
`RTL8763E`_      Y        Y         NA       NA       Y         
`RTL8772G`_      Y        Y         Y        NA       Y        
`RTL8773E`_      Y        Y         Y        NA       Y        
`RTL8772F`_      Y        Y         Y        Y        Y         
===============  =======  ========  =======  =======  =======

以下表格列出了主流芯片支持的与LCD相关的驱动IC。如果您想了解更多信息，请点击特定芯片的名称。

===============  =========  ==========  =========  =========  =========  ==========  ==========  ==========  ========  ========  ========  ========  ========
SOC              EK9716     ICNA3311    NT35510    NV3047     ST7701S    ST77903     ST7796      OTM8009A    SH8601A   SH8601Z   RM69330   ST7789    NV3041A
---------------  ---------  ----------  ---------  ---------  ---------  ----------  ----------  ----------  --------  --------  --------  --------  --------
`RTL8762D`_      NA         NA          NA         NA         NA         NA          Y           NA          NA        NA        Y         Y         Y
`RTL8763E`_      NA         NA          Y          NA         NA         NA          NA          NA          NA        Y         NA        NA        NA           
`RTL8772G`_      Y          Y           Y          Y          Y          Y           Y           NA          NA        NA        NA        NA        NA
`RTL8773E`_      NA         NA          NA         NA         NA         NA          NA          NA          Y         NA        NA        NA        NA
`RTL8772F`_      NA         Y           Y          NA         Y          NA          NA          Y           Y         Y         NA        NA        NA
===============  =========  ==========  =========  =========  =========  ==========  ==========  ==========  ========  ========  ========  ========  ========

'Y' 表示驱动程序已包含在库中。
'NA' 表示驱动程序尚未包含在库中。

.. _RTL8762C: https://www.realmcu.com/en/Home/Product/93cc0582-3a3f-4ea8-82ea-76c6504e478a
.. _RTL8762D: https://www.realmcu.com/en/Home/Product/52feef61-22d0-483e-926f-06eb10e804ca
.. _RTL8763E: https://www.realmcu.com/en/Home/Product/eed7a243-66bf-4b5c-b811-a60d2d4e95cf
.. _RTL8772G: https://www.realmcu.com/en/Home/Product/c175760b-088e-43d9-86da-1fc9b3f07ec3
.. _RTL8773E: https://www.realmcu.com/en/Home/Product/eed7a243-66bf-4b5c-b811-a60d2d4e95cf
.. _RTL8772F: https://www.realmcu.com/en/Home/Product/c175760b-088e-43d9-86da-1fc9b3f07ec3

