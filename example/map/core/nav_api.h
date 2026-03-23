/**
 * @file nav_api.h
 * @brief Navigation API for pathfinding
 */

#ifndef NAV_API_H
#define NAV_API_H

#include "map_types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Load map data from file
 *
 * @param filepath Path to the binary map file
 * @return Pointer to map structure, NULL on error
 */
map_t *map_load(const char *filepath);

/**
 * @brief Load map data from memory buffer (zero-copy mode)
 *
 * This function uses zero-copy mode where possible, meaning the map structure
 * will point directly into the provided data buffer. The caller must ensure
 * the data buffer remains valid for the lifetime of the map.
 *
 * @param data Pointer to map data in memory
 * @param size Size of the data in bytes
 * @param owns_data If true, the map will take ownership of data and free it in map_free()
 * @return Pointer to map structure, NULL on error
 */
map_t *map_load_from_memory(const uint8_t *data, uint32_t size, bool owns_data);

/**
 * @brief Free map data
 *
 * @param map Pointer to map structure
 */
void map_free(map_t *map);

/**
 * @brief Get map bounding box
 *
 * @param map Pointer to map structure
 * @param min_lat Output: minimum latitude
 * @param min_lon Output: minimum longitude
 * @param max_lat Output: maximum latitude
 * @param max_lon Output: maximum longitude
 * @return MAP_OK on success
 */
map_error_t map_get_bounds(const map_t *map,
                           float *min_lat, float *min_lon,
                           float *max_lat, float *max_lon);

/**
 * @brief Find the nearest node to a coordinate
 *
 * @param map Pointer to map structure
 * @param lat Latitude
 * @param lon Longitude
 * @return Node index, or UINT32_MAX if not found
 */
uint32_t map_find_nearest_node(const map_t *map, float lat, float lon);

/**
 * @brief Find multiple nearest nodes to a coordinate
 *
 * Returns up to max_count nearest nodes sorted by distance.
 * Useful for fallback routing when the nearest node is not reachable.
 *
 * @param map Pointer to map structure
 * @param lat Latitude
 * @param lon Longitude
 * @param out_nodes Output array for node indices (must be pre-allocated)
 * @param out_distances Output array for distances in meters (can be NULL)
 * @param max_count Maximum number of nodes to return
 * @return Number of nodes found (may be less than max_count)
 */
uint32_t map_find_nearest_nodes(const map_t *map, float lat, float lon,
                                uint32_t *out_nodes, float *out_distances,
                                uint32_t max_count);

/**
 * @brief Find path between two coordinates
 *
 * Uses A* algorithm to find the shortest path.
 * Defaults to car mode for backward compatibility.
 *
 * @param map Pointer to map structure
 * @param start_lat Start latitude
 * @param start_lon Start longitude
 * @param end_lat End latitude
 * @param end_lon End longitude
 * @return Pointer to path structure, NULL if no path found
 */
path_t *nav_find_path(const map_t *map,
                      float start_lat, float start_lon,
                      float end_lat, float end_lon);

/**
 * @brief Find path between two coordinates with transport mode
 *
 * Uses A* algorithm with transport-specific weights:
 * - TRANSPORT_CAR: Prefers highways/motorways, penalizes small roads
 * - TRANSPORT_BIKE: Prefers residential roads, avoids highways
 * - TRANSPORT_WALK: Prefers small roads and alleys
 * - TRANSPORT_TRANSIT: Future subway support
 *
 * @param map Pointer to map structure
 * @param start_lat Start latitude
 * @param start_lon Start longitude
 * @param end_lat End latitude
 * @param end_lon End longitude
 * @param mode Transport mode (car, bike, walk, transit)
 * @return Pointer to path structure, NULL if no path found
 */
path_t *nav_find_path_with_mode(const map_t *map,
                                float start_lat, float start_lon,
                                float end_lat, float end_lon,
                                transport_mode_t mode);

/**
 * @brief Find path with fallback to nearby nodes
 *
 * If the nearest node to the start position cannot reach the destination,
 * this function tries other nearby nodes as starting points.
 *
 * @param map Pointer to map structure
 * @param start_lat Start latitude
 * @param start_lon Start longitude
 * @param end_lat End latitude
 * @param end_lon End longitude
 * @param mode Transport mode (car, bike, walk, transit)
 * @param max_fallback_nodes Maximum number of nearby nodes to try (recommended: 5-10)
 * @param max_search_radius Maximum search radius in meters (0 = no limit)
 * @return Pointer to path structure, NULL if no path found
 */
path_t *nav_find_path_with_fallback(const map_t *map,
                                    float start_lat, float start_lon,
                                    float end_lat, float end_lon,
                                    transport_mode_t mode,
                                    uint32_t max_fallback_nodes,
                                    float max_search_radius);

/**
 * @brief Free path data
 *
 * @param path Pointer to path structure
 */
void path_free(path_t *path);

/**
 * @brief Calculate distance between two coordinates
 *
 * Uses Haversine formula.
 *
 * @param lat1 First latitude
 * @param lon1 First longitude
 * @param lat2 Second latitude
 * @param lon2 Second longitude
 * @return Distance in meters
 */
float nav_calculate_distance(float lat1, float lon1, float lat2, float lon2);

/**
 * @brief Get lane configuration for a path segment
 *
 * Looks up lane information from the map for the edge closest to the
 * given path segment coordinates.
 *
 * @param map Pointer to map structure
 * @param lat1 Start point latitude
 * @param lon1 Start point longitude
 * @param lat2 End point latitude
 * @param lon2 End point longitude
 * @param[out] config Output lane configuration
 * @return true if lane info was found, false otherwise
 */
bool nav_get_lane_config(const map_t *map,
                         float lat1, float lon1,
                         float lat2, float lon2,
                         lane_config_t *config);

/**
 * @brief Get road type for a given segment
 *
 * @param map Map data
 * @param lat1 Start point latitude
 * @param lon1 Start point longitude
 * @param lat2 End point latitude
 * @param lon2 End point longitude
 * @return Road type (0-6), or 0xFF if not found
 */
uint8_t nav_get_road_type(const map_t *map,
                          float lat1, float lon1,
                          float lat2, float lon2);

#ifdef __cplusplus
}
#endif

#endif /* NAV_API_H */
