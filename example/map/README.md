# HoneyGUI Offline Map Navigation System

A lightweight navigation map system designed for MCU platforms with limited resources, featuring GPS integration, real-time pathfinding, and interactive map rendering.

## Features

- **Map Data Processing**: Download OpenStreetMap data and convert to MCU-optimized binary format
- **A* Pathfinding**: Route planning algorithm optimized for embedded systems
- **TRVG Graphics Engine**: Ultra-fast vector graphics rendering without anti-aliasing
- **GPS Integration**: Support both simulated GPS and serial hardware GPS modules
- **Lane Guidance**: Junction navigation with multi-lane display and turn-by-turn directions
- **Spatial Indexing**: Grid-based optimization reducing road checks by 80%
- **Dual Buffer GPS Driver**: Robust NMEA parsing with 2048-byte ping-pong buffer
- **Windows Simulation**: Full PC development and testing environment

## Hardware Requirements

- **RAM**: 4MB
- **CPU**: 200MHz
- **Flash**: 128MB
- **Optional**: UART GPS module (9600 or 115200 baud)

## Directory Structure

```
example/map/
├── honeygui/           # HoneyGUI widget integration layer
├── core/               # Navigation engine (A*, spatial index, lane guidance)
│   ├── include/        # Public API headers
│   └── src/            # Core implementation
├── render/             # Map rendering (themes, styled elements)
├── graphics/trvg/      # Independent vector graphics library
│   ├── trvg.h/c       # Core ultra-fast drawing API
│   └── trvg_render.h/c # Map rendering adapter
├── location/           # Location provider abstraction
│   ├── gps_provider.*  # Unified GPS interface
│   ├── gps_simulator.* # Testing GPS simulator
│   └── gps_driver.*    # Real hardware GPS driver
├── hal/gps/            # GPS hardware drivers (MCU-specific)
├── simulator/          # Windows testing utilities
├── utils/              # Memory monitor, diagnostics
├── tools/              # Python data processing tools
│   ├── download_map.py # OSM data downloader
│   ├── convert_map.py  # Binary format converter
│   └── map_gui.py      # Graphical download/convert tool
└── data/               # Map data storage
```

## Quick Start

### 1. Setup Python Environment

```bash
cd example/map/tools
pip install -r requirements.txt
```

### 2. Download Map Data

#### Command Line
```bash
# Roads only (smallest)
python download_map.py --bbox "120.5,31.2,120.7,31.4" --output ../data/suzhou.osm

# Roads + water + parks
python download_map.py --bbox "120.5,31.2,120.7,31.4" --features roads,water,parks -o ../data/suzhou.osm

# All features (includes buildings)
python download_map.py --bbox "120.5,31.2,120.7,31.4" --features all -o ../data/suzhou_full.osm
```

#### Graphical Tool
```bash
python map_gui.py
```

Features:
- Interactive map with zoom/pan
- City quick jump (Suzhou, Shanghai, Beijing, Tokyo, New York, etc.)
- Drag to select download area (blue rectangle)
- Configure features (roads, water, parks, buildings)
- Visual conversion settings

### 3. Convert to MCU Format

```bash
# Standard conversion
python convert_map.py --input ../data/suzhou.osm --output ../data/suzhou.bin

# With spatial index optimization (recommended)
python convert_map.py -i ../data/suzhou.osm -o ../data/suzhou.bin --features all
```

Output shows:
```
Spatial index: 133 x 74 = 9842 cells
Spatial index entries: 184550 (avg 18.8 per cell)
Area spatial index entries: 65000 (avg 6.6 per cell)
```

### 4. Build Windows Simulator

```bash
cd simulator
mkdir build && cd build
cmake -G "MinGW Makefiles" ..
cmake --build .
```

### 5. Run Navigation

```bash
# GPS simulator mode (default)
./trmap_sim --start 31.30,120.58 --end 31.35,120.65 --mode car

# Serial GPS hardware mode
./trmap_sim --gps-serial COM3 --end 31.35,120.65 --gps-baud 9600

# GPS map display mode (no navigation)
./trmap_sim --gps-map --gps-serial COM3 --view-radius 1000
```

## API Reference

### Navigation API

```c
#include "core/include/nav_api.h"

// Load map
map_t* map = map_load("map.bin");

// Find path
path_t* path = nav_find_path(map, start_lat, start_lon, end_lat, end_lon);

// Iterate path points
for (int i = 0; i < path->count; i++) {
    MAP_PRINTF("Point %d: %.6f, %.6f\n", i, path->points[i].lat, path->points[i].lon);
}

// Get lane configuration for route segment
lane_config_t lane_config;
if (nav_get_lane_config(map, lat1, lon1, lat2, lon2, &lane_config)) {
    MAP_PRINTF("Lanes: %d\n", lane_config.lane_count);
}

// Cleanup
path_free(path);
map_free(map);
```

### Rendering API

```c
#include "render/include/render_api.h"

// Create renderer
renderer_t* renderer = renderer_create(800, 600);

// Set viewport
renderer_fit_path(renderer, path, 0.15f);

// Themed rendering (Google Maps style)
map_theme_t theme = MAP_THEME_GOOGLE;  // or MAP_THEME_DARK
render_map_themed(renderer, map, &theme);

// Draw navigation path
render_style_t style = RENDER_STYLE_DEFAULT;
render_path(renderer, path, &style);

// Save output
render_save_png(renderer, "path.png");

// Cleanup
renderer_destroy(renderer);
```

### Location Provider API

```c
#include "location/gps_provider.h"

// Configure GPS provider
gps_provider_config_t config = {0};
config.type = GPS_PROVIDER_SERIAL;  // or GPS_PROVIDER_SIMULATOR
config.serial_port = "COM3";
config.baudrate = 9600;

// Create provider
gps_provider_t* provider = gps_provider_create(&config);

// Update and get position
while (running) {
    gps_provider_update(provider, delta_time);
    const gps_position_t* pos = gps_provider_get_position(provider);
    
    if (pos->valid) {
        // Use position: pos->lat, pos->lon, pos->heading, pos->speed
    }
}

gps_provider_destroy(provider);
```

### Lane Guidance API

```c
#include "core/include/lane_guidance.h"

// Initialize guidance
guidance_state_t guidance;
guidance_init(&guidance, path);

// Update position
guidance_update(&guidance, current_lat, current_lon);

// Get next junction
junction_guidance_ext_t junction;
if (guidance_get_next_junction_ext(&guidance, &junction, &lane_config) == MAP_SUCCESS) {
    // Render lane guidance panel
    guidance_style_t turn_style;
    lane_display_style_t lane_style;
    
    guidance_style_init_preset(&turn_style, GUIDANCE_STYLE_GOOGLE);
    lane_display_style_init_preset(&lane_style, GUIDANCE_STYLE_GOOGLE);
    
    render_full_lane_guidance(renderer, &junction, &turn_style, &lane_style, x, y, width);
}
```

### TRVG Graphics API

```c
#include "graphics/trvg/trvg.h"

// Initialize context
trvg_context_t ctx;
trvg_init(&ctx, pixels, width, height, 0, TRVG_PIXEL_RGBA8888);

// Clear and draw primitives
trvg_fast_clear(&ctx, 255, 255, 255);
trvg_fast_fill_rect(&ctx, x, y, w, h, r, g, b);
trvg_fast_fill_circle(&ctx, cx, cy, radius, r, g, b);
trvg_fast_thick_line(&ctx, x0, y0, x1, y1, thickness, r, g, b);

// Polygon (point array: [x0, y0, x1, y1, ...])
int16_t points[] = {100, 100, 200, 100, 150, 200};
trvg_fast_fill_polygon(&ctx, points, 3, r, g, b);
trvg_fast_thick_polygon(&ctx, points, 3, true, thickness, r, g, b);
```

## Data Flow Architecture

### GPS Data Flow

```
GPS Hardware (NMEA)
    ↓ UART RX
[GPS Driver: Dual Buffer]
    ↓ Parse & Validate
[GPS Provider: Abstract Interface]
    ↓ gps_position_t
[Navigation Engine]
    ├→ Position Update
    ├→ Off-route Detection
    └→ Reroute Calculation
```

### Rendering Pipeline

```
[Map Binary Data]
    ↓ Load
[Spatial Index Query]
    ↓ Filter visible elements
[TRVG Graphics Engine]
    ↓ Fast drawing API
[Frame Buffer (RGBA/BGR565)]
    ↓ Display
[Screen/File Output]
```

### Navigation Update Cycle

```
1. GPS Update (10-100ms)
   └→ gps_provider_update()

2. Position Check
   └→ guidance_update(lat, lon)
   
3. Off-route Detection
   └→ If off-route: nav_find_path() + guidance_init()
   
4. Junction Guidance
   └→ guidance_get_next_junction_ext()
   
5. Render Frame (5-60 FPS)
   └→ render_map_themed() + render_lane_guidance()
```

## GPS Implementation

### Dual Buffer Architecture

The GPS driver uses a **ping-pong dual buffer** system to eliminate race conditions and data loss.

#### Key Features
- **Buffer Size**: 2 × 1024 bytes (2048 bytes total)
- **No Race Conditions**: ISR writes to active_buffer, main loop reads from process_buffer
- **IDLE Interrupt**: Detects complete NMEA sentences automatically
- **Overflow Protection**: Auto-switch buffers when full

#### Configuration

**Preset Configurations** (add to compiler flags):
```c
-DGPS_CONFIG_MINIMAL          // 512 bytes (memory constrained)
-DGPS_CONFIG_STANDARD         // 1024 bytes (default, balanced)
-DGPS_CONFIG_HIGH_PERFORMANCE // 2048 bytes (high-speed GPS)
```

**Custom Configuration** (`hal/gps/gps_driver_config_mcu.h`):
```c
#define GPS_RX_BUFFER_SIZE      1024    // Single buffer size
#define GPS_UART_IRQ_PRIORITY   3       // 0 = highest priority
```

#### Diagnostics

```c
#ifndef _WIN32
gps_buffer_diagnostics_t diag;
gps_driver_get_buffer_diagnostics(driver, &diag);

if (diag.overflow_count > 0) {
    MAP_PRINTF("WARNING: Buffer overflow! Consider increasing buffer size.\n");
}

MAP_PRINTF("ISR: %u, Overflows: %u, Active: %u bytes\n",
           diag.isr_count, diag.overflow_count, diag.active_bytes);
#endif
```

**Healthy Status**:
- `overflow_count` = 0
- `isr_count` > 0
- Parsing success rate > 95%

#### Latency Tolerance

| Buffer Size | Main Loop Tolerance |
|-------------|---------------------|
| 512 bytes   | ~500ms delay        |
| 1024 bytes  | ~1 second delay     |
| 2048 bytes  | ~2 seconds delay    |

### Supported NMEA Sentences

| Sentence | Content |
|----------|---------|
| GGA | Position, time, altitude, satellite count, HDOP |
| RMC | Position, time, date, speed, course |
| GSA | Fix mode, DOP values, satellites used |
| GSV | Visible satellite count and details |
| VTG | Course, speed |

### MCU Pin Configuration (Realtek Bumblebee RTL876x)

```c
// Default pins (modify in hal/gps/gps_driver.c)
#define GPS_UART_TX_PIN         P3_1
#define GPS_UART_RX_PIN         P3_0
#define GPS_UART_PERIPHERAL     UART0
```

Supported baud rates: **9600** (most common) and **115200** (high-speed).

## Spatial Index Optimization

Grid-based spatial indexing reduces rendering overhead by **80%** for large maps.

### Performance Metrics

**Without Spatial Index**:
- Checks all 164,706 edges every frame
- O(n) complexity

**With Spatial Index (v6+ maps)**:
- Checks only 32,284 edges (19.6%)
- Queries only 63/9,842 cells (0.64%)
- O(k) complexity where k << n

### Configuration

Cell size is configured in `tools/convert_map.py`:
```python
SPATIAL_INDEX_CELL_SIZE_DEG = 0.002  # ~200 meters
```

**Tuning Guidelines**:
- Small viewport (<500m): Use 0.001 deg (~100m) for better performance
- Large viewport (>2km): Use 0.005 deg (~500m) to reduce index size
- Memory constrained: Increase cell size to reduce index overhead

### Storage Overhead

For a typical city map:
```
Grid: 133 × 74 = 9,842 cells
Edge index: 184,550 entries × 4 bytes = ~738 KB
Area index: 65,000 entries × 4 bytes = ~260 KB
Total overhead: ~1 MB (~9.5% of 8 MB map)
```

### Data Structure (v7)

```c
// Map header extensions
typedef struct {
    // ... existing fields ...
    
    // v6: Edge spatial index
    uint16_t grid_cols;               // Grid columns
    uint16_t grid_rows;               // Grid rows
    uint32_t spatial_index_count;     // Edge index entry count
    
    // v7: Area spatial index
    uint32_t area_spatial_index_count; // Area index entry count
} map_header_t;

// Map structure
typedef struct {
    // ... existing fields ...
    
    uint32_t *cell_offsets;       // Per-cell edge offsets
    uint32_t *cell_edges;         // Cell-grouped edge indices
    
    uint32_t *area_cell_offsets;  // Per-cell area offsets
    uint32_t *area_cell_indices;  // Cell-grouped area indices
} map_t;
```

## TRVG Graphics Engine

**TRVG** (Triton Vector Graphics) is a lightweight C graphics library optimized for MCU map rendering.

### Design Goals

- ✅ Ultra-fast drawing (no anti-aliasing)
- ✅ Low memory footprint (~1KB context)
- ✅ Static memory allocation
- ✅ Multiple pixel formats (BGRA/RGBA/BGR565/RGB565)
- ✅ ARM MVE SIMD acceleration support
- ✅ No memory leaks or illegal access
- ✅ Integer coordinates only

### Core Drawing API

All functions use `trvg_fast_*` naming with integer coordinates and RGB parameters:

```c
// Primitives
trvg_fast_clear(&ctx, r, g, b);
trvg_fast_pixel(&ctx, x, y, r, g, b);
trvg_fast_line(&ctx, x0, y0, x1, y1, r, g, b);
trvg_fast_thick_line(&ctx, x0, y0, x1, y1, thickness, r, g, b);

// Shapes
trvg_fast_fill_rect(&ctx, x, y, w, h, r, g, b);
trvg_fast_fill_circle(&ctx, cx, cy, radius, r, g, b);
trvg_fast_fill_triangle(&ctx, x0, y0, x1, y1, x2, y2, r, g, b);

// Polygons
int16_t points[] = {x0, y0, x1, y1, x2, y2};
trvg_fast_fill_polygon(&ctx, points, 3, r, g, b);
trvg_fast_thick_polygon(&ctx, points, 3, true, thickness, r, g, b);
```

### Map Rendering Adapter

`graphics/trvg/trvg_render.h` provides map-specific optimizations:

```c
trvg_map_renderer_t renderer;
trvg_map_renderer_init(&renderer, pixels, width, height, stride, format);
trvg_map_set_viewport(&renderer, min_lat, min_lon, max_lat, max_lon);

// Draw map elements
trvg_map_draw_road(&renderer, points, count, ROAD_PRIMARY, &TRVG_THEME_DAY);
trvg_map_draw_area(&renderer, points, count, AREA_WATER, &TRVG_THEME_DAY);
trvg_map_draw_path(&renderer, points, count, trvg_rgb(0, 128, 255), 5.0f);
trvg_map_draw_marker(&renderer, lat, lon, trvg_rgb(255, 0, 0), 10.0f);
trvg_map_draw_direction_arrow(&renderer, lat, lon, heading, trvg_rgb(0, 0, 255), 12.0f);
```

### Performance Optimization

**Algorithm Choices**:
- Lines: Bresenham algorithm
- Circles: Midpoint circle algorithm
- Polygons: Scanline fill algorithm
- Triangles: Optimized scanline with edge sorting

**ARM MVE (Helium) SIMD Support**:
- Target: Cortex-M55/M85 with MVE
- Compiler: armclang 6.22+
- Flags: `-march=armv8.1-m.main+dsp+mve.fp+fp.dp -mfloat-abi=hard`

Speedup with MVE:
- `trvg_fast_clear`: 4-8×
- `trvg_fast_hline`: 2-4×
- `trvg_fast_fill_rect`: 2-4×
- `trvg_fast_fill_circle`: 2-3×

**Line Segment Simplification**:
```c
renderer.min_road_segment_length = 2.0f;  // Skip segments < 2 pixels
renderer.min_area_segment_length = 1.0f;  // Skip area edges < 1 pixel
```

### Memory Usage

| Item | Size |
|------|------|
| trvg_context_t | ~32 bytes |
| Temporary point buffer | 1 KB |
| **Total** | **~1 KB** |

## Map Data Format

### Binary Structure (v7)

```
Header (80 bytes)
  magic[4]           "TMAP"
  version            7 (latest)
  node_count         uint32
  edge_count         uint32
  min_lat/lon        float × 2
  max_lat/lon        float × 2
  features           uint32 (bitmask)
  area_count         uint32
  area_points_count  uint32
  label_count        uint32
  label_text_size    uint32
  adj_list_count     uint32
  grid_cols          uint16 (v6+)
  grid_rows          uint16 (v6+)
  spatial_index_count uint32 (v6+)
  area_spatial_index_count uint32 (v7+)
  lane_data_size     uint32 (v5+)
  reserved[4]        uint32 × 4

Node (8 bytes each)
  lat                float
  lon                float

Edge (16 bytes each)
  from_node          uint32
  to_node            uint32
  distance           float
  road_type          uint8
  flags              uint8
  lane_offset        uint16 (v5+: 0xFFFF = no lane data)

Areas
  area_type[area_count]      uint8 × N
  area_point_counts[...]     uint32 × N
  area_points[...]           coord_t × M

Spatial Index (v6+)
  cell_offsets[grid_cells+1] uint32 × (cols×rows+1)
  cell_edges[...]            uint32 × spatial_index_count

Area Spatial Index (v7+)
  area_cell_offsets[...]     uint32 × (cols×rows+1)
  area_cell_indices[...]     uint32 × area_spatial_index_count

Lane Data (v5+)
  [lane_count, dir0, dir1, ..., dirN] variable length per edge
```

### Features Bitmask

| Feature | Description | Size Impact |
|---------|-------------|-------------|
| `roads` | Road network (required) | Baseline |
| `water` | Lakes, rivers, ponds | +10-20% |
| `parks` | Parks, forests, grass | +10-30% |
| `buildings` | Building footprints | +50-200% |

### Map Themes

```c
// Predefined themes
extern const map_theme_t MAP_THEME_GOOGLE;  // Google Maps style
extern const map_theme_t MAP_THEME_DARK;    // Dark mode

// Lane guidance themes
extern const guidance_style_t GUIDANCE_STYLE_GOOGLE;
extern const guidance_style_t GUIDANCE_STYLE_DARK;
extern const guidance_style_t GUIDANCE_STYLE_AMAP;
extern const guidance_style_t GUIDANCE_STYLE_BAIDU;
```

## Lane Guidance System

### Junction Guidance

**Maneuver Types**:
- Straight, left, right, slight left/right
- Sharp left/right, U-turn left/right
- Ramp left/right, merge, arrive

**Angle Thresholds**:
- Straight: -20° to 20°
- Slight turn: 20° to 45°
- Normal turn: 45° to 120°
- Sharp turn: 120° to 160°
- U-turn: > 160°

**Distance Formatting**:
- < 100m: 10-meter units ("50 m")
- 100-1000m: 50-meter units ("250 m")
- 1-10km: One decimal place ("1.5 km")
- > 10km: Round to km ("15 km")

### Multi-lane Display

**Lane Direction Flags** (bitmask):
```c
LANE_DIR_STRAIGHT     = 0x01
LANE_DIR_LEFT         = 0x02
LANE_DIR_RIGHT        = 0x04
LANE_DIR_SLIGHT_LEFT  = 0x08
LANE_DIR_SLIGHT_RIGHT = 0x10
LANE_DIR_SHARP_LEFT   = 0x20
LANE_DIR_SHARP_RIGHT  = 0x40
LANE_DIR_UTURN        = 0x80
```

**Recommended Lane Algorithm**:
1. Convert next maneuver to required direction
2. Find all lanes allowing that direction
3. Score lanes (single-direction lanes prioritized)
4. Prefer outer lanes for turns (left lane for left turn)
5. Return highest-scoring lane

### Lane Data in OSM

Parsed from OSM tags:
- `lanes`: Total lane count
- `lanes:forward` / `lanes:backward`: Directional lane count
- `turn:lanes` / `turn:lanes:forward`: Lane-specific turn restrictions

Format: `"left|through|through;right|right"`
- `|` separates lanes
- `;` separates multiple directions per lane

## Performance Tuning

### GPS Buffer Configuration

**Symptoms of undersized buffer**:
- `overflow_count` increasing
- NMEA checksum failures
- Garbled data

**Solutions**:
1. Increase buffer size via preset config
2. Call `gps_driver_update()` more frequently (< 100ms)
3. Reduce other main loop task load

### Spatial Index Cell Size

**Smaller cells** (0.001 deg):
- Pros: Fewer edges per cell, faster for small viewports
- Cons: More cells, larger index overhead

**Larger cells** (0.005 deg):
- Pros: Smaller index, less memory
- Cons: More edges per cell, slower for small viewports

**Optimal for typical use**: 0.002 deg (~200m)

### Rendering Optimization

**Viewport culling**:
- Use spatial index (requires v6+ map format)
- Recompile maps with `convert_map.py` to enable

**Line segment simplification**:
```c
renderer->min_road_segment_length = 2.0f;  // Adjust threshold
renderer->min_area_segment_length = 1.0f;
```

**Frame rate control**:
```bash
./trmap_sim --frames 50  # Limit frame count for testing
```

## Testing and Diagnostics

### GPS Hardware Verification

```bash
cd tools/gps_driver
python build.py
./build/bin/gps_driver.exe --list               # List ports
./build/bin/gps_driver.exe -p COM3 -r           # Show raw NMEA
./build/bin/gps_driver.exe -s                   # Simulation mode
```

### Code Quality Tools

**Static Analysis** (cppcheck):
```bash
.\build_and_test.bat check
# Report: simulator/build/cppcheck/report.txt
```

**Code Formatting** (astyle):
```bash
.\build_and_test.bat format          # Format all files
.\build_and_test.bat format --check  # Check only
```

Configuration files:
- `.astylerc`: Allman style, 4-space indent
- `cppcheck.cfg`: Project-specific rules

## Changelog

### [0.7.4] - 2026-02-10 Road Test Validation

**1-Hour Real Road Test Passed**:
- ✅ Button & touch interaction (zoom, mode switch, pan)
- ✅ Track recording/playback (5 FPS, smooth)
- ✅ Navigation setup (touch destination, 5 FPS)
- ✅ Navigation with re-routing (1 FPS, lane guidance functional)
- ✅ No crashes or display glitches

**Known Issues**:
- ⚠️ Touch lag during off-route recalculation
- ⚠️ Routing detours (map data quality, algorithm limitations)

### [2.0.0] - 2026-01-29 GPS Dual Buffer

**Major Improvements**:
- 🐛 Fixed data packet loss (256B → 2048B dual buffer)
- 🐛 Fixed checksum failures (RX IDLE interrupt)
- 🐛 Fixed race conditions (ping-pong buffer)
- ✨ Diagnostic API (`gps_driver_get_buffer_diagnostics`)
- ✨ Configurable presets (MINIMAL/STANDARD/HIGH_PERFORMANCE)
- ⚡ 8× buffer capacity, reduced CPU overhead

### [1.5.0] - 2026-01-20 Spatial Index v7

- ✅ Area spatial index (water, parks, buildings)
- ✅ 80% reduction in edge checks
- ✅ v7 map format with backward compatibility

### [1.4.0] - 2026-01-15 Lane Guidance

- ✅ Junction guidance with turn arrows
- ✅ Multi-lane display with recommended lane
- ✅ v5 map format with lane data
- ✅ 5 predefined themes (Google, Dark, Amap, Baidu, Default)

### [1.3.0] - 2026-01-10 GPS Integration

- ✅ GPS provider abstraction layer
- ✅ Serial GPS driver (NMEA 0183)
- ✅ GPS simulator with jitter and deviation
- ✅ GPS map display mode

## License

MIT License

Copyright (c) 2026 HoneyGUI Team

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
