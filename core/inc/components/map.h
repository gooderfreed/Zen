#ifndef MAP_H
#define MAP_H

#include "components.h"

/*
 * Map dimensions
 * Can be overridden by implementation
 */
#ifndef MAP_HEIGHT
    #define MAP_HEIGHT 1
#endif
#ifndef MAP_WIDTH
    #define MAP_WIDTH 1
#endif
#ifndef MAP_LAYERS
    #define MAP_LAYERS 1
#endif

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
    #ifdef MAP_DYNAMIC
        int height;                                // Layer height
        int width;                                 // Layer width
        MapObject **objects;                       // Objects on the layer
    #else
        MapObject objects[MAP_HEIGHT][MAP_WIDTH];  // Objects on the layer
    #endif
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
    #ifdef MAP_DYNAMIC
        int layers_count;            // Number of map layers
        MapLayer *layers;            // Map layers
    #else
        MapLayer layers[MAP_LAYERS]; // Map layers
    #endif
    Coords global_coords;            // Global map position
} Map;

/*
 * Map functions
 * Map manipulation and object access
 */
void map_move(Map *map, Coords move);
void map_move_layer(Map *map, int layer);
MapLayer *map_get_current_layer(Map *map);
MapObject map_get_current_object(Map *map);
MapObject map_get_object(Map *map, Coords coords);

#endif
