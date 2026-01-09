.. _FAQ_Display_EN:

=======
Display
=======

Font
----------

Why is the font anti-aliasing effect poor or white font edges have abnormal colors?
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

When using font libraries with more than 2 bits, if the font anti-aliasing effect is poor and the font edges have abnormal colors or the font color display is abnormal, it may be due to endianness issues in the font rendering data.

Please verify by displaying RGB single-channel color fonts. For example, set the font color to ``gui_rgb(255,0,0,255)``. Normally it will display red. If the color is blue, it is abnormal (color fonts can all show the abnormality).

Image
----------

Why do black stripes appear after image rotation?
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

Black stripes may appear after image rotation because the pixel positions change during rotation, and pixels at the rotated positions are not filled, resulting in black stripes.

You can use the ``gui_img_set_quality`` function to enable image anti-aliasing calculation, which usually solves this problem.

If the problem persists, please contact the technical support team for assistance.

Screen Rotation
-----------

Does HoneyGUI support screen rotation?
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

HoneyGUI supports screen rotation functionality.

Screen rotation refers to rotating the screen by 90, 180, or 270 degrees, typically used to solve the problem of UI design resolution not matching the display screen dimensions.

For example, the UI design uses 480x800 resolution, but the display screen is 800x480 resolution. In this case, screen rotation is needed to match pixel dimensions.

HoneyGUI does not support real-time rotation within the UI, such as the portrait lock feature on phones or screen flipping when watching videos.

How is screen rotation implemented?
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

The current screen rotation functionality is a post-rendering process. After rendering, the generated image buffer is rotated. Its rendering process is completely consistent with normal mode.

What impact does screen rotation have on rendering frame rate?
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

Screen rotation will slightly reduce the frame rate.

How to enable screen rotation?
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

Since there are many implementation methods for screen rotation, and the most efficient rotation implementation may differ across platforms and applications, the SDK may not include screen rotation functionality implementation.

Users can refer to the technical documentation to make corresponding code modifications, or contact the technical team for assistance.

What rotation methods are available for screen rotation?
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

The essence of screen rotation is to adjust horizontally arranged pixel data to vertically arranged pixel data. Depending on hardware configuration, there are multiple implementation methods available.

Quick Decision Flow
""""""""""""""""""""

.. mermaid::
   :align: center

   flowchart TD
       Start[Select Rotation Solution] --> Screen{Screen supports<br/>rotation?}
       Screen -->|Yes| End1[Use Screen Rotation]
       Screen -->|No| GPU{Has rotation GPU?}
       GPU -->|Yes| RenderMode{Rendering mode?}
       GPU -->|No| CPURenderMode{Rendering mode?}
       
       RenderMode -->|Full-frame| Plan1[Solution 1:<br/>GPU Direct Rotation]
       RenderMode -->|Block-based| PushMode{Display mode?}
       PushMode -->|Block display| Plan1
       PushMode -->|Full-frame display| Compare1[Recommend Solution 1 or 2<br/>Test and compare]
       
       CPURenderMode -->|Block-based| CPUPushMode{Display mode?}
       CPUPushMode -->|Block display| Plan3[Solution 3:<br/>CPU Point-by-Point]
       CPUPushMode -->|Full-frame display| Plan4[Solution 4:<br/>CPU Column-wise]
       
       Plan1 --> End2[Complete]
       Compare1 --> End2
       Plan3 --> End2
       Plan4 --> End2
       End1 --> End2
       
       style End1 fill:#90EE90
       style Plan1 fill:#87CEEB
       style Compare1 fill:#FFD700
       style Plan3 fill:#FFB6C1
       style Plan4 fill:#FFB6C1

.. note::
   
   **Important Note**: Not all GPUs support rotation functionality. If your GPU does not support rotation, please select according to CPU solutions.

Detailed Rotation Solutions
""""""""""""""""""""""""""""

.. list-table:: 
   :widths: 15 10 30 45
   :header-rows: 1

   * - Solution
     - Execution Unit
     - Implementation Principle
     - Applicable Conditions & Features
   * - Solution 1:
     
       GPU Direct Rotation
     - GPU
     - GPU directly reads pixels from SRC, rotates and writes to DST
     - **Applicable**: Both SRC and DST are in high-speed area, or no extra high-speed cache
     
       **Features**: Simple implementation, GPU parallel processing, applicable to any range
   * - Solution 2:
     
       GPU Cached Rotation
     - GPU + DMA
     - GPU rotates to high-speed cache first, DMA copies to DST in parallel
     - **Applicable**: DST is in low-speed area and has extra high-speed cache
     
       **Features**: DMA parallel transfer, better performance than CPU copy, suitable for block-based scenarios
   * - Solution 3:
     
       CPU Point-by-Point Rotation
     - CPU
     - CPU reads SRC pixels point-by-point, rotates and writes to DST
     - **Applicable**: Both SRC and DST are in high-speed area
     
       **Features**: Simple implementation, applicable to any range, but average performance
   * - Solution 4:
     
       CPU Column-wise Rotation
     - CPU
     - CPU reads complete rows from SRC column-wise, rotates and writes to DST
     - **Applicable**: DST is in low-speed area
     
       **Features**: Reduces low-speed area write operations, improves efficiency, applicable to any range

Key Concepts
"""""""""""""

- **SRC (Source Buffer)**: Storage location of pixel data before rotation
- **DST (Destination Buffer)**: Storage location of pixel data after rotation
- **High-speed Area**: SRAM or storage medium with speed close to SRAM
- **Low-speed Area**: PSRAM or other storage medium significantly slower than SRAM
- **Any Range**: Solutions can be applied to full-frame or block-based, code logic is the same, only processing range differs

Typical Application Cases
""""""""""""""""""""""""""

**Case 1: GPU + Block-based Rendering + Full-frame Display**

Hardware Configuration:

- Has GPU with rotation support
- Block-based rendering, block area in high-speed SRAM
- Has another high-speed block cache area
- Needs output to low-speed PSRAM full-frame area (no high-speed full-frame cache)
- Full-frame display

Recommended Solution: Solution 2 (GPU Cached Rotation), compare with Solution 1

Reason: Block area is high-speed, GPU rotates and saves to another high-speed block cache first, then DMA copies to low-speed full-frame area in parallel. Although DMA parallel copy occupies the bus and affects other low-speed area access, it is usually better than direct writing to low-speed area. It is recommended to compare the performance of Solution 1 (GPU direct rotation to low-speed area) and Solution 2 on the actual platform.

**Case 2: GPU + Block-based Rendering + Block Display**

Hardware Configuration:

- Has GPU with rotation support
- Block-based rendering, block area in high-speed SRAM
- Block display, no full-frame needed

Recommended Solution: Solution 1 (GPU Direct Rotation)

Reason: Block rotation followed by immediate display, both source and destination are high-speed areas, GPU direct rotation has optimal performance, no extra cache and copy needed, minimal memory usage.

**Case 3: GPU + Full-frame Rendering + Full-frame Display**

Hardware Configuration:

- Has GPU with rotation support
- Full-frame rendering, both source and destination frame buffers in high-speed SRAM
- Full-frame display

Recommended Solution: Solution 1 (GPU Direct Rotation)

Reason: Since full-frame rendering is used, it indicates sufficient memory and all in high-speed area, GPU direct rotation has optimal performance.

**Case 4: CPU + Block-based Rendering + Full-frame Display**

Hardware Configuration:

- No GPU, CPU only
- Block-based rendering, block area in high-speed SRAM
- Has another high-speed block cache area
- Needs output to low-speed PSRAM full-frame area
- Full-frame display

Recommended Solution: Solution 4 (CPU Column-wise Rotation), compare with Solution 3

Reason: Destination is low-speed area, reading complete rows column-wise then writing reduces low-speed area write operations. Column-wise reading can be performed in high-speed block cache area, then batch written to low-speed full-frame area, more efficient than point-by-point rotation.

**Case 5: CPU + Block-based Rendering + Block Display**

Hardware Configuration:

- No GPU, CPU only
- Block-based rendering, block area in high-speed SRAM
- Block display, no full-frame needed

Recommended Solution: Solution 3 (CPU Point-by-Point Rotation)

Reason: Block rotation followed by immediate display, both source and destination are high-speed areas, using Solution 3 is simple to implement with acceptable performance.

Performance Optimization Recommendations
"""""""""""""""""""""""""""""""""""""""""

1. **Prioritize Hardware Acceleration**: If screen or GPU supports rotation, prioritize hardware solutions
2. **Reduce Low-speed Area Access**: When DST is in low-speed area, consider using high-speed cache transfer or column-wise reading algorithm
3. **Utilize DMA Parallelism**: When DMA is available, let DMA handle data transfer while CPU/GPU focuses on computation
4. **Actual Testing Verification**: Storage medium speeds vary greatly across platforms, recommend testing each algorithm's performance on target platform
5. **Algorithm Universality**: All algorithms support any range (full-frame or block-based), flexibly apply according to actual rendering mode
