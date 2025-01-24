/*
 * Map implementation
 * Handles map object management and cursor movement
 */
#include "../inc/core.h"

/*
 * Initialize map structure
 * Creates a map with specified number of layers and global coordinates
 * Allocates memory for layers and initializes the map structure
 */
Map *init_map(Arena *arena, int layers, Coords global_coords) {
    Map *map = (Map *)arena_alloc(arena, sizeof(Map));

    *map = (Map) {
        .layers_count = layers,
        .global_coords = global_coords,
        .layers = (MapLayer **)arena_alloc(arena, (size_t)(layers) * sizeof(MapLayer *))
    };

    return map;
}

/*
 * Set a layer in the map
 * Assigns a MapLayer to the specified layer index in the map
 */
void map_set_layer(Map *map, MapLayer *map_layer, int layer) {
    map->layers[layer] = map_layer;
}

/*
 * Get a layer from the map
 * Returns the MapLayer at the specified layer index
 */
MapLayer *map_get_layer(Map *map, int layer) {
    return map->layers[layer];
}

/*
 * Get current layer
 * Returns pointer to the current layer
 */
inline MapLayer *map_get_current_layer(Map *map) {
    return map->layers[map->global_coords.z];
}

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

    MapLayer *cur_layer = map_get_current_layer(map);

    if (new_coords.x < 0 || new_coords.x >= cur_layer->width ||
        new_coords.y < 0 || new_coords.y >= cur_layer->height) {
        return;
    }

    MapObject object = map->layers[map->global_coords.z]->objects[new_coords.y][new_coords.x];
    if (!(object.object && IS_INTERACTABLE(object.object))) return;

    map->global_coords = new_coords;
}

/*
 * Move cursor to a different layer on the map
 * Validates layer and updates global coordinates
 */
void map_move_layer(Map *map, int layer) {
    if (layer < 0 || layer >= map->layers_count) return;
    map->global_coords.z = (short)layer;
    map->global_coords.x = map->layers[layer]->default_layer_coords.x;
    map->global_coords.y = map->layers[layer]->default_layer_coords.y;
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
    return map->layers[coords.z]->objects[coords.y][coords.x];
}

