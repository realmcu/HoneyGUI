/**
 * @file nav_api.c
 * @brief Navigation API implementation with A* pathfinding
 */

#include "nav_api.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <float.h>

/* Constants */
#define PI 3.14159265358979323846
#define EARTH_RADIUS 6371000.0  /* meters */

/* Priority queue node for A* */
typedef struct
{
    uint32_t node_idx;
    float f_score;      /* g + h */
} pq_node_t;

/* Priority queue (min-heap) */
typedef struct
{
    pq_node_t *nodes;
    uint32_t size;
    uint32_t capacity;
} priority_queue_t;

/* ============================================================================
 * Utility Functions
 * ============================================================================
 */

static float deg_to_rad(float deg)
{
    return deg * (float)(PI / 180.0);
}

float nav_calculate_distance(float lat1, float lon1, float lat2, float lon2)
{
    float lat1_rad = deg_to_rad(lat1);
    float lat2_rad = deg_to_rad(lat2);
    float delta_lat = deg_to_rad(lat2 - lat1);
    float delta_lon = deg_to_rad(lon2 - lon1);

    float a = sinf(delta_lat / 2) * sinf(delta_lat / 2) +
              cosf(lat1_rad) * cosf(lat2_rad) *
              sinf(delta_lon / 2) * sinf(delta_lon / 2);
    float c = 2 * atan2f(sqrtf(a), sqrtf(1 - a));

    return (float)EARTH_RADIUS * c;
}

/* ============================================================================
 * Transport Mode Weight Calculation
 * ============================================================================
 */

/**
 * @brief Calculate edge weight based on transport mode and road type
 *
 * Weight multipliers adjust the effective distance:
 * - Lower multiplier = preferred road (faster/shorter effective distance)
 * - Higher multiplier = avoided road (slower/longer effective distance)
 * - INFINITY = road not accessible for this transport mode
 */
static float get_transport_weight(float distance, uint8_t road_type, transport_mode_t mode)
{
    float multiplier = 1.0f;

    switch (mode)
    {
    case TRANSPORT_CAR:
        /* Car mode: prefer highways/motorways, penalize small roads */
        switch (road_type)
        {
        case ROAD_MOTORWAY:
            multiplier = 0.4f;
            break;  /* Strongly prefer highways */
        case ROAD_TRUNK:
            multiplier = 0.5f;
            break;  /* Prefer trunk roads */
        case ROAD_PRIMARY:
            multiplier = 0.6f;
            break;  /* Prefer primary roads */
        case ROAD_SECONDARY:
            multiplier = 0.8f;
            break;  /* Slightly prefer */
        case ROAD_TERTIARY:
            multiplier = 1.0f;
            break;  /* Normal */
        case ROAD_RESIDENTIAL:
            multiplier = 2.0f;
            break;  /* Penalize residential */
        case ROAD_SERVICE:
            multiplier = 3.0f;
            break;  /* Strongly penalize service roads */
        case ROAD_OTHER:
            multiplier = 1.5f;
            break;  /* Unknown roads - slight penalty */
        default:
            multiplier = 1.5f;
            break;
        }
        break;
    case TRANSPORT_BIKE:
        /* Bike mode: prefer smaller roads, strongly penalize highways */
        switch (road_type)
        {
        case ROAD_MOTORWAY:
            multiplier = 20.0f;
            break;  /* Very strongly penalize highways */
        case ROAD_TRUNK:
            multiplier = 15.0f;
            break;  /* Very strongly penalize trunk roads */
        case ROAD_PRIMARY:
            multiplier = 3.0f;
            break;  /* Penalize busy roads */
        case ROAD_SECONDARY:
            multiplier = 2.0f;
            break;  /* Penalize */
        case ROAD_TERTIARY:
            multiplier = 1.0f;
            break;  /* Normal */
        case ROAD_RESIDENTIAL:
            multiplier = 0.5f;
            break;  /* Strongly prefer */
        case ROAD_SERVICE:
            multiplier = 0.4f;
            break;  /* Very strongly prefer alleys */
        case ROAD_OTHER:
            multiplier = 0.6f;
            break;  /* Prefer unknown (likely small roads) */
        default:
            multiplier = 0.6f;
            break;
        }
        break;
    case TRANSPORT_WALK:
        /* Walk mode: prefer small roads, strongly penalize highways */
        switch (road_type)
        {
        case ROAD_MOTORWAY:
            multiplier = 50.0f;
            break;  /* Extremely penalize highways */
        case ROAD_TRUNK:
            multiplier = 30.0f;
            break;  /* Very strongly penalize trunk roads */
        case ROAD_PRIMARY:
            multiplier = 5.0f;
            break;  /* Penalize busy roads */
        case ROAD_SECONDARY:
            multiplier = 3.0f;
            break;  /* Penalize */
        case ROAD_TERTIARY:
            multiplier = 1.5f;
            break;  /* Slight penalty */
        case ROAD_RESIDENTIAL:
            multiplier = 0.4f;
            break;  /* Strongly prefer */
        case ROAD_SERVICE:
            multiplier = 0.3f;
            break;  /* Very strongly prefer alleys */
        case ROAD_OTHER:
            multiplier = 0.5f;
            break;  /* Prefer unknown (likely small roads) */
        default:
            multiplier = 0.5f;
            break;
        }
        break;

    case TRANSPORT_TRANSIT:
        /* Transit mode: placeholder for future subway support */
        /* For now, treat as walking to reach transit stations */
        multiplier = 1.0f;
        break;

    default:
        multiplier = 1.0f;
        break;
    }

    return distance * multiplier;
}

static const char *get_transport_mode_name(transport_mode_t mode)
{
    switch (mode)
    {
    case TRANSPORT_CAR:
        return "Car (highway preferred)";
    case TRANSPORT_BIKE:
        return "Bicycle";
    case TRANSPORT_WALK:
        return "Walking";
    case TRANSPORT_TRANSIT:
        return "Transit (future)";
    default:
        return "Unknown";
    }
}

/* ============================================================================
 * Priority Queue Implementation
 * ============================================================================
 */

static priority_queue_t *pq_create(uint32_t capacity)
{
    priority_queue_t *pq = (priority_queue_t *)MAP_MALLOC(sizeof(priority_queue_t));
    if (!pq)
    {
        return NULL;
    }

    pq->nodes = (pq_node_t *)MAP_MALLOC(capacity * sizeof(pq_node_t));
    if (!pq->nodes)
    {
        MAP_FREE(pq);
        return NULL;
    }

    pq->size = 0;
    pq->capacity = capacity;
    return pq;
}

static void pq_destroy(priority_queue_t *pq)
{
    if (pq)
    {
        MAP_FREE(pq->nodes);
        MAP_FREE(pq);
    }
}

static void pq_swap(pq_node_t *a, pq_node_t *b)
{
    pq_node_t temp = *a;
    *a = *b;
    *b = temp;
}

static void pq_push(priority_queue_t *pq, uint32_t node_idx, float f_score)
{
    if (pq->size >= pq->capacity)
    {
        return;
    }

    /* Add at end */
    uint32_t i = pq->size++;
    pq->nodes[i].node_idx = node_idx;
    pq->nodes[i].f_score = f_score;

    /* Bubble up */
    while (i > 0)
    {
        uint32_t parent = (i - 1) / 2;
        if (pq->nodes[i].f_score < pq->nodes[parent].f_score)
        {
            pq_swap(&pq->nodes[i], &pq->nodes[parent]);
            i = parent;
        }
        else
        {
            break;
        }
    }
}

static bool pq_pop(priority_queue_t *pq, uint32_t *node_idx, float *f_score)
{
    if (pq->size == 0)
    {
        return false;
    }

    *node_idx = pq->nodes[0].node_idx;
    *f_score = pq->nodes[0].f_score;

    /* Move last to root */
    pq->nodes[0] = pq->nodes[--pq->size];

    /* Bubble down */
    uint32_t i = 0;
    while (true)
    {
        uint32_t left = 2 * i + 1;
        uint32_t right = 2 * i + 2;
        uint32_t smallest = i;

        if (left < pq->size && pq->nodes[left].f_score < pq->nodes[smallest].f_score)
        {
            smallest = left;
        }
        if (right < pq->size && pq->nodes[right].f_score < pq->nodes[smallest].f_score)
        {
            smallest = right;
        }

        if (smallest != i)
        {
            pq_swap(&pq->nodes[i], &pq->nodes[smallest]);
            i = smallest;
        }
        else
        {
            break;
        }
    }

    return true;
}

static bool pq_empty(priority_queue_t *pq)
{
    return pq->size == 0;
}

/* ============================================================================
 * Map Loading
 * ============================================================================
 */

static bool build_adjacency_list(map_t *map)
{
    uint32_t node_count = map->header.node_count;
    uint32_t edge_count = map->header.edge_count;

    /* Temporary array to count edges per node */
    uint32_t *edge_counts = (uint32_t *)MAP_CALLOC(node_count, sizeof(uint32_t));
    if (!edge_counts)
    {
        return false;
    }

    /* Count edges per node */
    for (uint32_t i = 0; i < edge_count; i++)
    {
        uint32_t from = map->edges[i].from_node;
        if (from < node_count)
        {
            edge_counts[from]++;
        }
    }

    /* Allocate adjacency structures */
    map->adj_offsets = (uint32_t *)MAP_CALLOC(node_count + 1, sizeof(uint32_t));
    map->adj_list = (adj_entry_t *)MAP_MALLOC(edge_count * sizeof(adj_entry_t));

    if (!map->adj_offsets || !map->adj_list)
    {
        MAP_FREE(edge_counts);
        return false;
    }

    /* Calculate offsets */
    uint32_t offset = 0;
    for (uint32_t i = 0; i < node_count; i++)
    {
        map->adj_offsets[i] = offset;
        offset += edge_counts[i];
    }
    map->adj_offsets[node_count] = offset;

    /* Reset counts for filling */
    memset(edge_counts, 0, node_count * sizeof(uint32_t));
    /* Fill adjacency list */
    for (uint32_t i = 0; i < edge_count; i++)
    {
        uint32_t from = map->edges[i].from_node;
        if (from < node_count)
        {
            uint32_t idx = map->adj_offsets[from] + edge_counts[from]++;
            map->adj_list[idx].to_node = map->edges[i].to_node;
            map->adj_list[idx].distance_dm = DISTANCE_TO_DM(map->edges[i].distance);
            map->adj_list[idx].road_type = map->edges[i].road_type;
        }
    }

    MAP_FREE(edge_counts);
    return true;
}
map_t *map_load(const char *filepath)
{
    if (!filepath)
    {
        MAP_FPRINTF("Error: NULL filepath\n");
        return NULL;
    }

    MAP_FPRINTF("Loading map from file: %s\n", filepath);

    map_file_t fp = MAP_FOPEN(filepath, "rb");
    if (!fp)
    {
        MAP_FPRINTF("Error: Cannot open file %s\n", filepath);
        return NULL;
    }

    MAP_FSEEK(fp, 0, SEEK_END);
    long file_size = MAP_FTELL(fp);
    MAP_FSEEK(fp, 0, SEEK_SET);

    uint8_t *data = (uint8_t *)MAP_MALLOC(file_size);
    if (!data)
    {
        MAP_FCLOSE(fp);
        return NULL;
    }

    if (MAP_FREAD(data, 1, file_size, fp) != (size_t)file_size)
    {
        MAP_FREE(data);
        MAP_FCLOSE(fp);
        return NULL;
    }
    MAP_FCLOSE(fp);

    map_t *map = map_load_from_memory(data, file_size, true);
    return map;
}

map_t *map_load_from_memory(const uint8_t *data, uint32_t size, bool owns_data)
{
    if (size < 32)    /* Minimum header size for v1 */
    {
        MAP_FPRINTF("Error: File too small\n");
        return NULL;
    }

    /* Allocate map structure */
    map_t *map = (map_t *)MAP_CALLOC(1, sizeof(map_t));
    if (!map)
    {
        return NULL;
    }

    /* Store raw data pointer for zero-copy access */
    map->raw_data = data;
    map->owns_raw_data = owns_data;

    /* Read basic header first (32 bytes for v1, 48 for v2, 64 for v3, 80 for v4) */
    memcpy(&map->header, data, 32);

    /* Verify magic */
    if (memcmp(map->header.magic, MAP_MAGIC, 4) != 0)
    {
        MAP_FPRINTF("Error: Invalid map file format\n");
        MAP_FREE(map);
        return NULL;
    }    /* Handle version compatibility */
    uint32_t version = map->header.version;
    if (version < 1 || version > 7)
    {
        MAP_FPRINTF("Error: Unsupported version %u\n", version);
        MAP_FREE(map);
        return NULL;
    }

    uint32_t header_size;
    if (version == 1)
    {
        header_size = 32;
    }
    else if (version == 2)
    {
        header_size = 48;
    }
    else if (version == 3)
    {
        header_size = 56;
    }
    else if (version == 4)
    {
        header_size = 60;
    }
    else if (version == 5)
    {
        header_size = 64;
    }
    else if (version == 6)
    {
        header_size = 72;    /* v6: 72 bytes */
    }
    else
    {
        header_size = 76;    /* v7: 76 bytes */
    }    /* Read extended header based on version */
    if (version >= 7 && size >= 76)
    {
        memcpy(&map->header, data, 76);
    }
    else if (version >= 6 && size >= 72)
    {
        memcpy(&map->header, data, 72);
        map->header.area_spatial_index_count = 0;
    }
    else if (version >= 4 && size >= 64)
    {
        memcpy(&map->header, data, 64);
        map->header.grid_cols = 0;
        map->header.grid_rows = 0;
        map->header.spatial_index_count = 0;
        map->header.area_spatial_index_count = 0;
    }
    else if (version == 3 && size >= 56)
    {
        memcpy(&map->header, data, 56);
        map->header.adj_list_count = 0;
        map->header.lane_data_size = 0;
        map->header.area_spatial_index_count = 0;
    }
    else if (version == 2 && size >= 48)
    {
        memcpy(&map->header, data, 48);
        map->header.label_count = 0;
        map->header.label_text_size = 0;
        map->header.adj_list_count = 0;
        map->header.lane_data_size = 0;
        map->header.area_spatial_index_count = 0;
    }
    else if (version == 1)
    {
        map->header.features = MAP_FEATURE_ROADS;
        map->header.area_count = 0;
        map->header.area_points_count = 0;
        map->header.reserved = 0;
        map->header.label_count = 0;
        map->header.label_text_size = 0;
        map->header.adj_list_count = 0;
        map->header.lane_data_size = 0;
        map->header.area_spatial_index_count = 0;
    }

    uint32_t node_count = map->header.node_count;
    uint32_t edge_count = map->header.edge_count;
    uint32_t area_count = map->header.area_count;
    uint32_t area_points_count = map->header.area_points_count;
    uint32_t label_count = map->header.label_count;
    uint32_t label_text_size = map->header.label_text_size;
    uint32_t adj_list_count = map->header.adj_list_count;

    /* Calculate node and edge sizes based on version */
    uint32_t node_size = 12;
    uint32_t edge_size = (version == 1) ? 12 : 16;

    /* Calculate expected size */
    uint32_t expected_size = header_size +
                             node_count * node_size +
                             edge_count * edge_size +
                             area_count * 12 +
                             area_points_count * 8 +
                             label_count * 20 +
                             label_text_size;

    /* v4+ adds adjacency data (8-byte aligned format) */
    if (version >= 4)
    {
        expected_size += (node_count + 1) * 4;  /* adj_offsets */
        expected_size += adj_list_count * 8;     /* adj_list entries (8 bytes each, aligned) */
    }

    /* v5+ adds lane data */
    if (version >= 5)
    {
        expected_size += map->header.lane_data_size;
    }    /* v6+ adds spatial index */
    if (version >= 6)
    {
        uint32_t total_cells = (uint32_t)map->header.grid_cols * (uint32_t)map->header.grid_rows;
        expected_size += (total_cells + 1) * 4;  /* cell_offsets */
        expected_size += map->header.spatial_index_count * 4;  /* cell_edges */
    }

    /* v7+ adds area spatial index */
    if (version >= 7)
    {
        uint32_t total_cells = (uint32_t)map->header.grid_cols * (uint32_t)map->header.grid_rows;
        expected_size += (total_cells + 1) * 4;  /* area_cell_offsets */
        expected_size += map->header.area_spatial_index_count * 4;  /* area_cell_indices */
    }

    if (size < expected_size)
    {
        MAP_FPRINTF("Error: File truncated (expected %u, got %u)\n", expected_size, size);
        MAP_FREE(map);
        return NULL;
    }
    const uint8_t *ptr = data + header_size;

    /* Point nodes directly to raw data (zero-copy) */
    map->nodes = (map_node_t *)ptr;
    ptr += node_count * node_size;

    /* Handle edges (v1 needs conversion, v2+ can use zero-copy) */
    if (version == 1)
    {
        /* v1 edges are 12 bytes, need to convert to 16 bytes - must allocate */
        map->edges = (map_edge_t *)MAP_MALLOC(edge_count * sizeof(map_edge_t));
        if (!map->edges)
        {
            MAP_FREE(map);
            return NULL;
        }
        for (uint32_t i = 0; i < edge_count; i++)
        {
            uint32_t from_node_val, to_node_val;
            float distance_val;
            MAP_MEMCPY(&from_node_val, ptr + i * 12, sizeof(uint32_t));
            MAP_MEMCPY(&to_node_val, ptr + i * 12 + 4, sizeof(uint32_t));
            MAP_MEMCPY(&distance_val, ptr + i * 12 + 8, sizeof(float));

            map->edges[i].from_node = from_node_val;
            map->edges[i].to_node = to_node_val;
            map->edges[i].distance = distance_val;
            map->edges[i].road_type = 1;  /* Default to residential */
            map->edges[i].flags = 0;
            map->edges[i].lane_offset = 0xFFFF;  /* No lane info in v1 */
        }
        ptr += edge_count * 12;
    }
    else
    {
        /* v2+ edges are 16 bytes, use zero-copy */
        map->edges = (map_edge_t *)ptr;
        ptr += edge_count * edge_size;
    }      /* Load areas if present (v2, v3, or v4) - zero-copy */
    if (version >= 2 && area_count > 0)
    {
        map->areas = (map_area_t *)ptr;
        ptr += area_count * 12;

        /* Load area points - zero-copy */
        if (area_points_count > 0)
        {
            map->area_points = (coord_t *)ptr;
            ptr += area_points_count * 8;
        }
    }    /* Load labels if present (v3 or v4) - zero-copy */
    if (version >= 3 && label_count > 0)
    {
        map->labels = (map_label_t *)ptr;
        ptr += label_count * 20;

        /* Load label text - zero-copy */
        if (label_text_size > 0)
        {
            map->label_text = (char *)ptr;
            ptr += label_text_size;
        }
    }

    /* Load or build adjacency list */
    if (version >= 4 && adj_list_count > 0)
    {
        /* v4+: load pre-built adjacency data from file (zero-copy, 12-byte aligned) */
        map->adj_offsets = (uint32_t *)ptr;
        ptr += (node_count + 1) * sizeof(uint32_t);

        /* adj_list is now 12-byte aligned, can use zero-copy */
        map->adj_list = (adj_entry_t *)ptr;
        ptr += adj_list_count * sizeof(adj_entry_t);

        MAP_PRINTF("[v%u] Pre-built adjacency list loaded (full zero-copy)\n", version);
        MAP_PRINTF("[v%u] adj_offsets: %u entries, adj_list: %u entries\n", version, node_count + 1,
                   adj_list_count);
    }
    else
    {
        /* v1/v2/v3: build adjacency list dynamically */
        if (!build_adjacency_list(map))
        {
            if (version == 1)
            {
                MAP_FREE(map->edges);    /* Only MAP_FREE if we allocated it */
            }
            MAP_FREE(map);
            return NULL;
        }
        MAP_PRINTF("[v%u] Adjacency list built dynamically (legacy mode)\n", version);
    }

    /* Load lane data if present (v5) - zero-copy */
    if (version >= 5 && map->header.lane_data_size > 0)
    {
        map->lane_data = (uint8_t *)ptr;
        ptr += map->header.lane_data_size;
        MAP_PRINTF("[v5] Lane data loaded: %u bytes\n", map->header.lane_data_size);
    }
    else
    {
        map->lane_data = NULL;
    }

    /* Load spatial index if present (v6) - zero-copy */
    if (version >= 6 && map->header.grid_cols > 0 && map->header.grid_rows > 0)
    {
        uint32_t total_cells = (uint32_t)map->header.grid_cols * (uint32_t)map->header.grid_rows;

        /* Cell offsets: (total_cells + 1) x 4 bytes */
        map->cell_offsets = (uint32_t *)ptr;
        ptr += (total_cells + 1) * sizeof(uint32_t);

        /* Cell edges: spatial_index_count x 4 bytes */
        map->cell_edges = (uint32_t *)ptr;
        ptr += map->header.spatial_index_count * sizeof(uint32_t);

        MAP_PRINTF("[v6] Spatial index loaded: %ux%u grid, %u entries\n",
                   map->header.grid_cols, map->header.grid_rows, map->header.spatial_index_count);
    }
    else
    {
        map->cell_offsets = NULL;
        map->cell_edges = NULL;
    }

    /* Load area spatial index if present (v7) - zero-copy */
    if (version >= 7 && map->header.grid_cols > 0 && map->header.grid_rows > 0 &&
        map->header.area_spatial_index_count > 0)
    {
        uint32_t total_cells = (uint32_t)map->header.grid_cols * (uint32_t)map->header.grid_rows;

        /* Area cell offsets: (total_cells + 1) x 4 bytes */
        map->area_cell_offsets = (uint32_t *)ptr;
        ptr += (total_cells + 1) * sizeof(uint32_t);

        /* Area cell indices: area_spatial_index_count x 4 bytes */
        map->area_cell_indices = (uint32_t *)ptr;
        ptr += map->header.area_spatial_index_count * sizeof(uint32_t);

        MAP_PRINTF("[v7] Area spatial index loaded: %u entries\n",
                   map->header.area_spatial_index_count);
    }
    else
    {
        map->area_cell_offsets = NULL;
        map->area_cell_indices = NULL;
    }

    map->loaded = true;

    MAP_PRINTF("Map loaded (v%u): %u nodes, %u edges", version, node_count, edge_count);
    if (area_count > 0)
    {
        MAP_PRINTF(", %u areas", area_count);
    }
    if (label_count > 0)
    {
        MAP_PRINTF(", %u labels", label_count);
    }
    MAP_PRINTF("\n");
    MAP_PRINTF("Bounds: lat [%.4f, %.4f], lon [%.4f, %.4f]\n",
               (double)map->header.min_lat, (double)map->header.max_lat,
               (double)map->header.min_lon, (double)map->header.max_lon);

    /* Print road type statistics */
    uint32_t road_counts[8] = {0};
    uint32_t edges_with_lane_info = 0;
    for (uint32_t i = 0; i < edge_count; i++)
    {
        uint8_t rt = map->edges[i].road_type;
        if (rt < 8)
        {
            road_counts[rt]++;
        }
        if (map->edges[i].lane_offset != 0xFFFF)
        {
            edges_with_lane_info++;
        }
    }
    MAP_PRINTF("Road types: service=%u, residential=%u, tertiary=%u, secondary=%u, primary=%u, trunk=%u, motorway=%u, other=%u\n",
               road_counts[0], road_counts[1], road_counts[2], road_counts[3],
               road_counts[4], road_counts[5], road_counts[6], road_counts[7]);
    MAP_PRINTF("Lane info coverage: %u/%u edges (%.2f%%)\n",
               edges_with_lane_info, edge_count,
               (double)(edge_count > 0 ? (100.0f * edges_with_lane_info / edge_count) : 0.0f));

    return map;
}

void map_free(map_t *map)
{
    if (map)
    {
        /* For v4: adj_offsets and adj_list are zero-copy, don't MAP_FREE */
        /* For v1/v2/v3: adj_offsets and adj_list were dynamically built, need to MAP_FREE */
        if (map->header.version != 4)
        {
            MAP_FREE(map->adj_offsets);
            MAP_FREE(map->adj_list);
        }

        /* edges: only MAP_FREE if v1 (converted from 12 to 16 bytes) */
        if (map->header.version == 1)
        {
            MAP_FREE(map->edges);
        }

        /* nodes, areas, area_points, labels, label_text are zero-copy, don't MAP_FREE */

        /* Free raw data if we own it */
        if (map->owns_raw_data && map->raw_data)
        {
            MAP_FREE((void *)map->raw_data);
        }

        MAP_FREE(map);
    }
}

map_error_t map_get_bounds(const map_t *map,
                           float *min_lat, float *min_lon,
                           float *max_lat, float *max_lon)
{
    if (!map || !map->loaded)
    {
        return MAP_ERR_INVALID_PARAM;
    }

    if (min_lat)
    {
        *min_lat = map->header.min_lat;
    }
    if (min_lon)
    {
        *min_lon = map->header.min_lon;
    }
    if (max_lat)
    {
        *max_lat = map->header.max_lat;
    }
    if (max_lon)
    {
        *max_lon = map->header.max_lon;
    }

    return MAP_OK;
}

/* ============================================================================
 * Pathfinding
 * ============================================================================
 */

uint32_t map_find_nearest_node(const map_t *map, float lat, float lon)
{
    if (!map || !map->loaded)
    {
        return UINT32_MAX;
    }

    uint32_t nearest = UINT32_MAX;
    float min_dist = FLT_MAX;

    for (uint32_t i = 0; i < map->header.node_count; i++)
    {
        float dist = nav_calculate_distance(lat, lon,
                                            map->nodes[i].lat,
                                            map->nodes[i].lon);
        if (dist < min_dist)
        {
            min_dist = dist;
            nearest = i;
        }
    }

    return nearest;
}

uint32_t map_find_nearest_nodes(const map_t *map, float lat, float lon,
                                uint32_t *out_nodes, float *out_distances,
                                uint32_t max_count)
{
    if (!map || !map->loaded || !out_nodes || max_count == 0)
    {
        return 0;
    }

    /* Temporary arrays for sorting */
    uint32_t *temp_nodes = (uint32_t *)MAP_MALLOC(max_count * sizeof(uint32_t));
    float *temp_dists = (float *)MAP_MALLOC(max_count * sizeof(float));

    if (!temp_nodes || !temp_dists)
    {
        MAP_FREE(temp_nodes);
        MAP_FREE(temp_dists);
        return 0;
    }

    /* Initialize with invalid values */
    uint32_t found_count = 0;
    for (uint32_t i = 0; i < max_count; i++)
    {
        temp_nodes[i] = UINT32_MAX;
        temp_dists[i] = FLT_MAX;
    }

    /* Find top N nearest nodes using insertion sort approach */
    for (uint32_t i = 0; i < map->header.node_count; i++)
    {
        float dist = nav_calculate_distance(lat, lon,
                                            map->nodes[i].lat,
                                            map->nodes[i].lon);

        /* Check if this node should be in our top N */
        if (dist < temp_dists[max_count - 1])
        {
            /* Find insertion position */
            uint32_t insert_pos = max_count - 1;
            while (insert_pos > 0 && dist < temp_dists[insert_pos - 1])
            {
                insert_pos--;
            }

            /* Shift elements down */
            for (uint32_t j = max_count - 1; j > insert_pos; j--)
            {
                temp_nodes[j] = temp_nodes[j - 1];
                temp_dists[j] = temp_dists[j - 1];
            }

            /* Insert new element */
            temp_nodes[insert_pos] = i;
            temp_dists[insert_pos] = dist;

            if (found_count < max_count)
            {
                found_count++;
            }
        }
    }

    /* Copy results to output arrays */
    for (uint32_t i = 0; i < found_count; i++)
    {
        out_nodes[i] = temp_nodes[i];
        if (out_distances)
        {
            out_distances[i] = temp_dists[i];
        }
    }
    MAP_FREE(temp_nodes);
    MAP_FREE(temp_dists);

    return found_count;
}

/**
 * @brief Find scattered nodes at different distances from a position
 *
 * Unlike map_find_nearest_nodes which returns the N closest nodes (which may all
 * be in the same disconnected region), this function ensures minimum spacing
 * between candidates to find nodes in different areas/components.
 *
 * @param map Pointer to map structure
 * @param lat Latitude
 * @param lon Longitude
 * @param out_nodes Output array for node indices
 * @param out_distances Output array for distances (can be NULL)
 * @param max_count Maximum number of nodes to return
 * @param min_spacing Minimum distance between any two candidate nodes (meters)
 * @param max_radius Maximum search radius (meters), 0 = no limit
 * @return Number of nodes found
 */
static uint32_t map_find_scattered_nodes(const map_t *map, float lat, float lon,
                                         uint32_t *out_nodes, float *out_distances,
                                         uint32_t max_count, float min_spacing,
                                         float max_radius)
{
    if (!map || !map->loaded || !out_nodes || max_count == 0)
    {
        return 0;
    }

    uint32_t found_count = 0;

    /* First, find the nearest node */
    float min_dist = FLT_MAX;
    uint32_t nearest = UINT32_MAX;

    for (uint32_t i = 0; i < map->header.node_count; i++)
    {
        float dist = nav_calculate_distance(lat, lon,
                                            map->nodes[i].lat,
                                            map->nodes[i].lon);
        if (dist < min_dist)
        {
            min_dist = dist;
            nearest = i;
        }
    }

    if (nearest == UINT32_MAX)
    {
        return 0;
    }

    /* Add the nearest node as first candidate */
    out_nodes[0] = nearest;
    if (out_distances)
    {
        out_distances[0] = min_dist;
    }
    found_count = 1;

    if (max_count == 1)
    {
        return 1;
    }

    /* Create temporary array to store all node distances for sorting */
    typedef struct
    {
        uint32_t node_idx;
        float dist_from_pos;
    } node_dist_t;

    /* We'll collect nodes in distance bands and ensure spacing */
    /* Process nodes in order of distance from position */
    for (uint32_t pass = 0; pass < map->header.node_count && found_count < max_count; pass++)
    {
        float best_dist = FLT_MAX;
        uint32_t best_node = UINT32_MAX;

        for (uint32_t i = 0; i < map->header.node_count; i++)
        {
            float dist_from_pos = nav_calculate_distance(lat, lon,
                                                         map->nodes[i].lat,
                                                         map->nodes[i].lon);

            /* Skip if beyond max radius */
            if (max_radius > 0 && dist_from_pos > max_radius)
            {
                continue;
            }

            /* Skip if already selected */
            bool already_selected = false;
            for (uint32_t j = 0; j < found_count; j++)
            {
                if (out_nodes[j] == i)
                {
                    already_selected = true;
                    break;
                }
            }
            if (already_selected)
            {
                continue;
            }

            /* Check minimum spacing from all already selected nodes */
            bool too_close = false;
            for (uint32_t j = 0; j < found_count; j++)
            {
                float spacing = nav_calculate_distance(
                                    map->nodes[i].lat, map->nodes[i].lon,
                                    map->nodes[out_nodes[j]].lat, map->nodes[out_nodes[j]].lon);
                if (spacing < min_spacing)
                {
                    too_close = true;
                    break;
                }
            }
            if (too_close)
            {
                continue;
            }

            /* This node is a valid candidate - check if it's the closest unselected */
            if (dist_from_pos < best_dist)
            {
                best_dist = dist_from_pos;
                best_node = i;
            }
        }

        if (best_node == UINT32_MAX)
        {
            /* No more valid candidates found */
            break;
        }

        /* Add this node */
        out_nodes[found_count] = best_node;
        if (out_distances)
        {
            out_distances[found_count] = best_dist;
        }
        found_count++;
    }

    return found_count;
}

static float heuristic(const map_t *map, uint32_t node_idx, uint32_t goal_idx)
{
    return nav_calculate_distance(
               map->nodes[node_idx].lat, map->nodes[node_idx].lon,
               map->nodes[goal_idx].lat, map->nodes[goal_idx].lon
           );
}

path_t *nav_find_path_with_mode(const map_t *map,
                                float start_lat, float start_lon,
                                float end_lat, float end_lon,
                                transport_mode_t mode)
{
    if (!map || !map->loaded)
    {
        return NULL;
    }

    /* Find nearest nodes */
    uint32_t start_node = map_find_nearest_node(map, start_lat, start_lon);
    uint32_t end_node = map_find_nearest_node(map, end_lat, end_lon);

    if (start_node == UINT32_MAX || end_node == UINT32_MAX)
    {
        MAP_FPRINTF("Error: Cannot find start or end node\n");
        return NULL;
    }

    MAP_PRINTF("Pathfinding: node %u -> node %u\n", start_node, end_node);
    MAP_PRINTF("Transport mode: %s\n", get_transport_mode_name(mode));

    uint32_t node_count = map->header.node_count;

    /* Allocate A* data structures */
    float *g_score = (float *)MAP_MALLOC(node_count * sizeof(float));
    float *g_score_real = (float *)MAP_MALLOC(node_count * sizeof(
                                                  float)); /* Real distance for output */
    uint32_t *came_from = (uint32_t *)MAP_MALLOC(node_count * sizeof(uint32_t));
    bool *closed = (bool *)MAP_CALLOC(node_count, sizeof(bool));

    if (!g_score || !g_score_real || !came_from || !closed)
    {
        MAP_FREE(g_score);
        MAP_FREE(g_score_real);
        MAP_FREE(came_from);
        MAP_FREE(closed);
        return NULL;
    }

    /* Initialize */
    for (uint32_t i = 0; i < node_count; i++)
    {
        g_score[i] = FLT_MAX;
        g_score_real[i] = FLT_MAX;
        came_from[i] = UINT32_MAX;
    }
    g_score[start_node] = 0;
    g_score_real[start_node] = 0;

    /* Priority queue */
    priority_queue_t *open = pq_create(node_count);
    if (!open)
    {
        MAP_FREE(g_score);
        MAP_FREE(g_score_real);
        MAP_FREE(came_from);
        MAP_FREE(closed);
        return NULL;
    }

    pq_push(open, start_node, heuristic(map, start_node, end_node));

    bool found = false;

    /* A* main loop */
    while (!pq_empty(open))
    {
        uint32_t current;
        float f;
        pq_pop(open, &current, &f);

        if (current == end_node)
        {
            found = true;
            break;
        }

        if (closed[current])
        {
            continue;
        }
        closed[current] = true;

        /* Explore neighbors */
        uint32_t adj_start = map->adj_offsets[current];
        uint32_t adj_end = map->adj_offsets[current + 1];

        for (uint32_t i = adj_start; i < adj_end; i++)
        {
            uint32_t neighbor = map->adj_list[i].to_node;

            if (closed[neighbor])
            {
                continue;
            }
            /* Calculate weighted distance based on transport mode */
            float real_distance = DISTANCE_FROM_DM(map->adj_list[i].distance_dm);
            float weighted_distance = get_transport_weight(
                                          real_distance,
                                          map->adj_list[i].road_type,
                                          mode
                                      );

            /* Skip if this road type is not accessible */
            if (weighted_distance >= FLT_MAX * 0.5f)
            {
                continue;
            }

            float tentative_g = g_score[current] + weighted_distance;
            float tentative_g_real = g_score_real[current] + real_distance;

            if (tentative_g < g_score[neighbor])
            {
                came_from[neighbor] = current;
                g_score[neighbor] = tentative_g;
                g_score_real[neighbor] = tentative_g_real;
                float f_score = tentative_g + heuristic(map, neighbor, end_node);
                pq_push(open, neighbor, f_score);
            }
        }
    }

    pq_destroy(open);
    MAP_FREE(closed);

    if (!found)
    {
        MAP_FPRINTF("Error: No path found for transport mode: %s\n",
                    get_transport_mode_name(mode));
        MAP_FREE(g_score);
        MAP_FREE(g_score_real);
        MAP_FREE(came_from);
        return NULL;
    }

    /* Reconstruct path */
    /* First, count path length */
    uint32_t path_len = 0;
    uint32_t current = end_node;
    while (current != UINT32_MAX)
    {
        path_len++;
        current = came_from[current];
    }

    /* Allocate path */
    path_t *path = (path_t *)MAP_MALLOC(sizeof(path_t));
    if (!path)
    {
        MAP_FREE(g_score);
        MAP_FREE(g_score_real);
        MAP_FREE(came_from);
        return NULL;
    }

    path->points = (coord_t *)MAP_MALLOC(path_len * sizeof(coord_t));
    if (!path->points)
    {
        MAP_FREE(path);
        MAP_FREE(g_score);
        MAP_FREE(g_score_real);
        MAP_FREE(came_from);
        return NULL;
    }

    path->count = path_len;
    path->total_distance = g_score_real[end_node];  /* Use real distance */

    /* Fill path in reverse */
    current = end_node;
    for (int i = path_len - 1; i >= 0; i--)
    {
        path->points[i].lat = map->nodes[current].lat;
        path->points[i].lon = map->nodes[current].lon;
        current = came_from[current];
    }

    MAP_FREE(g_score);
    MAP_FREE(g_score_real);
    MAP_FREE(came_from);

    MAP_PRINTF("Path found: %u points, %.2f meters\n", path->count, (double)path->total_distance);

    return path;
}

/* Original function - defaults to car mode for backward compatibility */
path_t *nav_find_path(const map_t *map,
                      float start_lat, float start_lon,
                      float end_lat, float end_lon)
{
    return nav_find_path_with_mode(map, start_lat, start_lon, end_lat, end_lon, TRANSPORT_CAR);
}

/* Maximum fallback nodes to try */
#define MAX_FALLBACK_CANDIDATES 20
/* Minimum spacing between fallback candidates (meters) */
#define MIN_FALLBACK_SPACING 150.0

path_t *nav_find_path_with_fallback(const map_t *map,
                                    float start_lat, float start_lon,
                                    float end_lat, float end_lon,
                                    transport_mode_t mode,
                                    uint32_t max_fallback_nodes,
                                    float max_search_radius)
{
    if (!map || !map->loaded)
    {
        return NULL;
    }

    /* Clamp max_fallback_nodes */
    if (max_fallback_nodes == 0)
    {
        max_fallback_nodes = 1;
    }
    if (max_fallback_nodes > MAX_FALLBACK_CANDIDATES)
    {
        max_fallback_nodes = MAX_FALLBACK_CANDIDATES;
    }

    /* Find scattered nodes with minimum spacing to escape disconnected regions */
    uint32_t candidate_nodes[MAX_FALLBACK_CANDIDATES];
    float candidate_distances[MAX_FALLBACK_CANDIDATES];

    /* Use scattered node search with minimum spacing between candidates */
    uint32_t found_count = map_find_scattered_nodes(map, start_lat, start_lon,
                                                    candidate_nodes, candidate_distances,
                                                    max_fallback_nodes,
                                                    MIN_FALLBACK_SPACING,
                                                    max_search_radius);

    if (found_count == 0)
    {
        MAP_FPRINTF("Error: No nearby nodes found\n");
        return NULL;
    }

    MAP_PRINTF("Fallback routing: found %u scattered candidate nodes (min spacing: %.0fm)\n",
               found_count, MIN_FALLBACK_SPACING);

    /* Try each candidate node */
    for (uint32_t i = 0; i < found_count; i++)
    {
        /* Get the coordinates of this candidate node */
        float node_lat = map->nodes[candidate_nodes[i]].lat;
        float node_lon = map->nodes[candidate_nodes[i]].lon;

        MAP_PRINTF("  Trying candidate %u (node %u): %.1f m from position\n",
                   i + 1, candidate_nodes[i], (double)candidate_distances[i]);

        /* Try to find path from this node */
        path_t *path = nav_find_path_with_mode(map, node_lat, node_lon,
                                               end_lat, end_lon, mode);

        if (path != NULL)
        {
            MAP_PRINTF("✓ Path found using fallback node %u (%.1f m from original position)\n",
                       candidate_nodes[i], (double)candidate_distances[i]);

            /* If we used a fallback node (not the nearest), prepend the user's actual position */
            if (i > 0)
            {
                /* Create new path with user's position as first point */
                path_t *extended_path = (path_t *)MAP_MALLOC(sizeof(path_t));
                if (extended_path)
                {
                    extended_path->count = path->count + 1;
                    extended_path->total_distance = path->total_distance + candidate_distances[i];
                    extended_path->points = (coord_t *)MAP_MALLOC(extended_path->count * sizeof(coord_t));

                    if (extended_path->points)
                    {
                        /* First point is user's actual position */
                        extended_path->points[0].lat = start_lat;
                        extended_path->points[0].lon = start_lon;

                        /* Copy rest of path */
                        memcpy(&extended_path->points[1], path->points,
                               path->count * sizeof(coord_t));

                        path_free(path);
                        return extended_path;
                    }
                    else
                    {
                        MAP_FREE(extended_path);
                    }
                }
            }

            return path;
        }

        MAP_PRINTF("  Candidate %u (node %u): no path found\n", i + 1, candidate_nodes[i]);
    }

    MAP_FPRINTF("Error: No path found after trying %u fallback nodes\n", found_count);
    return NULL;
}

void path_free(path_t *path)
{
    if (path)
    {
        MAP_FREE(path->points);
        MAP_FREE(path);
    }
}

bool nav_get_lane_config(const map_t *map,
                         float lat1, float lon1,
                         float lat2, float lon2,
                         lane_config_t *config)
{
    if (!map || !config || !map->loaded)
    {
        return false;
    }

    /* Check if map has lane data */
    if (!map->lane_data || map->header.lane_data_size == 0)
    {
        return false;
    }

    /* Find the closest edge to the given segment */
    /* We look for edges where the from/to nodes are close to our segment endpoints */
    float best_dist = 1e9f;
    const map_edge_t *best_edge = NULL;

    /* Threshold for matching (about 50 meters in coordinate units) */
    const float MATCH_THRESHOLD = 0.0005f;

    for (uint32_t i = 0; i < map->header.edge_count; i++)
    {
        const map_edge_t *edge = &map->edges[i];

        /* Skip edges without lane info */
        if (edge->lane_offset == 0xFFFF)
        {
            continue;
        }

        /* Get edge endpoint coordinates */
        if (edge->from_node >= map->header.node_count ||
            edge->to_node >= map->header.node_count)
        {
            continue;
        }

        float from_lat = map->nodes[edge->from_node].lat;
        float from_lon = map->nodes[edge->from_node].lon;
        float to_lat = map->nodes[edge->to_node].lat;
        float to_lon = map->nodes[edge->to_node].lon;

        /* Calculate distance from segment endpoints to edge endpoints */
        float d1 = (lat1 - from_lat) * (lat1 - from_lat) +
                   (lon1 - from_lon) * (lon1 - from_lon);
        float d2 = (lat2 - to_lat) * (lat2 - to_lat) +
                   (lon2 - to_lon) * (lon2 - to_lon);
        float total_dist = d1 + d2;

        /* Also check reverse direction */
        float d1_rev = (lat1 - to_lat) * (lat1 - to_lat) +
                       (lon1 - to_lon) * (lon1 - to_lon);
        float d2_rev = (lat2 - from_lat) * (lat2 - from_lat) +
                       (lon2 - from_lon) * (lon2 - from_lon);
        float total_dist_rev = d1_rev + d2_rev;

        float min_dist = (total_dist < total_dist_rev) ? total_dist : total_dist_rev;

        if (min_dist < best_dist && min_dist < MATCH_THRESHOLD * MATCH_THRESHOLD)
        {
            best_dist = min_dist;
            best_edge = edge;
        }
    }
    if (best_edge != NULL)
    {
        return map_get_edge_lane_config(map, best_edge, config);
    }

    return false;
}

uint8_t nav_get_road_type(const map_t *map,
                          float lat1, float lon1,
                          float lat2, float lon2)
{
    if (!map || !map->loaded)
    {
        return 0xFF;
    }

    /* Find the closest edge to the given segment */
    float best_dist = 1e9f;
    const map_edge_t *best_edge = NULL;

    /* Threshold for matching (about 50 meters in coordinate units) */
    const float MATCH_THRESHOLD = 0.0005f;

    for (uint32_t i = 0; i < map->header.edge_count; i++)
    {
        const map_edge_t *edge = &map->edges[i];

        /* Get edge endpoint coordinates */
        if (edge->from_node >= map->header.node_count ||
            edge->to_node >= map->header.node_count)
        {
            continue;
        }

        float from_lat = map->nodes[edge->from_node].lat;
        float from_lon = map->nodes[edge->from_node].lon;
        float to_lat = map->nodes[edge->to_node].lat;
        float to_lon = map->nodes[edge->to_node].lon;

        /* Calculate distance from segment endpoints to edge endpoints */
        float d1 = (lat1 - from_lat) * (lat1 - from_lat) +
                   (lon1 - from_lon) * (lon1 - from_lon);
        float d2 = (lat2 - to_lat) * (lat2 - to_lat) +
                   (lon2 - to_lon) * (lon2 - to_lon);
        float total_dist = d1 + d2;

        /* Also check reverse direction */
        float d1_rev = (lat1 - to_lat) * (lat1 - to_lat) +
                       (lon1 - to_lon) * (lon1 - to_lon);
        float d2_rev = (lat2 - from_lat) * (lat2 - from_lat) +
                       (lon2 - from_lon) * (lon2 - from_lon);
        float total_dist_rev = d1_rev + d2_rev;

        float min_dist = (total_dist < total_dist_rev) ? total_dist : total_dist_rev;

        if (min_dist < best_dist && min_dist < MATCH_THRESHOLD * MATCH_THRESHOLD)
        {
            best_dist = min_dist;
            best_edge = edge;
        }
    }

    if (best_edge != NULL)
    {
        return best_edge->road_type;
    }

    return 0xFF;  /* Not found */
}
