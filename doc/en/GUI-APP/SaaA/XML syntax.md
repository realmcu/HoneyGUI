# XML syntax
## Element
- Element corresponding widget
- Element's attributes corresponding widget's attributes (0 value can be ignored)
- text content is the widget instance's name
```xml
<type a1="xx" a2="xx" a3="xx" a4="xx">name</type> 
```
## Nesting
The nesting structure of elements is consistent with the nesting structure of actual widgets.

```eval_rst
.. raw:: html

    <style>
    .center {
        display: flex;
        justify-content: center;
    }
    </style>

.. raw:: html

    <div class="center">

.. mermaid::
 
  graph TD;
      fatherName --> childName1;
      fatherName --> childName2;
      childName2 --> childName3;
      childName2 --> childName4;

.. raw:: html

    </div>
```

```xml
<fatherType a1="xx" a2="xx" a3="xx" a4="xx">fatherName 

    <childType a1="xx" a2="xx" a3="xx">childName1</childType> 
    <childType a1="xx" a2="xx" a3="xx">childName2
        <childType a1="xx" a2="xx" a3="xx">childName3</childType> 
        <childType a1="xx" a2="xx" a3="xx">childName4</childType> 
    </childType> 
</fatherType> 
```
## Specifications

| Element | Attributes |   |  |   |  | | | | | ||||||||
|----------|:-----------|:-----------|:-----------|:---|:---|:---|:---|:---|:---|:---|:---|:---|:---|:---|:---|:---|:---|
| win |x | y | w | h |hidden|
| textbox|x | y | w | h |text|font|fontSize|color|mode|inputable|
| img|x | y | w | h |scaleX|scaleY|rotationAngle|blendMode|opacity|file|folder|duration|
| seekbar |x | y | w | h |folder|picture|orientation|centralX|centralY|startDegree|endDegree|reverse|blendMode|opacity|
| tabview |x | y | w | h | transition|
| tab |x | y | w | h | idx|idy|
| curtainview |x | y | w | h | transition|
| curtain |x | y | w | h | scope|orientation|transition|
| icon |x | y | w | h | font|picture|highlightPicture|fontColor|fontSize|text|textX|textY|pictureX|pictureY|mode|blendMode|opacity|
| script |file|
| switch |x | y | w | h | picture|highlightPicture|clickedPicture|clickedHighlightPicture|pictureX|pictureY|blendMode|opacity|mode|duration|
| page |x | y | w | h | 
| screen |w | h | 
| grid |x | y | rowNumber | colNumber | rowGap|colGap|
| gallery |x | y | w | h | folder|mainBg|centerBg|centerPercent|sideScale|sidePosPercent|blendMode|opacity|
| animateTransform |type | from | to|  dur| repeatCount|
| motorizedCurtain |x | y | w | h | switchOpen|switchClose|switchPause|
| keyboard |ime|
| multiLevel |
| onClick |type | to | id1 | id2 | 
| backIcon |x | y | w | h | picture|highlightPicture|

| Attribute | brief |  values |  |   |  | | | | | ||||||||
|----------|:-----------|:-----------|:-----------|:---|:---|:---|:---|:---|:---|:---|:---|:---|:---|:---|:---|:---|:---|
| x |Relative left|number|
| y |Relative up|number|
| w|Width|number|
| h|Height |number|
| hidden|Not display |hidden|
| text|Text string| string |
| font |Font file|file path|
| fontSize |Font size | number|
| color |RGB hexadecimal color|#RRGGBB|
| mode(textbox)|text effect| truncate| verticalscroll | horizontalscroll  | transition|
| mode(icon)|press highlight| normal| fade | scale  | fadeScale|array|
| mode(switch)|press highlight| array|
| inputable |Soft heyboard | boolean|
| scaleX |Horizontal scaling rate | number|
| scaleY |Vertical scaling rate|number|
| rotationAngle |Rotation angle | number|
| blendMode |Image blending modes | imgBypassMode | imgFilterBlack| imgSrcOverMode | imgCoverMode|
| opacity |Opacity 0~255| number | 
| file |File path | string|
| folder |Folder path | string|
| duration |Animation duration in ms | number|
| picture |Image file path | string|
| orientation(seekbar) |Orientation|vertical|V|horizontal|H|arc|
| orientation(curtain) |middle|up|down|left|right|
| centralX |arc's center| number|
| centralY |arc's center| number|
| startDegree |arc's start degree |number | 
| endDegree |arc's end degree |number | 
| transition |Transition Effects|normal|fade|scale|fadeScale|
| idx |Horizontal index|number|
| idy |Vertical index|number|
| scope |Scope rate(0~1)|number|
| highlightPicture |highlight image file path |string|
| fontColor |RGB hexadecimal color|#RRGGBB|
| textX |Text relative left|number|
| textY |Text relative up|number|
| pictureX |Picture relative left|number|
| pictureY |Picture relative up|number|
| rowNumber |Number of rows|number|
| colNumber |Number of columns|number|
| rowGap |Gap between rows|number|
| colGap |Gap between columns|number|
|mainBg|centerBg|centerPercent|sideScale|sidePosPercent|
| mainBg |Main background image file path|string| 
| centerBg |background image of the center main image file path|string| 
| centerPercent |Proportion of the center area|number|
| sideScale |Default scaling ratio of the side image|number|
| sidePosPercent |Percentage position of the side image|number|
| type(animateTransform) |Animation type|rotate|
| from |Animation starting value|number|
| to(animateTransform) |Animation target value|number|
| dur |Animation duration|number|
| repeatCount |Animation repeat count|number|
| switchOpen|switchClose|switchPause|
| switchOpen |Motorized curtain's opening button's tag name|string|
| switchClose |Motorized curtain's closeing button's tag name|string|
| switchOpen |Motorized curtain's pausing button's tag name|string|
| ime|input method editor|null/pinyin|
|type(onClick)| to | id1 | id2 | 
| type(onClick)|action triggered by the click event|jump|control|
| type(onClick)|action triggered by the click event|jump|control|
| to(onClick) |target|light|multiLevel|
| id1 |main parameter|number|
| id2 |Secondary parameter|number|

## Example
### win 
```xml
<win
    x="0"
    y="0"
    w="480"
    h="480">cool_win
</win>
```
### img 
```xml
<img
    x="80"
    y="70"
    w="303"
    h="239"
    opacity="255"
    file="app/box/resource/new_folder/aa2.bin"
    blendMode="imgFilterBlack"
    rotationAngle="0"
    scaleX="1"
    scaleY="1">image3
</img>
```







