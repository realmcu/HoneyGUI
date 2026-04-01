# HoneyGUI 脏块优化设计

**设计原则：用户通过 API 主动标记脏块**

---

## 一、核心概念

### 1.1 什么是脏块？

**脏块 = 需要重绘的矩形区域**

- ❌ 不是一个矩形扩展
- ✅ 是多个独立矩形的数组

```
场景：左上角按钮 + 右下角文本都变化

✅ 标记两个独立矩形：
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
    gui_rect_t regions[16];  // 矩形数组
    uint32_t count;          // 当前数量
    bool full_refresh;       // 降级全屏标志
} gui_dirty_region_manager_t;

static gui_dirty_region_manager_t dirty_mgr;  // 全局管理器
```

---

## 三、核心 API

### 唯一的标记 API

```c
/* 标记一个矩形区域为脏块 */
void gui_fb_change_region(gui_rect_t *rect);
```

**这是用户唯一需要调用的 API**

---

## 四、API 实现

```c
/* realgui/dc/gui_dirty_region.c */

void gui_fb_change_region(gui_rect_t *rect)
{
    if (rect == NULL) return;
    
    fb_change = true;  // 触发刷新
    
    // 1. 裁剪到屏幕范围
    gui_dispdev_t *dc = gui_get_dc();
    rect->x1 = MAX(0, rect->x1);
    rect->y1 = MAX(0, rect->y1);
    rect->x2 = MIN(rect->x2, dc->screen_width - 1);
    rect->y2 = MIN(rect->y2, dc->screen_height - 1);
    
    // 检查是否完全在屏幕外
    if (rect->x1 > rect->x2 || rect->y1 > rect->y2) {
        return;
    }
    
    // 2. 检查是否溢出
    if (dirty_mgr.count >= GUI_MAX_DIRTY_REGIONS) {
        dirty_mgr.full_refresh = true;  // 降级全屏
        return;
    }
    
    // 3. 添加到数组
    dirty_mgr.regions[dirty_mgr.count] = *rect;
    dirty_mgr.count++;
}
```

---

## 五、用户如何使用

### 使用模式：用户主动标记

```c
/* 用户代码 - 需要显式调用 API 标记脏块 */

void on_button_click(gui_button_t *btn)
{
    // 1. 修改按钮状态
    btn->state = BUTTON_PRESSED;
    
    // 2. 计算按钮区域
    gui_rect_t btn_rect = {
        .x1 = btn->base.x,
        .y1 = btn->base.y,
        .x2 = btn->base.x + btn->base.w - 1,
        .y2 = btn->base.y + btn->base.h - 1
    };
    
    // 3. 主动标记脏块
    gui_fb_change_region(&btn_rect);
    
    // 下一帧会重绘这个区域
}
```

---

## 六、使用示例

### 示例1：按钮点击

```c
void handle_button_click(gui_button_t *btn)
{
    btn->state = PRESSED;
    
    // 标记按钮区域
    gui_rect_t rect = {
        btn->base.x, 
        btn->base.y,
        btn->base.x + btn->base.w - 1,
        btn->base.y + btn->base.h - 1
    };
    gui_fb_change_region(&rect);
}
```

### 示例2：对象移动（需要标记两次）

```c
void move_object(gui_obj_t *obj, int16_t new_x, int16_t new_y)
{
    // 1. 标记旧位置
    gui_rect_t old_rect = {
        obj->x, 
        obj->y,
        obj->x + obj->w - 1,
        obj->y + obj->h - 1
    };
    gui_fb_change_region(&old_rect);
    
    // 2. 修改坐标
    obj->x = new_x;
    obj->y = new_y;
    
    // 3. 标记新位置
    gui_rect_t new_rect = {
        obj->x,
        obj->y,
        obj->x + obj->w - 1,
        obj->y + obj->h - 1
    };
    gui_fb_change_region(&new_rect);
}
```

**关键**：移动对象需要标记**两个区域**（旧位置 + 新位置）

### 示例3：文本更新

```c
void update_time_display(gui_text_t *label)
{
    // 更新文本内容
    sprintf(label->content, "%02d:%02d", hour, min);
    
    // 标记文本区域
    gui_rect_t text_rect = {
        label->base.x,
        label->base.y,
        label->base.x + label->base.w - 1,
        label->base.y + label->base.h - 1
    };
    gui_fb_change_region(&text_rect);
}
```

### 示例4：时钟秒针（精确标记）

```c
void update_clock_second(clock_widget_t *clock)
{
    // 只标记秒针扫过的区域，而非整个时钟
    gui_rect_t needle_area = {
        clock->center_x - clock->needle_length,
        clock->center_y - clock->needle_length,
        clock->center_x + clock->needle_length,
        clock->center_y + clock->needle_length
    };
    gui_fb_change_region(&needle_area);
}
```

---

## 七、渲染流程

```c
/* realgui/dc/gui_fb.c */

void gui_fb_draw(gui_obj_t *root)
{
    gui_dispdev_t *dc = gui_get_dc();
    
    if (dirty_mgr.full_refresh || dirty_mgr.count == 0) {
        // 全屏刷新（降级或无脏块）
        gui_fb_clear(dc->frame_buf, fb_bg_color, dc->fb_width * dc->fb_height);
        dc->section = (gui_rect_t){0, 0, dc->fb_width - 1, dc->fb_height - 1};
        obj_draw_scan(root);
        dc->lcd_update(dc);
    } else {
        // 逐个绘制脏块
        for (uint32_t i = 0; i < dirty_mgr.count; i++) {
            gui_rect_t *dirty = &dirty_mgr.regions[i];
            
            // 设置当前渲染区域
            dc->section = *dirty;
            
            // 清空脏块区域
            uint32_t width = dirty->x2 - dirty->x1 + 1;
            uint32_t height = dirty->y2 - dirty->y1 + 1;
            for (uint32_t y = 0; y < height; y++) {
                uint8_t *line = dc->frame_buf + 
                    ((dirty->y1 + y) * dc->fb_width + dirty->x1) * 
                    (dc->bit_depth / 8);
                gui_fb_clear(line, fb_bg_color, width);
            }
            
            // 绘制与脏块相交的对象
            obj_draw_scan(root);
            
            // 更新这个区域到 LCD
            dc->lcd_update(dc);
        }
    }
    
    // 清空脏块列表（为下一帧准备）
    dirty_mgr.count = 0;
    dirty_mgr.full_refresh = false;
}
```

---

## 八、工作流程

```
1. 用户修改对象
   ↓
2. 用户计算矩形坐标
   ↓
3. 用户调用 gui_fb_change_region(rect)
   ↓
4. 矩形被添加到 dirty_mgr.regions[]
   ↓
5. 下一帧 gui_fb_draw() 读取脏块数组
   ↓
6. 逐个重绘脏块区域
   ↓
7. 清空脏块数组
```

---

## 九、关键要点

### 用户责任

- ✅ 用户需要**主动调用** `gui_fb_change_region()`
- ✅ 用户需要**计算矩形坐标**
- ✅ 对象移动需要标记**两次**（旧位置 + 新位置）

### 系统责任

- ✅ 存储脏块数组
- ✅ 裁剪到屏幕范围
- ✅ 溢出时降级全屏
- ✅ 渲染时处理脏块

---

## 十、优缺点

### 优点

- ✅ 简单直接，API 最少
- ✅ 用户完全控制，可以精确优化
- ✅ 不需要修改现有对象 API

### 缺点

- ⚠️ 用户需要理解脏块概念
- ⚠️ 容易忘记标记（导致不刷新）
- ⚠️ 需要手动计算矩形坐标
- ⚠️ 移动对象需要记得标记两次

---

## 十一、需要修改的文件

| 文件 | 类型 | 说明 |
|------|------|------|
| `realgui/dc/gui_dirty_region.h` | 新增 | 声明 `gui_fb_change_region()` |
| `realgui/dc/gui_dirty_region.c` | 新增 | 实现脏块管理 |
| `realgui/dc/gui_fb.c` | 修改 | 改造 `gui_fb_draw()` |
| `Kconfig.gui` | 修改 | 添加配置选项 |

---

## 十二、完整调用示例

```c
/* 完整示例：移动图片 */

void demo_move_image(void)
{
    gui_img_t *img = get_my_image();
    
    // 当前位置
    int16_t old_x = img->base.x;  // 10
    int16_t old_y = img->base.y;  // 10
    int16_t w = img->base.w;      // 50
    int16_t h = img->base.h;      // 50
    
    // 新位置
    int16_t new_x = 100;
    int16_t new_y = 200;
    
    // 步骤1：标记旧位置
    gui_rect_t old_rect = {old_x, old_y, old_x + w - 1, old_y + h - 1};
    gui_fb_change_region(&old_rect);  // regions[0] = [10,10,59,59]
    
    // 步骤2：修改坐标
    img->base.x = new_x;
    img->base.y = new_y;
    
    // 步骤3：标记新位置
    gui_rect_t new_rect = {new_x, new_y, new_x + w - 1, new_y + h - 1};
    gui_fb_change_region(&new_rect);  // regions[1] = [100,200,149,249]
    
    // 结果：dirty_mgr.count = 2
    // 下一帧会重绘这两个区域
}
```
