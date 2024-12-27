/*
 * Core engine implementation
 * Contains main game loop logic and object management
 */
#include "../inc/core.h"

/*
 * Initialize core engine with provided components
 * Validates all interfaces before returning
 */
Core init_core(Map *map, Cursor *cursor, Screen *screen) {
    Core core = {
        .screen = screen,
        .map = map,
        .cursor = cursor,
    };

    core_validate_interfaces(&core);
    MapLayer *layer = map_get_current_layer(core.map);
    if (layer->prepare_screen) {
        layer->prepare_screen(core.screen);
    }

    return core;
}

void core_shutdown(Core *core) {
    screen_shutdown(core->screen);
    core_free(core);
}

/*
 * Handle cursor movement request in current object
 * Moves cursor if target object is interactable and allows movement
 */
void core_local_move(Core *core, Coords move) {
    Cursor *cursor = core->cursor;
    if (!IS_INTERACTABLE(cursor->subject)) return;

    Coords coords = {.x = cursor->coords.x, .y = cursor->coords.y};
    MOVE(cursor->subject, &coords, move);

    // Update cursor position only if movement was successful
    if (coords.x != cursor->coords.x || coords.y != cursor->coords.y) {
        cursor->coords = coords;
    }
}

/*
 * Handle action on current cursor position
 * Implements main game logic for card manipulation:
 * - Button handling
 * - Card selection
 * - Card movement between objects
 */
void core_action(Core *core) {
    Cursor *cursor = core->cursor;

    // Handle button press if cursor is on a button
    if (is_button(cursor->subject, cursor->coords)) {
        handle_button(cursor->subject, cursor->coords);
        core_update(core);
        return;
    }

    // Handle card operations
    if (cursor->cards->size != 0) {
        Card *first_card = cursor->cards->container[0];

        // Toggle card selection if clicking on the same card
        if (cursor->subject == cursor->cards->source &&
            IS_SAME_CARD(cursor->subject, cursor->coords, first_card)) {
            SELECT_CARDS(cursor->subject, cursor->coords, cursor->cards);
        }
        // Move cards if source can give and target can take
        else if (CAN_GIVE_CARDS(cursor->cards->source) && 
                 CAN_TAKE_CARDS(cursor->subject) &&
                 CAN_PLACE_CARDS(cursor->subject, cursor->coords, cursor->cards)) {
            GET_CARDS(cursor->cards->source, cursor->cards);
            PLACE_CARDS(cursor->subject, cursor->coords, cursor->cards);
        }
    }
    // Select cards if no cards are currently selected
    else {
        if (CAN_GIVE_CARDS(cursor->subject)) {
            SELECT_CARDS(cursor->subject, cursor->coords, cursor->cards);
        }
    }
    core_update(core);
}

/*
 * Update screen with current game state
 * Draws all visible objects and cursor
 */
void core_update_screen(Core *core) {
    // Draw all objects on map
    MapLayer *layer = map_get_current_layer(core->map);
    for (int y = 0; y < MAP_HEIGHT; y++) {
        for (int x = 0; x < MAP_WIDTH; x++) {
            void *target_struct = layer->objects[y][x].object;
            if (!target_struct) continue;
            
            if (IS_DRAWABLE(target_struct)) {
                DRAW(target_struct, core->screen, core->cursor);
            }
        }
    }

    // Draw cursor and update screen
    print_cursor(core->cursor, core->screen);
    print_screen(core->screen);
}

/*
 * Handle global map movement
 * Saves current position, moves map and restores position for new object
 */
void core_global_move(Core *core, Coords move) {
    // Save current position if object supports it
    if (IS_POSITIONABLE(core->cursor->subject)) {
        SAVE_POSITION(core->cursor->subject, core->cursor->coords);
    }

    // Move map and get new object
    map_move(core->map, move);
    MapObject target = map_get_current_object(core->map);

    // Update cursor for new object
    if (target.object) {
        core->cursor->subject = target.object;
        if (IS_POSITIONABLE(core->cursor->subject)) {
            RESTORE_POSITION(core->cursor->subject, &core->cursor->coords);
        }
        else {
            core->cursor->coords = GET_DEFAULT_COORDS(target.object);
        }
    }
}

/*
 * Free all dynamic objects on map
 * Called during shutdown to prevent memory leaks
 */
void core_free(Core *core) {
    for (int z = 0; z < MAP_LAYERS; z++) {
        for (int y = 0; y < MAP_HEIGHT; y++) {
            for (int x = 0; x < MAP_WIDTH; x++) {
                MapObject object = map_get_object(core->map, (Coords){.x = (short)x, .y = (short)y, .z = (short)z});
                if (!object.object) continue;

                if (IS_DYNAMIC(object.object)) {
                    FREE(object.object);
                }
            }
        }
    }
}

/*
 * Update all objects on map
 * Calls update for each object
 */
void core_update(Core *core) {
    MapLayer *layer = map_get_current_layer(core->map);
    for (int y = 0; y < MAP_HEIGHT; y++) {
        for (int x = 0; x < MAP_WIDTH; x++) {
            MapObject object = layer->objects[y][x];
            if (!object.object) continue;

            if (IS_UPDATEABLE(object.object)) {
                UPDATE(object.object);
            }
        }
    }
}

/*
 * Validate interfaces of all objects on map
 * Ensures all objects have correctly implemented required interfaces
 * Exits program if validation fails
 */
void core_validate_interfaces(Core *core) {
    for (int z = 0; z < MAP_LAYERS; z++) {
        for (int y = 0; y < MAP_HEIGHT; y++) {
            for (int x = 0; x < MAP_WIDTH; x++) {
                MapObject object = map_get_object(core->map, (Coords){.x = (short)x, .y = (short)y, .z = (short)z});
                if (!object.object) continue;
                
                const ObjectInterfaces *interfaces = (const ObjectInterfaces*)object.object;
                if (!validate_object_interfaces(interfaces)) {
                    exit(1);
                }
            }
        }
    }
}

/*
 * Change layer of the map
 * Updates cursor position to the default position of the new layer
 */
void core_change_layer(Core *core, int layer) {
    Map *map = core->map;
    map_move_layer(map, layer);
    MapObject object = map_get_object(map, map->global_coords);

    // Update cursor position to default position of new layer
    if (IS_INTERACTABLE(object.object)) {
        core->cursor->coords = GET_DEFAULT_COORDS(object.object);
    }
    core->cursor->subject = object.object;

    MapLayer *layer_ = map_get_current_layer(core->map);
    if (layer_->prepare_screen) {
        layer_->prepare_screen(core->screen);
    }
}
