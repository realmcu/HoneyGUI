# XML syntax
## Element
- Element corresponding widget
- Element's attributes corresponding widget's attributes (0 value can be ignored)
- text content is the widget instance's name
```xml
<type a1="xx" a2="xx" a3="xx" a4="xx">name</type> 
```
## Nesting
The nesting structure of elements is consistent with the nesting structure of actual widgets

```eval_rst

.. mermaid::
 
  graph TD;
      fatherName --> childName1;
      fatherName --> childName2;
      childName2 --> childName3;
      childName2 --> childName4;
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
## Attributes table

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




