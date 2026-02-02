.. _Video:

==========
Video
==========

The Video widget is designed for video display and playback. It supports various video formats and data sources (Memory, File System, and FTL Storage), and provides a comprehensive set of APIs for playback control.


Usage
-----

Video Formats
~~~~~~~~~~~~~~~~~~~~~~~~
The Video widget supports the MJPEG, AVI, and H264 formats. The characteristics of each format are described below:


.. table:: Comparison of Different Video Formats
   :widths: 15 30 10 15 15 16 13 18
   :align: center

   +---------------+----------------------------+----------+---------------+---------------+----------------+-------------+------------------+
   | Type          | Resource                   |  Audio   | Transparency  | Decoding Cost | Frame Skipping |  Encoding   |     Decoder      |
   +===============+============================+==========+===============+===============+================+=============+==================+
   | `MJPEG`       |  Small                     |    ❌    |      ❌       | Low           |      ✔️        | JPEG        | HW(RTL8773G)/SW  |
   +---------------+----------------------------+----------+---------------+---------------+----------------+-------------+------------------+
   | `AVI`         | Slightly larger than MJPEG |    ✔️    |      ❌       | Low           |      ✔️        | JPEG        | HW(RTL8773G)/SW  |
   +---------------+----------------------------+----------+---------------+---------------+----------------+-------------+------------------+
   | `H264`        |  Very Small                |    ❌    |      ❌       | High          |      ❌        | H264        | SW               |
   +---------------+----------------------------+----------+---------------+---------------+----------------+-------------+------------------+


1. Video File Resource Size 
The file size of a video resource is influenced by multiple factors, including duration, frame rate, resolution, scene complexity, and the presence of audio. Higher scene complexity—characterized by high-frequency content with significant changes or detailed textures—typically results in larger exported file sizes.
During video conversion, appropriate quality parameters can be configured to balance the trade-off between storage space and image quality. Exported AVI files can be previewed using standard media players on PC platforms.

2. Video Resource Conversion 
Use the conversion tool to generate video formats compatible with HoneyGUI:

  a. The tool supports importing common video formats as well as image sequences.
  b. Identical quality parameter settings are not equivalent across different export formats.

For detailed instructions on video resource conversion, please refer to the conversion tool documentation.


Access Methods
~~~~~~~~~~~~~~~~~~~~~~~~
The Video widget supports various resource access methods:

- **Direct Mapping**: Reads directly from memory addresses (e.g., RAM/PSRAM, NOR Flash XIP).
- **File System**: Reads via file system interfaces (e.g., LittleFS).
- **Buffered Access**: Reads from storage addresses via a buffer (e.g., NAND Flash).

When creating a Video widget, the appropriate creation API must be selected based on the specific resource access method.

.. table:: Video Creation API
   :widths: 18 145
   :align: center

   +------------------+----------------------------------------------------------------------------------------------------------------------------------------------+
   |  Type            |  Creation API                                                                                                                                |
   +==================+==============================================================================================================================================+
   |  Direct Mapping  |  :cpp:any:`gui_video_t *gui_video_create_from_mem(void *parent, const char *name, void *addr, int16_t x, int16_t y, int16_t w, int16_t h);`  |
   +------------------+----------------------------------------------------------------------------------------------------------------------------------------------+
   |  File System     |  :cpp:any:`gui_video_t *gui_video_create_from_fs(void *parent, const char *name, void *addr, int16_t x, int16_t y, int16_t w, int16_t h);`   |
   +------------------+----------------------------------------------------------------------------------------------------------------------------------------------+
   |  Buffered Access |  :cpp:any:`gui_video_t *gui_video_create_from_ftl(void *parent, const char *name, void *addr, int16_t x, int16_t y, int16_t w, int16_t h);`  |
   +------------------+----------------------------------------------------------------------------------------------------------------------------------------------+



Playback Control
~~~~~~~~~~~~~~~~~~~~~~~~

Playback State
^^^^^^^^^^^^^^^^^^^^^^^^^^^^
Use the function :cpp:any:`gui_video_set_state` to set the video playback state. Supported states include Playing (:cpp:any:`GUI_VIDEO_STATE_PLAYING`), Paused (:cpp:any:`GUI_VIDEO_STATE_PAUSE`), and Stopped (:cpp:any:`GUI_VIDEO_STATE_STOP`).

If the video is set to Playing (:cpp:any:`GUI_VIDEO_STATE_PLAYING`) while in the Stopped state (:cpp:any:`GUI_VIDEO_STATE_STOP`), playback will restart from the first frame.


Repeat Playback
^^^^^^^^^^^^^^^^^^^^^^^^
Use the function :cpp:any:`gui_video_set_repeat_count` to set the number of times the video repeats. For example, setting the repeat count to 1 results in the video playing twice (initial play + 1 repeat).

Set the count to :cpp:any:`GUI_VIDEO_REPEAT_INFINITE` or a negative value to enable infinite looping.


Frame Rate Settings
^^^^^^^^^^^^^^^^^^^^^^^
The Video widget uses the frame rate defined in the resource file as the initial playback rate. Since MJPEG files do not contain frame rate information, a default of 25 fps is used. AVI and H264 files include frame rate information.

Use the function :cpp:any:`gui_video_set_frame_rate` to set the playback frame rate, which will override the original frame rate setting.

Use the function :cpp:any:`gui_video_get_frame_time` to retrieve the actual frame interval time.


Step Settings
^^^^^^^^^^^^^^^^^^^^^^^^
For video formats that support frame skipping, use the function :cpp:any:`gui_video_set_frame_step` to set the playback frame step. This achieves a fast-forward effect. Reverse playback is not supported.

Use the function :cpp:any:`gui_video_get_frame_step` to retrieve the current playback step of the video widget.



UI Transformations
~~~~~~~~~~~~~~~~~~~~~~~~
Scaling
^^^^^^^^^^^^^^
Use the function :cpp:any:gui_video_set_scale to set the scaling parameters for the video display, allowing adjustment of the video aspect ratio.





Example
---------
The following code snippet demonstrates how to create a Video widget, configure the frame rate, control playback, register interaction events, and adjust playback speed.


.. literalinclude:: ../../../example/widget/video/example_video.c
    :language: c
    :start-after: /* gui video widget example start*/
    :end-before: /* gui video widget example end*/

    

.. raw:: html

   <br>
   <div style="text-align: center"><img src="https://foruda.gitee.com/images/1770019653434266312/5aa9c3c8_9218678.webp" width= "400" /></div>
   <br>


API
------

.. doxygenfile:: gui_video.h









