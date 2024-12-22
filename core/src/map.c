/*
 * Map implementation
 * Handles map object management and cursor movement
 */
#include "../inc/core.h"

/*
 * Move cursor on map to new position
 * Validates move and checks if target position is interactable
 * Does nothing if move is invalid or target is not interactable
 */
void map_move(Map *map, Coords move) {
    if (move.x == 0 && move.y == 0) return;

    Coords new_coords = {
        .x = map->global_coords.x + move.x,
        .y = map->global_coords.y + move.y,
        .z = map->global_coords.z
    };

    if (new_coords.x < 0 || new_coords.x >= MAP_WIDTH ||
        new_coords.y < 0 || new_coords.y >= MAP_HEIGHT) {
        return;
    }

    MapObject object = map->layers[map->global_coords.z].objects[new_coords.y][new_coords.x];
    if (!(object.object && IS_INTERACTABLE(object.object))) return;

    map->global_coords = new_coords;
}

/*
 * Move cursor to a different layer on the map
 * Validates layer and updates global coordinates
 */
void map_move_layer(Map *map, int layer) {
    if (layer < 0 || layer >= MAP_LAYERS) return;
    map->global_coords.z = (short)layer;
    map->global_coords.x = map->layers[layer].default_layer_coords.x;
    map->global_coords.y = map->layers[layer].default_layer_coords.y;
}

/*
 * Get current layer
 * Returns pointer to the current layer
 */
inline MapLayer *map_get_current_layer(Map *map) {
    return &map->layers[map->global_coords.z];
}

/*
 * Get object at current cursor position
 * Returns MapObject structure containing object data
 */
inline MapObject map_get_current_object(Map *map) {
    return map_get_current_layer(map)->objects[map->global_coords.y][map->global_coords.x];
} 

/*
 * Get object at position
 * Returns MapObject structure containing object data
 */
inline MapObject map_get_object(Map *map, Coords coords) {
    return map->layers[coords.z].objects[coords.y][coords.x];
}

