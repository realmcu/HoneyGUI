.. _FAQ_Display_EN:

========
Display
========

Font Anti-Aliasing
------------------

- Poor font anti-aliasing and abnormal colored edges on white text.

When using font libraries with 2 bits or more, if the font anti-aliasing effect is poor and there are abnormal colors on the edges of the font or the font color is displayed incorrectly, it may be an issue with the endianness of the font rendering data.
To diagnose this, try displaying the font in RGB single-channel colors.
For example, set the font color to ``gui_rgb(255,0,0,255)``.
Normally, the text should appear red. If the text appears blue, there is an abnormality (this can also be identified with any colored text).

Image Display
----------

- Why do black stripes appear after rotating an image?

  Black stripes may appear after rotating an image because the position of the pixels changes during rotation, and the pixels in the new positions are not filled, resulting in black stripes.
  You can use the ``gui_img_set_quality`` function to enable image anti-aliasing, which usually resolves this issue.
  If the problem persists, you can contact the technical support team for assistance.