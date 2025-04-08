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

struct MapObjectList {
    void *object;
    Coords coords;
    bool is_main;
    MapObjectList *next;
};

/*
 * Map layer structure
 * Represents a single layer on the map
 */
typedef struct MapLayer {
    int height;                                         // Layer height
    int width;                                          // Layer width
    MapObject **objects;                                // Objects on the layer
    Coords default_layer_coords;                        // Default layer coordinates
    void *layer_main_object;                            // Main object for the current layer
    void (*prepare_screen)(Screen *screen);             // Function to prepare screen for the layer
    void (*layer_loop)(Zen *zen, wint_t key);           // Function to handle layer loop
    bool (*layer_cursor_loop)(Zen *zen, wint_t key);    // Function to handle cursor movement
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
MapLayer *create_map_layer(Arena *arena, int height, int width, Coords base_coords);
MapLayer *map_get_layer(Map *map, int layer);
MapLayer *map_get_current_layer(Map *map);
MapObject map_get_current_object(Map *map);
MapObject map_get_object(Map *map, Coords coords);


#define OBJECT_FULL(_arena, _objects_list, _object, _coords, _params)                                              \
    do {                                                                                                           \
        MapObjectList *object_list = (MapObjectList *)arena_alloc(_arena, sizeof(MapObjectList));                  \
        object_list->object = _object;                                                                             \
        object_list->next = NULL;                                                                                  \
        object_list->coords = _coords;                                                                             \
        if (object_list->coords.x > cur_layer->width)  cur_layer->width  = object_list->coords.x;                  \
        if (object_list->coords.y > cur_layer->height) cur_layer->height = object_list->coords.y;                  \
        bool is_main = false;                                                                                      \
        _params;                                                                                                   \
        object_list->is_main = is_main;                                                                            \
        object_list->next = _objects_list;                                                                         \
        _objects_list = object_list;                                                                               \
    } while (0)

#define OBJECT_2(object, _coords) \
    OBJECT_FULL(cur_arena, objects_list, object, _coords, {})

#define OBJECT_3(object, _coords, _params) \
    OBJECT_FULL(cur_arena, objects_list, object, _coords, _params)

#define OBJECT(...) GET_MACRO_3(__VA_ARGS__, OBJECT_3, OBJECT_2,)(__VA_ARGS__)




#define MAP_LAYER_FULL(_arena, _name, _params, _objects)                                                                     \
    do {                                                                                                                     \
        _name = (MapLayer *)arena_alloc(_arena, sizeof(MapLayer));                                                           \
        _name->height = 0;                                                                                                   \
        _name->width = 0;                                                                                                    \
        void (*prepare_screen)(Screen *screen) = NULL;                                                                       \
        void (*loop)(Zen *zen, wint_t key) = NULL;                                                                         \
        bool (*cursor_loop)(Zen *zen, wint_t key) = NULL;                                                                  \
        void *main_object = NULL;                                                                                            \
        _params;                                                                                                             \
        _name->prepare_screen = prepare_screen;                                                                              \
        _name->layer_loop = loop;                                                                                            \
        _name->layer_cursor_loop = cursor_loop;                                                                              \
        _name->layer_main_object = main_object;                                                                              \
        MapObjectList *objects_list = NULL;                                                                                  \
        Arena *cur_arena = _arena;                                                                                           \
        MapLayer *cur_layer = _name;                                                                                         \
        _objects;                                                                                                            \
        if (!objects_list) {                                                                                                 \
            arena_free_block(_name);                                                                              \
            _name = NULL;                                                                                                    \
        } else {                                                                                                             \
            _name->width++;                                                                                                  \
            _name->height++;                                                                                                 \
            _name->objects = (MapObject **)arena_alloc(cur_arena, (size_t)(_name->height) * sizeof(MapObject *));            \
            for (int i = 0; i < _name->height; i++) {                                                                        \
                _name->objects[i] = (MapObject *)arena_alloc(cur_arena, (size_t)(_name->width) * sizeof(MapObject));         \
            }                                                                                                                \
            bool was_main_object = false;                                                                                    \
            while (objects_list) {                                                                                           \
                MapObjectList *cur_object_list = objects_list;                                                               \
                _name->objects[cur_object_list->coords.y][cur_object_list->coords.x].object = cur_object_list->object;       \
                if (cur_object_list->is_main) {                                                                              \
                    if (!was_main_object) {                                                                                  \
                        _name->default_layer_coords.x = cur_object_list->coords.x;                                           \
                        _name->default_layer_coords.y = cur_object_list->coords.y;                                           \
                        was_main_object = true;                                                                              \
                    } else {                                                                                                 \
                        wprintf(L"Warning: Multiple main objects in layer %s\n", #_name);                                    \
                    }                                                                                                        \
                }                                                                                                            \
                if (!was_main_object) {                                                                                      \
                    _name->default_layer_coords = cur_object_list->coords;                                                   \
                }                                                                                                            \
                objects_list = cur_object_list->next;                                                                        \
                arena_free_block(cur_object_list);                                                                \
            }                                                                                                                \
        }                                                                                                                    \
    } while (0)

#define MAP_LAYER_3(arena, name, _objects) \
    MAP_LAYER_FULL(arena, name, {}, _objects)

#define MAP_LAYER_4(arena, name, _params, _objects) \
    MAP_LAYER_FULL(arena, name, _params, _objects)

#define MAP_LAYER(...) GET_MACRO_4(__VA_ARGS__, MAP_LAYER_4, MAP_LAYER_3)(__VA_ARGS__)

#endif
