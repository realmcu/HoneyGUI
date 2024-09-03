# Display

## Font Anti-Aliasing

- Poor font anti-aliasing and abnormal colored edges on white text.

When using font libraries with 2 bits or more, if the font anti-aliasing effect is poor and there are abnormal colors on the edges of the font or the font color is displayed incorrectly, it may be an issue with the endianness of the font rendering data. To diagnose this, try displaying the font in RGB single-channel colors. For example, set the font color to `gui_rgb(255,0,0,255)`. Normally, the text should appear red. If the text appears blue, there is an abnormality (this can also be identified with any colored text).