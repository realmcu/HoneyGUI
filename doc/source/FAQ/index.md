# FAQ
## Porting FAQ
- xx
- xx
- xx
## Specification FAQ
### 1. Graphics 
| Platform  | 8762D  |8772F   |87X2D |PC   |8763E|
|---|---|---|---|---|---|
| RGB565  |  Y | Y  | Y  | Y  | Y  |
|  RGB888 | N  | Y  |  Y |  Y | N  | 
|  RGBA8888 |  N | Y  |  Y | Y  | N  | 
|  SVG | N  | Y   |  N |  Y  | N  | 
| TTF | N  | Y   |  N |  Y  | N  | 
| DOT font  |  Y | Y  | Y  | Y  |Y  |
|  Vector Graphics | N  | Y   |  N |  Y  |N |
|  Linear gradient | N  | Y   |  N |  Y  |N |
|  Radial gradient | N  | N   |  N |  Y  |N |

### Memory usage
#### 8772F demo

```eval_rst
.. raw:: html

    <iframe src="https://drive.google.com/file/d/1qpLBVJRt2gnRR_cpuXc9L06VjqwvpzS0/preview" width="640" height="480" allow="autoplay"></iframe>
```
The memory consumption statistics of this demo are as follows
| Module | Cost  |
|---|---|
| Widget  |  14.56KB |
| Framebuffer  |  screenWidth*pixelBytes*Lines |
| Thread stack  |  10KB |

