软件加速
========

总体流程图
----------

该流程图描绘了软件加速处理图像资源的流程。在处理图像时，根据图像的压缩状态和类型选择不同的处理方法：

- **覆盖 (cover)**: 将原始图像数据直接写入到帧缓冲区中的相应位置。不进行任何处理，只是直接覆盖。
- **旁路 (bypass)**: 将原始图像数据直接写入到帧缓冲区中的相应位置。旁路模式无法处理图像的透明度。它对整个图像应用全局不透明度值，从而影响整体透明度。在创建透明效果时，旁路模式相比源覆盖模式更占用空间。
- **滤黑 (Filter black)**: 滤波技术有效地从原始图像数据中筛选出像素值为零的像素数据，这意味着黑色像素不会被写入到帧缓冲区中。这种机制能够加快刷新速度。除黑色以外的任何颜色像素都会经过标准处理方法，并被记录到帧缓冲区中。
- **源覆盖 (Source_over)**: 一种混合方法，将图像的颜色数据和帧缓冲区像素的颜色数据结合起来，基于不透明度值 Sa 计算最终的颜色，并将其写入到帧缓冲区的相应位置。计算公式为 ((255 - Sa) * D + Sa * S) / 255，其中 ``Sa`` 是原始图像的不透明度值，``D`` 是帧缓冲区的像素数据，``S`` 是源图像的像素数据。

.. figure:: https://foruda.gitee.com/images/1726135723241666402/b6cb0cbc_13671125.png
       :align: center
       :width: 1200px
       :name: 图-sw_acc
        
       软件加速

- ``img_type`` 可以从图像的头部中获得，图像头部的结构如下：

.. literalinclude:: ../../../realgui/core/def_file.h
   :language: c
   :start-after: /* gui_rgb_data_head struct start*/
   :end-before: /* gui_rgb_data_head struct end*/

- ``img_type`` 的枚举值如下。如果值为 ``IMDC_COMPRESS``，则表示图像已压缩，并进入 :term:`RLE` 处理流程；否则，进入无 RLE 处理流程。

.. literalinclude:: ../../../realgui/engine/draw/draw_img.h
   :language: c
   :start-after: /* GUI_FormatType structure start*/
   :end-before: /* GUI_FormatType structure end*/

- 根据不同的混合模式 ``blend_mode`` 执行相应的 blit 过程。

.. literalinclude:: ../../../realgui/engine/draw/draw_img.h
   :language: c
   :start-after: /* BLEND_MODE_TYPE structure start*/
   :end-before: /* BLEND_MODE_TYPE structure end*/

- 当图像被压缩时，需要从压缩数据的地址中获取压缩头。该头部中的 ``algorithm_type`` 参数包含了实际的图像类型。压缩图像的类型在 ``imdc_src_type`` 结构体中描述。

.. literalinclude:: ../../../realgui/core/def_file.h
   :language: c
   :start-after: /* imdc_file_header struct start*/
   :end-before: /* imdc_file_header struct end*/
   
.. literalinclude:: ../../../realgui/core/def_file.h
   :language: c
   :start-after: /* imdc_src_type struct start*/
   :end-before: /* imdc_src_type struct end*/

无 RLE 覆盖模式概述
-------------------

以下流程描述了无 RLE 压缩图像的覆盖模式处理过程。根据图像矩阵和显示设备的像素字节数选择处理方法，并将其写入帧缓冲区。

.. figure:: https://foruda.gitee.com/images/1726135750546602965/e70749d0_13671125.png
       :align: center
       :width: 800px
       :name: 图-cover_mode_path
        
       Cover Mode Path

- 如果矩阵是单位矩阵，则执行没有矩阵操作的 blit 过程；否则，执行具有矩阵操作的 blit 过程。
- ``dc_bytes_per_pixel`` 表示显示设备的像素字节数，计算方式为 ``dc->bit_depth >> 3``，其中 ``bit_depth`` 为显示设备的位深度。以位深度为 24 的显示设备为例，其像素字节数为 3。

无 RLE 覆盖模式（不带矩阵变换）
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

下面的流程图描述了将未压缩图像写入帧缓冲区的覆盖模式处理过程，以 RGB565 为目标设备图像类型为例。

.. figure:: https://foruda.gitee.com/images/1726135919132573906/4e4cb2fd_13671125.png
       :align: center
       :width: 400px
       :name: 图-cover_blit_2_rgb565
        
       Cover_blit_2_rgb565

无 RLE 覆盖模式（带矩阵变换）
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

下面的流程图描述了使用覆盖模式（带矩阵变换）将未压缩图像写入帧缓冲区的过程，以 RGB565 为目标设备图像类型为例。

.. figure:: https://foruda.gitee.com/images/1726135926171202621/00c62b0c_13671125.png
       :align: center
       :width: 550px
       :name: 图-cover_matrix_blit_2_rgb565
        
       Cover_matrix_blit_2_rgb565

无 RLE 旁路模式概述
-------------------

以下流程描述了无 RLE 压缩图像的旁路模式处理过程。根据图像矩阵和显示设备的像素字节数选择处理方法，并将其写入帧缓冲区。

.. figure:: https://foruda.gitee.com/images/1726135755236374806/661c982d_13671125.png
       :align: center
       :width: 800px
       :name: 图-bypass_mode_path
        
       Bypass_mode_path

- 如果矩阵是单位矩阵，则执行没有矩阵操作的 blit 过程；否则，执行具有矩阵操作的 blit 过程。
- ``dc_bytes_per_pixel`` 表示显示设备的像素字节数，计算方式为 ``dc->bit_depth >> 3``，其中 ``bit_depth`` 为显示设备的位深度。以位深度为 24 的显示设备为例，其像素字节数为 3。
  
无 RLE 旁路模式（不带矩阵变换）
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

下面的流程图描述了将未压缩图像写入帧缓冲区的旁路模式处理过程，以 RGB565 为目标设备图像类型为例。

.. figure:: https://foruda.gitee.com/images/1726135755236374806/661c982d_13671125.png
       :align: center
       :width: 900px
       :name: 图-Bypass_blit_2_rgb565
        
       Bypass_blit_2_rgb565

1. 根据 ``img_type`` 执行不同的处理步骤。
2. 基于 ``opacity_value`` 执行相应的操作将图像像素写入帧缓冲区。

  - 如果 ``opacity_value`` 为 0，表示图像不显示，直接跳出处理流程。
  - 如果 ``opacity_value`` 为 255，将源图像像素转换为 RGB565 格式，并写入帧缓冲区。
  - 如果 ``opacity_value`` 介于 0 和 255 之间，执行 Alpha 混合操作将源图像像素与对应的帧缓冲区像素进行混合。混合公式为 ((255 - Sa) * D + Sa * S) / 255，将混合结果写入帧缓冲区。

无 RLE 旁路模式（带矩阵变换）
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

下面的流程图描述了使用混合模式（带矩阵变换）将未压缩图像写入帧缓冲区的过程，以 RGB565 为目标设备图像类型为例。

.. figure:: https://foruda.gitee.com/images/1726135932315321260/e1dc02cc_13671125.png
       :align: center
       :width: 900px
       :name: 图-Bypass_matrix_blit_2_rgb565
        
       Bypass_matrix_blit_2_rgb565

1. 根据 ``img_type`` 执行不同的处理步骤。
2. 执行矩阵计算，将目标区域的写入点映射到图像像素，并获取图像像素的像素值。
3. 基于 ``opacity_value`` 执行相应的操作将图像像素写入帧缓冲区。

    - 如果 ``opacity_value`` 为 0，表示图像不显示，直接跳出处理流程。
    - 如果 ``opacity_value`` 为 255，将源图像像素转换为 RGB565 格式，并写入帧缓冲区。
    - 如果 ``opacity_value`` 介于 0 和 255 之间，执行 Alpha 混合操作将源图像像素与对应的帧缓冲区像素进行混合。混合公式为 ((255 - Sa) * D + Sa * S) / 255，将混合结果写入帧缓冲区。

无 RLE 滤黑模式概述
--------------------

以下流程描述了无 RLE 压缩图像的滤黑模式处理过程。根据图像矩阵和显示设备的像素字节数选择处理方法，并将其写入帧缓冲区。

.. figure:: https://foruda.gitee.com/images/1726135914549506931/43192327_13671125.png
       :align: center
       :width: 900px
       :name: 图-Filter_mode_path
        
       Filter_mode_path

无 RLE 滤黑模式（不带矩阵变换）
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

下面的流程图描述了将未压缩图像写入帧缓冲区的滤黑模式处理过程，以 RGB565 为目标设备图像类型为例。

.. figure:: https://foruda.gitee.com/images/1726135936809407977/178a3356_13671125.png
       :align: center
       :width: 900px
       :name: 图-filter_blit_2_rgb565
        
       Filter_blit_2_rgb565


1. 根据 ``img_type`` 执行不同的处理步骤。
2. 如果像素值为 0，则跳过处理；否则，执行后续写入操作。
3. 基于 ``opacity_value`` 执行相应的操作将图像像素写入帧缓冲区。

    - 如果 ``opacity_value`` 为 0，表示图像不显示，直接跳出处理流程。
    - 如果 ``opacity_value`` 为 255，将源图像像素转换为 RGB565 格式，并写入帧缓冲区。
    - 如果 ``opacity_value`` 介于 0 和 255 之间，执行 Alpha 混合操作将源图像像素与对应的帧缓冲区像素进行混合。混合公式为 ((255 - Sa) * D + Sa * S) / 255，将混合结果写入帧缓冲区。

无 RLE 旁路模式（带矩阵变换）
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

下面的流程图描述了使用滤黑模式（带矩阵变换）将未压缩图像写入帧缓冲区的过程，以 RGB565 为目标设备图像类型为例。

.. figure:: https://foruda.gitee.com/images/1726135941645383326/65173b6c_13671125.png
       :align: center
       :width: 900px
       :name: 图-filter_matrix_blit_2_rgb565
        
       Filter_matrix_blit_2_rgb565

1. 根据 ``img_type`` 执行不同的处理步骤。
2. 执行矩阵计算，将目标区域的写入点映射到图像像素，并获取图像像素的像素值。
3. 如果像素值为 0，则跳过处理；否则，执行后续写入操作。
4. 基于 ``opacity_value`` 执行相应的操作将图像像素写入帧缓冲区。

  - 如果 ``opacity_value`` 为 0，表示图像不显示，直接跳出处理流程。
  - 如果 ``opacity_value`` 为 255，将源图像像素转换为 RGB565 格式，并写入帧缓冲区。
  - 如果 ``opacity_value`` 介于 0 和 255 之间，执行 Alpha 混合操作将源图像像素与对应的帧缓冲区像素进行混合。混合公式为 ((255 - Sa) * D + Sa * S) / 255，将混合结果写入帧缓冲区。

无 RLE 混合模式概述
-------------------

以下流程描述了无 RLE 压缩图像的源覆盖模式处理过程。根据图像矩阵和显示设备的像素字节数选择处理方法，并将其写入帧缓冲区。

.. figure:: https://foruda.gitee.com/images/1726135811742209771/c8bad88f_13671125.png
       :align: center
       :width: 1000px
       :name: 图-alpha_mode_path
        
       Alpha_mode_path

无 RLE 混合覆盖模式（不带矩阵变换）
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

下面的流程图描述了将未压缩图像写入帧缓冲区的源覆盖模式处理过程，以 RGB565 为目标设备图像类型为例。

.. figure:: https://foruda.gitee.com/images/1726135946825496906/bafaabe5_13671125.png
       :align: center
       :width: 500px
       :name: 图-alpha_blit_2_rgb565
        
       Alpha_blit_2_rgb565

基于 ``opacity_value`` 执行相应的操作将图像像素写入帧缓冲区。

  - 如果 ``opacity_value`` 为 0，表示图像不显示，直接跳出处理流程。
  - 如果 ``opacity_value`` 为 255，将源图像像素转换为 RGB565 格式，并写入帧缓冲区。
  - 如果 ``opacity_value`` 介于 0 和 255 之间，执行 ``rgb565_fast_blending`` 对源图像像素与相应的帧缓冲区像素进行混合，将混合结果写入帧缓冲区。

无 RLE 混合覆盖模式（带矩阵变换）
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

下面的流程图描述了使用源覆盖模式（带矩阵变换）将未压缩图像写入帧缓冲区的过程，以 RGB565 为目标设备图像类型为例。

.. figure:: https://foruda.gitee.com/images/1726135953438894385/7422d479_13671125.png
       :align: center
       :width: 500px
       :name: 图-alpha_matrix_blit_2_rgb565
        
       Alpha_matrix_blit_2_rgb565

1. 执行矩阵计算，将目标区域的写入点映射到图像像素，并获取图像像素的像素值。
2. 基于 ``opacity_value`` 执行相应的操作将图像像素写入帧缓冲区。

  - 如果 ``opacity_value`` 为 0，表示图像不显示，直接跳出处理流程。
  - 如果 ``opacity_value`` 为 255，将源图像像素转换为 RGB565 格式，并写入帧缓冲区。
  - 如果 ``opacity_value`` 介于 0 和 255 之间，执行 ``rgb565_fast_blending`` 对源图像像素与相应的帧缓冲区像素进行混合，将混合结果写入帧缓冲区。

RLE 覆盖模式概述
-----------------

以下流程描述了 RLE 压缩图像的覆盖模式处理过程。根据图像矩阵和显示设备的像素字节数选择处理方法，并将其写入帧缓冲区。

.. figure:: https://foruda.gitee.com/images/1726135823311485058/2b8f94bf_13671125.png
       :align: center
       :width: 800px
       :name: 图-rle_cover_mode_path
        
       RLE_cover_mode_path

RLE 覆盖模式（不带矩阵变换）
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

下面的流程图描述了将压缩图像写入帧缓冲区的覆盖模式处理过程，以 RGB565 为目标设备图像类型为例。

.. figure:: https://foruda.gitee.com/images/1726136024470398285/83f66a60_13671125.png
       :align: center
       :width: 700px
       :name: 图-rle_cover_blit_2_rgb565
        
       RLE_cover_blit_2_rgb565

1. 根据压缩数据头部的 ``img_type`` 执行不同的处理步骤。
2. 对压缩图像数据进行解压。
3. 将像素结果写入帧缓冲区。

RLE 覆盖模式（带矩阵变换）
~~~~~~~~~~~~~~~~~~~~~~~~~~~

下面的流程图描述了使用覆盖模式（带矩阵变换）将压缩图像写入帧缓冲区的过程，以 RGB565 为目标设备图像类型为例。

.. figure:: https://foruda.gitee.com/images/1726136030945132846/70e37d28_13671125.png
       :align: center
       :width: 700px
       :name: 图-rle_cover_matrix_blit_2_rgb565
        
       RLE_cover_matrix_blit_2_rgb565

1. 根据压缩数据头部的 ``img_type`` 执行不同的处理步骤。
2. 对压缩图像数据进行解压。
3. 进行矩阵计算，将目标区域的写入点映射到图像像素，并获得图像像素的像素值。
4. 将像素结果写入帧缓冲区。

RLE 旁路模式概述
------------------

以下流程描述了 RLE 压缩图像的旁路模式处理过程。根据图像矩阵和显示设备的像素字节数选择处理方法，并将其写入帧缓冲区。

.. figure:: https://foruda.gitee.com/images/1726135816836257523/3224601d_13671125.png
       :align: center
       :width: 800px
       :name: 图-rle_bypass_mode_path
        
       Rle_bypass_mode_path

RLE 旁路模式（不带矩阵变换）
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

下面的流程图描述了将旁路图像写入帧缓冲区的覆盖模式处理过程，以 RGB565 为目标设备图像类型为例。

.. figure:: https://foruda.gitee.com/images/1726136019654689664/085ffd2a_13671125.png
       :align: center
       :width: 900px
       :name: 图-rle_bypass_blit_2_rgb565
        
       RLE_bypass_blit_2_rgb565

1. 根据压缩数据头部的 ``img_type`` 执行不同的处理步骤。
2. 对压缩图像数据进行解压。
3. 基于 ``opacity_value`` 执行相应的操作将图像像素写入帧缓冲区。

  - 如果 ``opacity_value`` 为 0，表示图像不显示，直接跳出处理流程。
  - 如果 ``opacity_value`` 为 255，将源图像像素转换为 RGB565 格式，并写入帧缓冲区。
  - 如果 ``opacity_value`` 介于 0 和 255 之间，执行 Alpha 混合操作将源图像像素与对应的帧缓冲区像素进行混合。混合公式为 ((255 - Sa) * D + Sa * S) / 255，将混合结果写入帧缓冲区。

RLE 旁路模式（带矩阵变换）
~~~~~~~~~~~~~~~~~~~~~~~~~~

下面的流程图描述了使用旁路模式（带矩阵变换）将压缩图像写入帧缓冲区的过程，以 RGB565 为目标设备图像类型为例。

.. figure:: https://foruda.gitee.com/images/1726136035401068218/cd9d0da1_13671125.png
       :align: center
       :width: 45%
       :name: 图-rle_bypass_matrix_blit_2_rgb565
        
       RLE_bypass_matrix_blit_2_rgb565

1. 根据压缩数据头部的 ``img_type`` 执行不同的处理步骤。
2. 对压缩图像数据进行解压。
3. 进行矩阵计算，将目标区域的写入点映射到图像像素，并获得图像像素的像素值。
4. 基于 ``opacity_value`` 执行相应的操作将图像像素写入帧缓冲区。

  - 如果 ``opacity_value`` 为 0，表示图像不显示，直接跳出处理流程。
  - 如果 ``opacity_value`` 为 255，将源图像像素转换为 RGB565 格式，并写入帧缓冲区。
  - 如果 ``opacity_value`` 介于 0 和 255 之间，执行 Alpha 混合操作将源图像像素与对应的帧缓冲区像素进行混合。混合公式为 ((255 - Sa) * D + Sa * S) / 255，将混合结果写入帧缓冲区。

RLE 滤黑模式概述
------------------

以下流程描述了 RLE 压缩图像的滤黑模式处理过程。根据图像矩阵和显示设备的像素字节数选择处理方法，并将其写入帧缓冲区。

.. figure:: https://foruda.gitee.com/images/1726135828203631320/cee92853_13671125.png
       :align: center
       :width: 800px
       :name: 图-rle_filter_mode_path
        
       RLE_filter_mode_path

RLE 滤黑模式（不带矩阵变换）
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

下面的流程图描述了将压缩图像写入帧缓冲区的滤黑模式处理过程，以 RGB565 为目标设备图像类型为例。

.. figure:: https://foruda.gitee.com/images/1726136039315952991/89ee16cb_13671125.png
       :align: center
       :width: 900px
       :name: 图-rle_filter_blit_2_rgb565
        
       RLE_filter_blit_2_rgb565

1. 根据压缩数据头部的 ``img_type`` 执行不同的处理步骤。
2. 对压缩图像数据进行解压。
3. 如果像素值为 0，则跳过处理；否则，执行后续写入操作。
4. 基于 ``opacity_value`` 执行相应的操作将图像像素写入帧缓冲区。

  - 如果 ``opacity_value`` 为 0，表示图像不显示，直接跳出处理流程。
  - 如果 ``opacity_value`` 为 255，将源图像像素转换为 RGB565 格式，并写入帧缓冲区。
  - 如果 ``opacity_value`` 介于 0 和 255 之间，执行 Alpha 混合操作将源图像像素与对应的帧缓冲区像素进行混合。混合公式为 ((255 - Sa) * D + Sa * S) / 255，将混合结果写入帧缓冲区。

RLE 滤黑模式（带矩阵变换）
~~~~~~~~~~~~~~~~~~~~~~~~~~

下面的流程图描述了使用滤黑模式（带矩阵变换）将压缩图像写入帧缓冲区的过程，以 RGB565 为目标设备图像类型为例。

.. figure:: https://foruda.gitee.com/images/1726136044246530066/3cb24841_13671125.png
       :align: center
       :width: 900px
       :name: 图-rle_filter_matrix_blit_2_rgb565
        
       RLE_filter_matrix_blit_2_rgb565

1. 根据压缩数据头部的 ``img_type`` 执行不同的处理步骤。
2. 对压缩图像数据进行解压。
3. 进行矩阵计算，将目标区域的写入点映射到图像像素，并获得图像像素的像素值。
4. 如果像素值为 0，则跳过处理；否则，执行后续写入操作。
5. 基于 ``opacity_value`` 执行相应的操作将图像像素写入帧缓冲区。

  - 如果 ``opacity_value`` 为 0，表示图像不显示，直接跳出处理流程。
  - 如果 ``opacity_value`` 为 255，将源图像像素转换为 RGB565 格式，并写入帧缓冲区。
  - 如果 ``opacity_value`` 介于 0 和 255 之间，执行 Alpha 混合操作将源图像像素与对应的帧缓冲区像素进行混合。混合公式为 ((255 - Sa) * D + Sa * S) / 255，将混合结果写入帧缓冲区。

RLE 混合模式概述
----------------

以下流程描述了 RLE 压缩图像的源覆盖模式处理过程。根据图像矩阵和显示设备的像素字节数选择处理方法，并将其写入帧缓冲区。

.. figure:: https://foruda.gitee.com/images/1726135833249440419/4bf65309_13671125.png
       :align: center
       :width: 800px
       :name: 图-rle_alpha_mode_path
        
       RLE_alpha_mode_path

RLE 混合模式（不带矩阵变换）
~~~~~~~~~~~~~~~~~~~~~~~~~~~~

下面的流程图描述了将压缩图像写入帧缓冲区的源覆盖模式处理过程，以 RGB565 为目标设备图像类型为例。

.. figure:: https://foruda.gitee.com/images/1726136048376898937/05600e92_13671125.png
       :align: center
       :width: 900px
       :name: 图-rle_alpha_blit_2_rgb565
        
       RLE_alpha_blit_2_rgb565

1. 根据压缩数据头部的 ``img_type`` 执行不同的处理步骤。
2. 对压缩图像数据进行解压。
3. 基于 ``opacity_value`` 执行相应的操作将图像像素写入帧缓冲区。

  - 如果 ``opacity_value`` 为 0，表示图像不显示，直接跳出处理流程。
  - 如果 ``opacity_value`` 为 255，当源图像为 RGB565 格式时，直接将其写入帧缓冲区。否则，执行相应的混合操作，并将混合结果写入帧缓冲区。
  - 如果 ``opacity_value`` 介于 0 和 255 之间，执行适当的混合操作来混合源图像像素与相应的帧缓冲区像素，将混合结果写入帧缓冲区。

RLE 源覆盖模式（带矩阵变换）
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

下面的流程图描述了使用源覆盖模式（带矩阵变换）将压缩图像写入帧缓冲区的过程，以 RGB565 为目标设备图像类型为例。

.. figure:: https://foruda.gitee.com/images/1726136052909602952/42461659_13671125.png
       :align: center
       :width: 900px
       :name: 图-rle_alpha_matrix_blit_2_rgb565
        
       RLE_alpha_matrix_blit_2_rgb565


1. 根据压缩数据头部的 ``img_type`` 执行不同的处理步骤。
2. 对压缩图像数据进行解压。
3. 进行矩阵计算，将目标区域的写入点映射到图像像素，并获得图像像素的像素值。
4. 基于 ``opacity_value`` 执行相应的操作将图像像素写入帧缓冲区。

  - 如果 ``opacity_value`` 为 0，表示图像不显示，直接跳出处理流程。
  - 如果 ``opacity_value`` 为 255，当源图像为 RGB565 格式时，直接将其写入帧缓冲区。否则，执行相应的混合操作，并将混合结果写入帧缓冲区。
  - 如果 ``opacity_value`` 介于 0 和 255 之间，执行适当的混合操作来混合源图像像素与相应的帧缓冲区像素，将混合结果写入帧缓冲区。


.. note:: 
   在压缩的 source_over 矩阵模式下，rle_rgb888 和 rle_rgba8888 相当于输出 rle_rgb565。

支持的输入类型和输出类型
-------------------------

.. csv-table:: 
  :header: 输入类型, 输出类型
  :stub-columns: 1
  :align: center
  :name: 图-图片源输入输出类型

  RGB565, RGB565
  RGB888, RGB888
  ARGB8888, RLE_ARGB8888
  ARGB8565, ARGB8565
  RLE_RGB565, RLE_RGB565
  RLE_RGB888, RLE_RGB888
  RLE_ARGB8888, RLE_ARGB8888
  RLE_ARGB8565, RLE_ARGB8565
