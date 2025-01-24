#ifndef MAP_H
#define MAP_H

#include "components.h"

/*
 * MapObject structure
 * Represents a single object on the map with its position
 */
typedef struct MapObject {
    void *object;           // Pointer to the actual object
} MapObject;

/*
 * Map layer structure
 * Represents a single layer on the map
 */
typedef struct MapLayer {
    int height;                                    // Layer height
    int width;                                     // Layer width
    MapObject **objects;                           // Objects on the layer
    Coords default_layer_coords;                   // Default layer coordinates
    void *layer_main_object;                       // Main object for the current layer
    void (*prepare_screen)(Screen *screen);        // Function to prepare screen for the layer
    void (*layer_loop)(Core *core, wint_t key);    // Function to handle layer loop
} MapLayer;


/*
 * Map structure
 * Represents the game map with objects and global position
 */
typedef struct Map {
    int layers_count;        // Number of map layers
    MapLayer **layers;       // Map layers
    Coords global_coords;    // Global map position
} Map;

/*
 * Map functions
 * Map manipulation and object access
 */
Map *init_map(Arena *arena, int layers, Coords global_coords);
void map_set_layer(Map *map, MapLayer *map_layer, int layer);
void map_move(Map *map, Coords move);
void map_move_layer(Map *map, int layer);
MapLayer *map_get_layer(Map *map, int layer);
MapLayer *map_get_current_layer(Map *map);
MapObject map_get_current_object(Map *map);
MapObject map_get_object(Map *map, Coords coords);

#endif
