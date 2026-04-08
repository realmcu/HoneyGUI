# HoneyGUI 脏块系统设计文档

**版本：** v2.1.1.0 | **文件：** `realgui/dc/`

---

## 一、核心概念

**脏块 = 需要重绘的矩形区域**

```
场景：左上角按钮 + 右下角文本都变化

┌─────────────────────────┐
│ ┌──┐矩形1               │  regions[0] = [10,10,60,60]
│ │●││                    │  regions[1] = [400,400,450,450]
│ └──┘                    │
│                          │  → 只重绘 5,000 像素
│              ┌──┐矩形2  │  → 节省 97% 性能
│              │●││       │
└──────────────└──┘───────┘
```

---

## 二、数据结构

```c
/* realgui/dc/gui_dirty_region.h */

#define GUI_MAX_DIRTY_REGIONS 16

typedef struct {
    gui_rect_t regions[GUI_MAX_DIRTY_REGIONS];  // 矩形数组
    uint16_t count;                              // 当前数量
    bool full_refresh;                           // 降级全屏标志
} gui_dirty_region_manager_t;
```

---

## 三、核心 API

```c
/* 标记一个矩形区域为脏块（用户唯一需要调用的 API） */
void gui_dirty_set_region(gui_rect_t *rect);
```

内部实现：裁剪到屏幕范围 → 尝试与已有区域合并（重叠/相邻） → 溢出时降级全屏 → 添加到 `regions[]`。

---

## 四、使用示例

### 4.1 按钮点击

```c
gui_rect_t rect = {btn->base.x, btn->base.y,
                   btn->base.x + btn->base.w - 1,
                   btn->base.y + btn->base.h - 1};
gui_dirty_set_region(&rect);
```

### 4.2 对象移动（标记两次）

```c
// 1. 标记旧位置
gui_rect_t old_rect = {obj->x, obj->y, obj->x + obj->w - 1, obj->y + obj->h - 1};
gui_dirty_set_region(&old_rect);
// 2. 修改坐标
obj->x = new_x; obj->y = new_y;
// 3. 标记新位置
gui_rect_t new_rect = {obj->x, obj->y, obj->x + obj->w - 1, obj->y + obj->h - 1};
gui_dirty_set_region(&new_rect);
```

**关键**：移动对象必须标记 **旧位置 + 新位置**。`gui_img` 已内置自动标记（见第八节）。

### 4.3 完整示例：移动图片

```c
void demo_move_image(void)
{
    gui_img_t *img = get_my_image();
    int16_t old_x = img->base.x, old_y = img->base.y;
    int16_t w = img->base.w, h = img->base.h;

    // 标记旧位置
    gui_rect_t old_rect = {old_x, old_y, old_x + w - 1, old_y + h - 1};
    gui_dirty_set_region(&old_rect);

    // 修改坐标
    img->base.x = 100; img->base.y = 200;

    // 标记新位置
    gui_rect_t new_rect = {100, 200, 100 + w - 1, 200 + h - 1};
    gui_dirty_set_region(&new_rect);
}
```

---

## 五、PFB 渲染流程

### 5.1 Section 扫描机制

```
屏幕 480×480, fb_height=20 → section_total=24

Section 0:  y=[0,19]   ─┐
Section 1:  y=[20,39]    │ 逐条扫描，每个 section：
...                       │ fb_clear → dc->section → obj_draw_scan → post_process → draw_border → lcd_update
Section 23: y=[460,479] ─┘
```

### 5.2 Stride 计算

渲染引擎写入偏移：
```c
offset = (y - dc->section.y1) * (dc->section.x2 - dc->section.x1 + 1) + (x - dc->section.x1)
```

**stride = section_width**（`dc->section.x2 - dc->section.x1 + 1`），**不是** `dc->fb_width`。

### 5.3 lcd_update 行为

`lcd_update` 只发送 `dc->section` 矩形到 LCD。**不在任何脏区域交集内的 LCD 区域永远不会被更新。**

### 5.4 多交集 Bounding Box 合并

同一 section 可能与多个脏区域有交集。多次 `obj_draw_scan` 会因 stride 不同互相覆盖。

**解决方案：** 合并所有交集为 bounding box，只调用一次 `obj_draw_scan`：

```c
gui_rect_t bbox = intersections[0];
for (k = 1; k < intersection_count; k++) {
    bbox.x1 = MIN(bbox.x1, intersections[k].x1);
    bbox.y1 = MIN(bbox.y1, intersections[k].y1);
    bbox.x2 = MAX(bbox.x2, intersections[k].x2);
    bbox.y2 = MAX(bbox.y2, intersections[k].y2);
}
dc->section = bbox;
obj_draw_scan(root);
```

---

## 六、工作流程

```
用户修改对象
  ↓
调用 gui_dirty_set_region(rect)
  ↓
矩形添加到 dirty_mgr.regions[]
  ↓
下一帧 gui_pfb_draw() 读取脏块数组
  ↓
dirty_border_prepare() 注入上一帧清理 rect
  ↓
逐 section 扫描：交集检测 → bounding box 合并 → 渲染 → 画边框 → lcd_update
  ↓
清空脏块数组
```

---

## 七、调试边框系统

### 7.1 功能

在每个脏块区域绘制 **1px 红色边框**，验证脏块标记正确性。

| 属性 | 值 |
|------|-----|
| 线宽 | 1px |
| 颜色 | 红色（RGB565: 0xF800, RGB888: 0x0000FF, ARGB8888: 0xFFFF0000） |
| 时机 | post_process 之后，lcd_update 之前 |
| 格式 | RGB565, RGB888, ARGB8888 |

### 7.2 边框残留问题

```
Frame N：脏区域 A=[100,100,200,200]
  → 画红色边框，lcd_update 写到 LCD ✓

Frame N+1：脏区域 B=[300,300,400,400]（A 消失）
  → A 所在 section 无交集 → skip
  → A 的红色边框永久残留在 LCD ✗
```

**根因：** `lcd_update` 只更新 `dc->section` 区域。section 被跳过时，LCD 上的旧边框无法清除。

### 7.3 解决方案：动态脏区域清理

将上一帧的边框位置注入为"清理脏区域"，使渲染引擎重绘这些区域。

**核心流程：**
```
Frame N:
  real_dirty = [A]
  → 保存 A 到 s_prev_regions
  → 画 A 的边框

Frame N+1:
  real_dirty = [B]
  → dirty_border_prepare:
      mgr->regions = [B, A_top, A_bottom, A_left, A_right]
      real_dirty_count = 1
  → A 的边线区域被 obj_draw_scan 重绘（正确内容覆盖旧边框）
  → 只画 B 的边框（real_dirty_count=1）
  → A 的旧边框被正确清除 ✓
```

**关键分离：**

| 操作 | 遍历范围 | 说明 |
|------|----------|------|
| 交集检测 | `mgr->count`（全部） | 清理 rect 参与，确保 section 不被跳过 |
| obj_draw_scan | `dc->section`（bbox） | 重绘所有交集区域，覆盖旧边框 |
| draw_dirty_border | `real_dirty_count`（仅真实脏区域） | 清理 rect 不画新边框 |

### 7.4 边线优化

不保存整个 rect，只保存 4 条 1px 边线：

```c
gui_rect_t edges[4] = {
    {r->x1, r->y1, r->x2, r->y1},  // top    (1px 高)
    {r->x1, r->y2, r->x2, r->y2},  // bottom (1px 高)
    {r->x1, r->y1, r->x1, r->y2},  // left   (1px 宽)
    {r->x2, r->y1, r->x2, r->y2},  // right  (1px 宽)
};
```

**性能对比（200×200 脏区域）：**
- 整个 rect 清理：40000 像素
- 边线清理：~1200 像素（**约 30 倍减少**）

### 7.5 容量限制

`GUI_MAX_DIRTY_REGIONS = 16`，每个上一帧脏区域需 4 个清理 rect：

```
当前帧 N 个脏区域 → 最多清理 (16 - N) / 4 个上一帧边框
例：当前 4 个脏区域 → 最多清理 3 个上一帧边框
```

### 7.6 API

```c
// 注入上一帧清理 rect，返回当前帧真实脏区域数
uint16_t dirty_border_prepare(gui_dirty_region_manager_t *mgr);

// 在 framebuffer 中绘制 rect 的红色边框
void draw_dirty_border(gui_dispdev_t *dc, gui_rect_t *rect);
```

**调用位置（gui_fb.c PFB 路径）：**
```c
uint16_t real_dirty_count = dirty_border_prepare(mgr);  // section 循环前

for (section_total) {
    // 交集检测 (mgr->count, 含清理rect)
    // bbox 合并 → obj_draw_scan → post_process
    for (j = 0; j < real_dirty_count; j++)              // 只画真实脏区域边框
        draw_dirty_border(dc, &mgr->regions[j]);
    lcd_update(dc);
}
```

---

## 八、gui_img 自动脏块标记

`gui_img` 在 prepare 阶段自动标记旧+新位置，无需手动调用。

**机制：** `last_render_rect` 缓存上一帧 AABB，checksum 变化时自动标记两个区域。

```c
// realgui/widget/gui_img/gui_img.c
if (last != _this->checksum) {
    draw_img_t *draw_img = _this->draw_img;
    gui_rect_t new_rect = {draw_img->img_target_x, draw_img->img_target_y,
                           draw_img->img_target_x + draw_img->img_target_w - 1,
                           draw_img->img_target_y + draw_img->img_target_h - 1};

    if (_this->last_render_rect.x2 >= _this->last_render_rect.x1 &&
        _this->last_render_rect.y2 >= _this->last_render_rect.y1)
    {
        gui_dirty_set_region(&_this->last_render_rect);  // 旧位置
    }
    gui_dirty_set_region(&new_rect);                      // 新位置
    _this->last_render_rect = new_rect;
    gui_fb_change();
}
```

**场景：图片旋转 0° → 45°**
```
N 帧：渲染区域 [100,100,150,150] → 缓存到 last_render_rect
N+1 帧：旧 [100,100,150,150] + 新 [85,85,165,165] → 两个脏块
结果：旧图清除，新图正确绘制，无残留
```

---

## 九、业界参考

### 9.1 Arm-2D

- **`bDebugDirtyRegions`：** 强制全屏刷新（简单但性能差）
- **动态脏矩阵：** `tOldRegion`/`tNewRegion` + 最小包络 + `bUpdated` 标志
- **`bDirtyRegionDebugModeSkipFrame`：** 无脏区域时的边缘处理

### 9.2 LVGL

- **`LV_USE_REFR_DEBUG`：** 半透明随机色块叠加在重绘对象上（标记对象而非脏区域）
- 无残留问题：每个脏区域完整重绘

---

## 十、文件清单

| 文件 | 职责 |
|------|------|
| `realgui/dc/gui_dirty_region.h` | 脏区域管理器定义，`gui_dirty_set_region()` |
| `realgui/dc/gui_dirty_region.c` | 脏区域管理实现 |
| `realgui/dc/gui_dirty_border.h` | 调试边框 API：`draw_dirty_border()`, `dirty_border_prepare()` |
| `realgui/dc/gui_dirty_border.c` | 边框绘制 + 动态清理机制 |
| `realgui/dc/gui_fb.c` | PFB/DC_SINGLE 渲染主循环，集成边框与脏区域逻辑 |
| `realgui/widget/gui_img/gui_img.c` | gui_img 自动脏块标记（`last_render_rect`） |

---

## 十一、已知限制与待改进

| 项目 | 状态 | 说明 |
|------|------|------|
| 边线清理优化 | ✅ 完成 | 4 条 1px 边线代替整个 rect |
| 多交集 bbox 合并 | ✅ 完成 | 避免 stride 冲突 |
| full_refresh → dirty 过渡 | ⚠️ 待处理 | full_refresh 帧的全屏边框可能残留 |
| 无脏区域帧 | ⚠️ 待处理 | 类似 Arm-2D 的 SkipFrame 机制 |
| 容量不足 | ⚠️ 潜在 | 可增大 `GUI_MAX_DIRTY_REGIONS` 或独立缓冲区 |
| DC_SINGLE 模式 | ⚠️ 待处理 | `gui_fb_draw` 路径可能需类似处理 |

---

## 十二、用户责任 vs 系统责任

**用户：**
- 主动调用 `gui_dirty_set_region()`
- 移动对象标记两次（旧 + 新）

**系统：**
- 存储/裁剪/溢出降级
- PFB section 扫描与交集计算
- 调试边框绘制与自动清理
- `gui_img` 自动标记
