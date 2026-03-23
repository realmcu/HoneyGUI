#ifndef TRMAP_MAP_CONTROL_H
#define TRMAP_MAP_CONTROL_H

#ifdef __cplusplus
extern "C" {
#endif

typedef enum
{
    TRMAP_APP_MODE_MAP_VIEW = 0,
    TRMAP_APP_MODE_GPS_INIT,
    TRMAP_APP_MODE_GPS_TRACK_RECORD,
    TRMAP_APP_MODE_GPS_TRACK_REPLAY,
    TRMAP_APP_MODE_GPS_MAP,
    TRMAP_APP_MODE_NAVIGATION,
    TRMAP_APP_MODE_COUNT
} trmap_app_mode_t;

/*
 * Shared map control API.
 *
 * Usage:
 * - These functions operate on the currently active application mode.
 * - They dispatch internally by current mode state (Map View / GPS Map /
 *   GPS Track / Navigation).
 * - Return value: 1 means the active mode handled the operation, 0 means
 *   the current mode does not support that operation.
 *
 * Typical usage patterns:
 * - Discrete key input:
 *     trmap_map_control_pan_north();
 *     trmap_map_control_zoom_in();
 *     trmap_map_control_reset();
 *
 * - Continuous touch drag:
 *     trmap_map_control_pan_delta(delta_lat, delta_lon);
 *     trmap_map_control_pan_finish();
 *
 * Notes:
 * - pan_north/south/west/east() are intended for fixed-step pan input such
 *   as keys or buttons.
 * - pan_delta() is intended for gesture/touch drag input where the caller has
 *   already converted screen movement into latitude/longitude deltas.
 * - pan_finish() finalizes a drag gesture for modes that use temporary pan
 *   offsets during touch interaction.
 * - reset() restores the default map control state for the active mode.
 * - request_mode_switch() / request_next_mode_switch() only enqueue a mode
 *   switch request. The main loop performs the actual mode transition.
 */

/* Get the currently active application mode. */
trmap_app_mode_t trmap_get_current_mode(void);

/* Request switch to a specific mode. */
int trmap_request_mode_switch(trmap_app_mode_t target_mode);

/* Request switch to the next mode in the cycle. */
int trmap_request_next_mode_switch(void);

/* Pan map by one built-in step toward north. Suitable for key/button input. */
int trmap_map_control_pan_north(void);

/* Pan map by one built-in step toward south. Suitable for key/button input. */
int trmap_map_control_pan_south(void);

/* Pan map by one built-in step toward west. Suitable for key/button input. */
int trmap_map_control_pan_west(void);

/* Pan map by one built-in step toward east. Suitable for key/button input. */
int trmap_map_control_pan_east(void);

/*
 * Apply a continuous pan delta in degrees.
 * delta_lat: positive moves north, negative moves south.
 * delta_lon: positive moves east,  negative moves west.
 */
int trmap_map_control_pan_delta(float delta_lat, float delta_lon);

/* Finalize a touch/drag pan operation for the active mode. */
int trmap_map_control_pan_finish(void);

/* Zoom in for the active mode. */
int trmap_map_control_zoom_in(void);

/* Zoom out for the active mode. */
int trmap_map_control_zoom_out(void);

/* Reset pan/zoom/view state for the active mode. */
int trmap_map_control_reset(void);

#ifdef __cplusplus
}
#endif

#endif /* TRMAP_MAP_CONTROL_H */