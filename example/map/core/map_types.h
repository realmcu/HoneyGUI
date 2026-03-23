/**
 * @file map_types.h
 * @brief Common type definitions for the map navigation system
 */

#ifndef MAP_TYPES_H
#define MAP_TYPES_H

#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include "../platform/map_platform.h"

#ifdef __cplusplus
extern "C" {
#endif

#define MAP_MALLOC(size)           map_platform_get()->mem.malloc(size)
#define MAP_FREE(ptr)              map_platform_get()->mem.free(ptr)
#define MAP_CALLOC(num, size)      map_platform_get()->mem.calloc(num, size)
#define MAP_REALLOC(ptr, size)     map_platform_get()->mem.realloc(ptr, size)
#define MAP_MEMCPY                 memcpy

#define MAP_FOPEN(path, mode)      map_platform_get()->fs.open(path, mode)
#define MAP_FREAD(ptr, sz, n, f)   map_platform_get()->fs.read(ptr, sz, n, f)
#define MAP_FWRITE(ptr, sz, n, f)  map_platform_get()->fs.write(ptr, sz, n, f)
#define MAP_FSEEK(f, off, ori)     map_platform_get()->fs.seek(f, off, ori)
#define MAP_FTELL(f)               map_platform_get()->fs.tell(f)
#define MAP_FCLOSE(f)              map_platform_get()->fs.close(f)

#define MAP_PRINTF                 MAP_LOG_INFO
#define MAP_FPRINTF                MAP_PRINTF

#define MAP_TIME_MS()              map_platform_get()->time.get_ms()
#define MAP_SLEEP_MS(ms)           map_platform_get()->time.sleep_ms(ms)

/* Map file magic number */
#define MAP_MAGIC       "TMAP"
#define MAP_VERSION     7       /* Version 7: area spatial index for fast rendering */

/* Spatial index configuration */
#define SPATIAL_INDEX_CELL_SIZE_DEG  0.005f  /* Cell size in degrees (~500m at equator) */
#define SPATIAL_INDEX_MAX_CELLS      10000   /* Maximum number of cells */

/* Maximum limits */
#define MAX_PATH_POINTS     4096
#define MAX_NODES           100000
#define MAX_EDGES           500000
#define MAX_AREAS           50000
#define MAX_AREA_POINTS     1000000
#define MAX_LABELS          10000
#define MAX_LABEL_TEXT_SIZE 200000

/* Error codes */
typedef enum
{
    MAP_OK = 0,
    MAP_ERR_FILE_NOT_FOUND,
    MAP_ERR_INVALID_FORMAT,
    MAP_ERR_VERSION_MISMATCH,
    MAP_ERR_OUT_OF_MEMORY,
    MAP_ERR_NO_PATH,
    MAP_ERR_INVALID_PARAM
} map_error_t;

/* Road types (0-7) */
typedef enum
{
    ROAD_SERVICE = 0,       /* Service roads, alleys */
    ROAD_RESIDENTIAL = 1,   /* Residential streets */
    ROAD_TERTIARY = 2,      /* Tertiary roads */
    ROAD_SECONDARY = 3,     /* Secondary roads */
    ROAD_PRIMARY = 4,       /* Primary roads */
    ROAD_TRUNK = 5,         /* Trunk roads */
    ROAD_MOTORWAY = 6,      /* Motorways/highways */
    ROAD_OTHER = 7          /* Other types */
} road_type_t;

/* Transport mode for navigation */
typedef enum
{
    TRANSPORT_CAR = 0,      /* Car: prefer highways/motorways, avoid small roads */
    TRANSPORT_BIKE = 1,     /* Bicycle: prefer residential/tertiary, avoid highways */
    TRANSPORT_WALK = 2,     /* Walking: same as bike, prefer smaller roads */
    TRANSPORT_TRANSIT = 3   /* Public transit (subway): future support */
} transport_mode_t;

/* Area types for polygons */
typedef enum
{
    AREA_WATER = 0,         /* Water bodies: lakes, rivers, ponds */
    AREA_PARK = 1,          /* Parks and gardens */
    AREA_FOREST = 2,        /* Forests and woods */
    AREA_GRASS = 3,         /* Grass and meadows */
    AREA_BUILDING = 4,      /* Buildings */
    AREA_INDUSTRIAL = 5,    /* Industrial areas */
    AREA_COMMERCIAL = 6,    /* Commercial areas */
    AREA_RESIDENTIAL = 7,   /* Residential areas */
    AREA_OTHER = 8          /* Other types */
} area_type_t;

/* Label types for text labels */
typedef enum
{
    LABEL_ROAD = 0,         /* Road name */
    LABEL_PLACE = 1,        /* Place name (city, district, etc.) */
    LABEL_POI = 2,          /* Point of interest */
    LABEL_WATER = 3,        /* Water body name */
    LABEL_PARK = 4,         /* Park name */
    LABEL_OTHER = 5         /* Other labels */
} label_type_t;

/* ============================================================================
 * Phase 2: Lane Direction Types
 * ============================================================================
 */

/**
 * @brief Lane direction flags (bitmask)
 *
 * Each lane can have one or more allowed directions.
 * Multiple flags can be combined using bitwise OR.
 */
typedef enum
{
    LANE_DIR_NONE         = 0x00,   /* No direction specified */
    LANE_DIR_STRAIGHT     = 0x01,   /* Straight ahead */
    LANE_DIR_LEFT         = 0x02,   /* Left turn */
    LANE_DIR_RIGHT        = 0x04,   /* Right turn */
    LANE_DIR_SLIGHT_LEFT  = 0x08,   /* Slight left / bear left */
    LANE_DIR_SLIGHT_RIGHT = 0x10,   /* Slight right / bear right */
    LANE_DIR_SHARP_LEFT   = 0x20,   /* Sharp left turn */
    LANE_DIR_SHARP_RIGHT  = 0x40,   /* Sharp right turn */
    LANE_DIR_UTURN        = 0x80,   /* U-turn allowed */
} lane_direction_t;

/* Maximum number of lanes per road segment */
#define MAX_LANES_PER_ROAD  8

/**
 * @brief Single lane information
 */
typedef struct
{
    uint8_t directions;     /* Bitmask of allowed directions (lane_direction_t) */
    uint8_t is_recommended; /* 1 if this is the recommended lane, 0 otherwise */
    uint8_t reserved[2];    /* Reserved for future use */
} lane_info_t;

/**
 * @brief Lane configuration for a road segment
 */
typedef struct
{
    uint8_t lane_count;                     /* Total number of lanes */
    uint8_t recommended_lane;               /* Index of recommended lane (0-based, 255 = none) */
    uint8_t reserved[2];                    /* Reserved for future use */
    lane_info_t lanes[MAX_LANES_PER_ROAD];  /* Lane information array */
} lane_config_t;

/**
 * @brief Extended edge with lane information (24 bytes)
 *
 * Used for roads with lane data. Extends map_edge_t with lane info.
 */
typedef struct
{
    uint32_t from_node;     /* Source node index */
    uint32_t to_node;       /* Target node index */
    float distance;         /* Distance in meters */
    uint8_t road_type;      /* Road type (road_type_t) */
    uint8_t flags;          /* Flags (oneway, etc.) */
    uint8_t lane_count;     /* Number of lanes (0 = no lane info) */
    uint8_t lane_offset;    /* Offset into lane data array (if lane_count > 0) */
    uint32_t reserved;      /* Reserved for future use */
} map_edge_v2_t;

/* Feature flags for map content */
typedef enum
{
    MAP_FEATURE_ROADS     = 0x01,   /* Has road network */
    MAP_FEATURE_WATER     = 0x02,   /* Has water areas */
    MAP_FEATURE_PARKS     = 0x04,   /* Has parks/green areas */
    MAP_FEATURE_BUILDINGS = 0x08,   /* Has buildings */
    MAP_FEATURE_LABELS    = 0x10,   /* Has labels */
    MAP_FEATURE_ALL       = 0x1F    /* All features */
} map_feature_t;

/* Coordinate point */
typedef struct
{
    float lat;      /* Latitude in degrees */
    float lon;      /* Longitude in degrees */
} coord_t;

/* Map file header (76 bytes for v7) */
typedef struct
{
    char magic[4];          /* "TMAP" */
    uint32_t version;       /* Format version */
    uint32_t node_count;    /* Number of nodes */
    uint32_t edge_count;    /* Number of edges */
    float min_lat;          /* Bounding box */
    float min_lon;
    float max_lat;
    float max_lon;
    /* v2 additions */
    uint32_t features;      /* Feature flags (MAP_FEATURE_*) */
    uint32_t area_count;    /* Number of areas */
    uint32_t area_points_count; /* Total points in all areas */
    uint32_t reserved;      /* Reserved for future use */
    /* v3 additions (optional, check version) */
    uint32_t label_count;       /* Number of labels */
    uint32_t label_text_size;   /* Total size of label text data */
    /* v4 additions: pre-built adjacency list */
    uint32_t adj_list_count;    /* Total adjacency list entries */
    /* v5 additions: lane data */
    uint32_t lane_data_size;    /* Size of lane data section in bytes */
    /* v6 additions: spatial index for edges */
    uint16_t grid_cols;         /* Number of grid columns */
    uint16_t grid_rows;         /* Number of grid rows */
    uint32_t spatial_index_count; /* Total number of edge indices in spatial index */
    /* v7 additions: spatial index for areas */
    uint32_t area_spatial_index_count; /* Total number of area indices in area spatial index */
} map_header_t;

/* Node in the map */
typedef struct
{
    uint32_t id;            /* Node index */
    float lat;              /* Latitude */
    float lon;              /* Longitude */
} map_node_t;

/* Edge in the map (16 bytes) */
typedef struct
{
    uint32_t from_node;     /* Source node index */
    uint32_t to_node;       /* Target node index */
    float distance;         /* Distance in meters */
    uint8_t road_type;      /* Road type (road_type_t) */
    uint8_t flags;          /* Flags (oneway, etc.) */
    uint16_t lane_offset;   /* Offset into lane_data, 0xFFFF = no lane info (v5) */
} map_edge_t;

/* Label in the map (20 bytes) */
typedef struct
{
    float lat;              /* Label center latitude */
    float lon;              /* Label center longitude */
    uint32_t text_offset;   /* Offset into label_text array */
    uint16_t text_length;   /* Length of text in bytes */
    uint8_t label_type;     /* Label type (label_type_t) */
    uint8_t priority;       /* Display priority (higher = more important) */
    uint8_t road_type;      /* For road labels: road type */
    uint8_t flags;          /* Flags */
    uint16_t area_hectare;  /* Area in hectares (10000 sqm), max 65535 hectares ≈ 655 km² */
} map_label_t;

/* Area (polygon) in the map (12 bytes) */
typedef struct
{
    uint32_t point_offset;  /* Offset into area_points array */
    uint16_t point_count;   /* Number of points in this area */
    uint8_t area_type;      /* Area type (area_type_t) */
    uint8_t flags;          /* Flags */
    uint32_t reserved;      /* Reserved */
} map_area_t;

/* Adjacency list entry (8 bytes, 4-byte aligned for zero-copy) */
typedef struct
{
    uint32_t to_node;       /* Target node index */
    uint16_t distance_dm;   /* Distance in decimeters (0.1m), max 6553.5m */
    uint8_t road_type;      /* Road type (road_type_t) */
    uint8_t reserved;       /* Padding for 4-byte alignment */
} adj_entry_t;

/* Convert distance in meters to decimeters (for adj_entry_t) */
#define DISTANCE_TO_DM(meters) ((uint16_t)((meters) * 10.0f + 0.5f))
/* Convert decimeters back to meters */
#define DISTANCE_FROM_DM(dm) ((float)(dm) * 0.1f)

/* Map data structure */
typedef struct
{
    map_header_t header;
    map_node_t *nodes;              /* Node array */
    map_edge_t *edges;              /* Edge array */

    /* Adjacency list for fast lookup (v4) */
    uint32_t *adj_offsets;          /* Offset into adj_list for each node (node_count + 1 entries) */
    adj_entry_t *adj_list;          /* Adjacency entries */

    /* Area data (v2) */
    map_area_t *areas;              /* Area array */
    coord_t *area_points;           /* All area polygon points */

    /* Label data (v3) */
    map_label_t *labels;            /* Label array */
    char *label_text;               /* All label text (UTF-8) */

    /* Lane data (v5) */
    uint8_t *lane_data;             /* Lane configuration data */    /* Spatial index (v6) - for fast edge rendering */
    uint32_t *cell_offsets;         /* Offset into cell_edges for each cell (grid_cols*grid_rows+1 entries) */
    uint32_t *cell_edges;           /* Edge indices organized by cell */

    /* Area spatial index (v7) - for fast area rendering */
    uint32_t *area_cell_offsets;    /* Offset into area_cell_indices for each cell (grid_cols*grid_rows+1 entries) */
    uint32_t *area_cell_indices;    /* Area indices organized by cell */

    /* Raw data pointer (zero-copy mode) */
    const uint8_t *raw_data;        /* Original data buffer (NULL if copied) */
    bool owns_raw_data;             /* True if we should free raw_data */

    bool loaded;
} map_t;

/* Path result */
typedef struct
{
    coord_t *points;        /* Array of path points */
    uint32_t count;         /* Number of points */
    float total_distance;   /* Total path distance in meters */
} path_t;

/* Render style */
typedef struct
{
    uint32_t path_color;        /* Path color (RGBA) */
    float path_width;           /* Path line width in pixels */
    uint32_t start_color;       /* Start point color */
    uint32_t end_color;         /* End point color */
    float point_radius;         /* Point radius in pixels */
    uint32_t background_color;  /* Background color */
    float min_segment_length;   /* Minimum segment length (pixels), segments smaller than this are skipped */
} render_style_t;

/* Map color theme (Google Maps style) */
typedef struct
{
    uint32_t background;        /* Background color */
    uint32_t water;             /* Water bodies */
    uint32_t park;              /* Parks */
    uint32_t forest;            /* Forests */
    uint32_t grass;             /* Grass areas */
    uint32_t building;          /* Buildings */
    uint32_t industrial;        /* Industrial areas */
    uint32_t commercial;        /* Commercial areas */
    uint32_t residential_area;  /* Residential areas */
    /* Road colors by type */
    uint32_t road_motorway;     /* Motorway color */
    uint32_t road_trunk;        /* Trunk road color */
    uint32_t road_primary;      /* Primary road color */
    uint32_t road_secondary;    /* Secondary road color */
    uint32_t road_tertiary;     /* Tertiary road color */
    uint32_t road_residential;  /* Residential road color */
    uint32_t road_service;      /* Service road color */
    /* Road widths by type */
    float width_motorway;
    float width_trunk;
    float width_primary;
    float width_secondary;
    float width_tertiary;
    float width_residential;
    float width_service;
} map_theme_t;

/* Default render style */
#define RENDER_STYLE_DEFAULT { \
        .path_color = 0x4285F4FF,       /* Google Blue */ \
                      .path_width = 4.0f, \
                                    .start_color = 0x34A853FF,      /* Green */ \
                                                   .end_color = 0xEA4335FF,        /* Red */ \
                                                                .point_radius = 8.0f, \
                                                                                .background_color = 0xFFFFFFFF, /* White */ \
                                                                                        .min_segment_length = 1.0f      /* Minimum segment length (pixels) */ \
    }


#define MAP_THEME_GOOGLE { \
        .background = 0xF5F5F5FF, \
                      .water = 0x9ECFEFFF, \
                               .park = 0xC8FACDFF, \
                                       .forest = 0xA8DAB5FF, \
                                                 .grass = 0xD4EDCFFF, \
                                                          .building = 0xE8E8E8FF, \
                                                                      .industrial = 0xE0DFDCFF, \
                                                                                    .commercial = 0xF5E6D3FF, \
                                                                                            .residential_area = 0xFAFAFAFF, \
                                                                                                    .road_motorway = 0xFFB347FF, \
                                                                                                            .road_trunk = 0xFFCC80FF, \
                                                                                                                    .road_primary = 0xFFF176FF, \
                                                                                                                            .road_secondary = 0xECECECFF, \
                                                                                                                                    .road_tertiary = 0xE8E8E8FF, \
                                                                                                                                            .road_residential = 0xE5E5E5FF, \
                                                                                                                                                    .road_service = 0xE0E0E0FF, \
                                                                                                                                                            .width_motorway = 4.0f, \
                                                                                                                                                                    .width_trunk = 3.5f, \
                                                                                                                                                                            .width_primary = 3.0f, \
                                                                                                                                                                                    .width_secondary = 1.5f, \
                                                                                                                                                                                            .width_tertiary = 0.7f, \
                                                                                                                                                                                                    .width_residential = 0.6f, \
                                                                                                                                                                                                            .width_service = 0.5f \
    }

/* Latest Google Maps 2025 Style Theme */
#define MAP_THEME_GOOGLE_25 { \
        .background = 0xF1F3F4FF,       /* Lighter global light gray background */ \
                      .water = 0xAECDE3FF,            /* New: slate blue, lower saturation */ \
                               .park = 0xC1E1C5FF,             /* New: mint green */ \
                                       .forest = 0xA8D4AFFF,           /* New: cool deep green */ \
                                                 .grass = 0xD4EDCFFF,            /* Light grass green */ \
                                                          .building = 0xE6E8EAFF,         /* Building color with slight blue-gray tint */ \
                                                                      .industrial = 0xE8EAEDFF,       /* Industrial area */ \
                                                                                    .commercial = 0xF1F3F4FF,       /* Commercial area (now blends more with background) */ \
                                                                                            .residential_area = 0xF8F9FAFF, /* Pure residential area */ \
                                                                                                    .road_motorway = 0x8E9BA7FF,    /* Deep Blue-Gray (Major Highways) */ \
                                                                                                            .road_trunk = 0xBDC1C6FF,       /* Medium Gray (Trunk roads) */ \
                                                                                                                    .road_primary = 0xFFFFFFFF,     /* Pure White (Main City Arterials) */ \
                                                                                                                            .road_secondary = 0xFFFFFFFF,   /* Pure White */ \
                                                                                                                                    .road_tertiary = 0xF8F9FAFF,    /* Subtle Off-White (Smaller links) */ \
                                                                                                                                            .road_residential = 0xF1F3F4FF, /* Light Gray (Neighborhood streets) */ \
                                                                                                                                                    .road_service = 0xE8EAEDFF,     /* Dimmer Gray (Alleys/Service) */ \
                                                                                                                                                            .width_motorway = 4.5f,         /* New: widened highways to emphasize contour */ \
                                                                                                                                                                    .width_trunk = 3.8f, \
                                                                                                                                                                            .width_primary = 3.2f, \
                                                                                                                                                                                    .width_secondary = 1.8f, \
                                                                                                                                                                                            .width_tertiary = 0.9f, \
                                                                                                                                                                                                    .width_residential = 0.7f, \
                                                                                                                                                                                                            .width_service = 0.5f \
    }
/* Dark theme */
#define MAP_THEME_DARK { \
        .background = 0x1A1A2EFF, \
                      .water = 0x16213EFF, \
                               .park = 0x1E3A2FFF, \
                                       .forest = 0x1A3328FF, \
                                                 .grass = 0x1E3A2FFF, \
                                                          .building = 0x2D2D44FF, \
                                                                      .industrial = 0x2D2D44FF, \
                                                                                    .commercial = 0x2D2D44FF, \
                                                                                            .residential_area = 0x252538FF, \
                                                                                                    .road_motorway = 0x4A6FA5FF, \
                                                                                                            .road_trunk = 0x4A6FA5FF, \
                                                                                                                    .road_primary = 0x5C7BA8FF, \
                                                                                                                            .road_secondary = 0x3D3D56FF, \
                                                                                                                                    .road_tertiary = 0x3D3D56FF, \
                                                                                                                                            .road_residential = 0x3D3D56FF, \
                                                                                                                                                    .road_service = 0x2D2D44FF, \
                                                                                                                                                            .width_motorway = 4.0f, \
                                                                                                                                                                    .width_trunk = 3.5f, \
                                                                                                                                                                            .width_primary = 3.0f, \
                                                                                                                                                                                    .width_secondary = 2.5f, \
                                                                                                                                                                                            .width_tertiary = 2.0f, \
                                                                                                                                                                                                    .width_residential = 1.5f, \    .width_service = 1.0f \
    }

/**
 * @brief Get lane configuration from edge
 *
 * Extracts lane information from an edge's lane_offset field.
 * The lane data is stored in the map's lane_data array.
 *
 * @param map The map data
 * @param edge The edge to get lane info from
 * @param[out] config Output lane configuration (must not be NULL)
 * @return true if lane info was found, false if not available
 */
static inline bool map_get_edge_lane_config(const map_t *map, const map_edge_t *edge,
                                            lane_config_t *config)
{
    if (!map || !edge || !config)
    {
        return false;
    }

    /* Check if edge has lane info */
    if (edge->lane_offset == 0xFFFF)
    {
        return false;
    }

    /* Check if map has lane data */
    if (!map->lane_data || map->header.lane_data_size == 0)
    {
        return false;
    }

    /* Validate offset */
    if (edge->lane_offset >= map->header.lane_data_size)
    {
        return false;
    }

    /* Parse lane data: [lane_count, dir0, dir1, ...] */
    const uint8_t *lane_ptr = map->lane_data + edge->lane_offset;
    config->lane_count = lane_ptr[0];

    if (config->lane_count == 0 || config->lane_count > MAX_LANES_PER_ROAD)
    {
        return false;
    }

    config->recommended_lane = 255;  /* No recommendation yet */
    config->reserved[0] = 0;
    config->reserved[1] = 0;

    for (uint8_t i = 0; i < config->lane_count; i++)
    {
        config->lanes[i].directions = lane_ptr[1 + i];
        config->lanes[i].is_recommended = 0;
        config->lanes[i].reserved[0] = 0;
        config->lanes[i].reserved[1] = 0;
    }

    return true;
}
int map_config(void);
int map_init(void);
int map_exit(void);
#ifdef __cplusplus
}
#endif

#endif /* MAP_TYPES_H */
