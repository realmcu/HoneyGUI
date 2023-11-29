# GUI-APP: UI resource image

## Image format conversion
Convert pictures in various formats into RGB raw pictures
* Open the converter. Please download from [```ImageConvert\ImageConverter.exe```](https://wiki.realtek.com/display/BTMKT/BLE+Soc+Image+Convert+Tool) 
* The operation steps and detailed instructions are as follows: 


![Image description](https://foruda.gitee.com/images/1686896876324689552/b3be8646_10088396.png "converter.png")


### Configuration
- compress : Enable image compression
- scanMode : Select whether the scan direction is horizontal or vertical, BeeGUI only wants horizontal.
- Color Space : Select colorSpace (RGB565, RGBA, BINARY...), BeeGUI can display all of them.
- Color head : BeeGUI wants this head to display.

### Color Space

- RGB565: Colorful but with low rendering cost and storage. 2 bytes per pixel.


|  Red |  Green |  Blue |
|----------|------------|-----------|
|  5bit|  6bit  |  5bit |

- RGB : True color. 3 bytes per pixel.

|  Red |  Green |  Blue |
|----------|------------|-----------|
|  8bit|  8bit  |  8bit |
- RGBA : True color with opacity. Enhance the display quality with transparency effects. 4 bytes per pixel.

|  Red |  Green |  Blue |  Opacity |
|----------|------------|-----------|--------------|
|  8bit|  8bit  |  8bit |    8bit  |
- BINARY : Use one bit for a pixel.


























