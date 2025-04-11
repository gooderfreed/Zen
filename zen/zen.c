/*
 * Core engine implementation
 * Contains main game loop logic and object management
 */
#include "zen.h"

/*
 * Initialize core structure
 * Allocates memory for the core structure and binds it to the given arena
 */
Zen *zen_init(Arena *arena) {
    Zen *zen = (Zen *)arena_alloc(arena, sizeof(Zen));
    
    zen->arena        = arena;
    zen->input_type   = INPUT_TYPE_CURSOR;
    zen->time_manager = init_time_manager();
    zen->frame_timer  = init_frame_timer();
    zen->tick_counter = init_tick_counter();

    return zen;
}

/*
 * Set new map to core engine
 * Assigns new map and validates all interfaces
 */
void zen_set_map(Zen *zen, Map *map) {
    zen->map = map;

    static CoreDependent core_dependent;
    core_dependent = (CoreDependent) {
        .zen = zen,
        .change_layer = zen_change_layer,
        .action = zen_action,
        .global_move = zen_global_move,
        .local_move = zen_local_move,
        .get_screen = zen_get_screen,
        .shutdown = zen_shutdown
    };

    for (int z = 0; z < map->layers_count; z++) {
        MapLayer *layer = map_get_layer(map, z);
        SignalListenersList *listeners = NULL;
        EmitterList *emitters = NULL;

        for (int y = 0; y < layer->height; y++) {
            for (int x = 0; x < layer->width; x++) {
                MapObject object = map_get_object(map, (Coords){.x = (short)x, .y = (short)y, .z = (short)z});
                if (!object.object) continue;

                if (IS_TICK_DEPENDENT(object.object)) {
                    SET_TICK_COUNTER(object.object, &zen->tick_counter);
                }

                if (IS_CORE_DEPENDENT(object.object)) {
                    ObjectInterfaces *interfaces = GET_INTERFACES(object.object);
                    interfaces->core_dependent = &core_dependent;
                }

                if (IS_OBSERVER(object.object)) {
                    Observer *observer = OBSERVER_HANDLER(object.object);
                    if (observer->subscriptions) {
                        while (observer->subscriptions) {
                            SignalSubscription subscription = observer->subscriptions->subscription;
                            listeners = add_subscription_to_listeners(zen->arena, listeners, subscription, object.object);

                            observer->subscriptions = observer->subscriptions->next;
                        }
                    }
                }

                if (IS_EMITTER(object.object)) {
                    Emitter *emitter = EMITTER_HANDLER(object.object);
                    EmitterList *new_emitter_list = arena_alloc(zen->arena, sizeof(EmitterList));
                    new_emitter_list->emitter = emitter;
                    new_emitter_list->next = emitters;
                    emitters = new_emitter_list;
                }
            }
        }

        link_observers_to_emitters(zen->arena, listeners, emitters);

    }

    MapLayer *layer = map_get_current_layer(zen->map);
    if (layer->prepare_screen) {
        layer->prepare_screen(zen->screen);
    }
}

/*
 * Set new cursor to core engine
 * Assigns new cursor and validates all interfaces
 */
void zen_set_cursor(Zen *zen, Cursor *cursor) {
    zen->cursor = cursor;
}

/*
 * Set new screen to core engine
 * Assigns new screen and validates all interfaces
 */
void zen_set_screen(Zen *zen, Screen *screen) {
    zen->screen = screen;
}

/*
 * Get screen from core engine
 * Returns screen from core engine
 */
Screen *zen_get_screen(Zen *zen) {
    return zen->screen;
}

/*
 * Shutdown core engine
 * Frees all dynamic objects and shuts down screen
 */
void zen_shutdown(Zen *zen) {
    screen_shutdown(zen->screen);
    zen_free(zen);
    
    #ifdef DEBUG
    print_arena(zen->arena);
    print_fancy(zen->arena, 102);
    #endif
    
    arena_free(zen->arena);
}

/*
 * Handle cursor movement request in current object
 * Moves cursor if target object is interactable and allows movement
 */
void zen_local_move(Zen *zen, Coords move) {
    Cursor *cursor = zen->cursor;
    if (!IS_CURSOR_INTERACTABLE(cursor->subject)) return;

    Coords coords = {.x = cursor->coords.x, .y = cursor->coords.y};
    MOVE_CURSOR(cursor->subject, &coords, move);

    // Update cursor position only if movement was successful
    if (coords.x != cursor->coords.x || coords.y != cursor->coords.y) {
        cursor->coords = coords;
    }
}

/*
 * Handle action on current cursor position
 * Implements main logic for cursor interaction:
 * - Button handling
 * - Card selection
 * - Card movement between objects
 */
void zen_action(Zen *zen) {
    Cursor *cursor = zen->cursor;

    // Handle button press if cursor is on a button
    if (is_button(cursor->subject, cursor->coords)) {
        handle_button(cursor->subject, cursor->coords);
        zen_update(zen);
        return;
    }

    // Handle card operations
    if (cursor->cards->size != 0) {
        void *first_card = cursor->cards->container[0];

        // Toggle card selection if clicking on the same card
        if (cursor->subject == cursor->cards->source &&
            GET_IS_SAME_CARD(cursor->subject, cursor->coords, first_card)) {
            SELECT_CARDS(cursor->subject, cursor->coords, cursor->cards);
        }
        // Move cards if source can give and target can take
        else if (GET_CAN_GIVE_CARDS(cursor->cards->source) && 
                 GET_CAN_TAKE_CARDS(cursor->subject) &&
                 GET_CAN_PLACE_CARDS(cursor->subject, cursor->coords, cursor->cards)) {
            GET_CARDS(cursor->cards->source, cursor->cards);
            PLACE_CARDS(cursor->subject, cursor->coords, cursor->cards);
        }
    }
    // Select cards if no cards are currently selected
    else {
        if (GET_CAN_GIVE_CARDS(cursor->subject)) {
            SELECT_CARDS(cursor->subject, cursor->coords, cursor->cards);
        }
    }
    zen_update(zen);
}

/*
 * Update screen with current game state
 * Draws all visible objects and cursor
 */
void zen_update_screen(Zen *zen) {
    // Draw all objects on map
    MapLayer *layer = map_get_current_layer(zen->map);
    for (int y = 0; y < layer->height; y++) {
        for (int x = 0; x < layer->width; x++) {
            void *target_struct = layer->objects[y][x].object;
            if (!target_struct) continue;
            
            if (IS_ACTIVE_DRAWABLE(target_struct)) {
                DRAW(target_struct, zen->screen, zen->cursor);
            }
        }
    }

    // Draw cursor and update screen
    print_cursor(zen->cursor, zen->screen);
    draw_fps_stats(zen->frame_timer.stats, zen->screen);
    print_screen(zen->screen);
}

/*
 * Handle global map movement
 * Saves current position, moves map and restores position for new object
 */
void zen_global_move(Zen *zen, Coords move) {
    // Save current position if object supports it
    if (IS_POSITIONABLE(zen->cursor->subject)) {
        SAVE_POSITION(zen->cursor->subject, zen->cursor->coords);
    }

    // Move map and get new object
    map_move(zen->map, move);
    MapObject target = map_get_current_object(zen->map);

    // Update cursor for new object
    if (target.object) {
        zen->cursor->subject = target.object;
        if (IS_POSITIONABLE(zen->cursor->subject)) {
            RESTORE_POSITION(zen->cursor->subject, &zen->cursor->coords);
        }
        else {
            zen->cursor->coords = GET_DEFAULT_COORDS(target.object);
        }
    }
}

/*
 * Free all dynamic objects on map
 * Called during shutdown to prevent memory leaks
 */
void zen_free(Zen *zen) {
    for (int z = 0; z < zen->map->layers_count; z++) {
        MapLayer *layer = map_get_layer(zen->map, z);
        for (int y = 0; y < layer->height; y++) {
            for (int x = 0; x < layer->width; x++) {
                MapObject object = map_get_object(zen->map, (Coords){.x = (short)x, .y = (short)y, .z = (short)z});
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
void zen_update(Zen *zen) {
    MapLayer *layer = map_get_current_layer(zen->map);
    for (int y = 0; y < layer->height; y++) {
        for (int x = 0; x < layer->width; x++) {
            MapObject object = layer->objects[y][x];
            if (!object.object) continue;

            if (IS_UPDATEABLE(object.object)) {
                UPDATE(object.object);
            }
        }
    }
}

/*
 * Change layer of the map
 * Updates cursor position to the default position of the new layer
 */
void zen_change_layer(Zen *zen, int layer) {
    Map *map = zen->map;
    map_move_layer(map, layer);
    MapObject object = map_get_object(map, map->global_coords);

    // Update cursor position to default position of new layer
    if (IS_CURSOR_INTERACTABLE(object.object)) {
        zen->cursor->coords = GET_DEFAULT_COORDS(object.object);
    }
    zen->cursor->subject = object.object;

    MapLayer *layer_ = map_get_current_layer(zen->map);
    if (layer_->prepare_screen) {
        layer_->prepare_screen(zen->screen);
    }
}

/*
 * Main game loop
 * Handles game loop and input processing
 */
void zen_manage_loop(Zen *zen, wint_t key) {
    MapLayer *layer = map_get_current_layer(zen->map);
    MapObject obj = map_get_current_object(zen->map);
    bool handled = false;

    switch (zen->input_type) {
    
    case INPUT_TYPE_DIRECT: {
        if (IS_INPUT_HANDLER(obj.object)) {
            handled = HANDLE_INPUT(obj.object, zen, key);
        }
        break;
    }
        
    case INPUT_TYPE_CURSOR: {
        if (IS_CURSOR_INTERACTABLE(obj.object)) {
            handled = layer->layer_cursor_loop(zen, key);
        }
        break;
    }

    default: break;
    }
    
    if (!handled && layer->layer_loop) layer->layer_loop(zen, key);
}

/*
 * Set target FPS
 * Configures the desired frames per second for the game loop.
 */
void zen_set_target_fps(Zen *zen, int fps) {
    set_target_fps(&zen->frame_timer, fps);
}

/*
 * Check if the game should close
 * Updates time and processes game logic and rendering if needed.
 */
bool zen_should_close(Zen *zen) {
    update_time_manager(&zen->time_manager);

    // Update game logic if it's time for a new tick
    if (should_update_ticks(&zen->tick_counter, &zen->time_manager)) {
        zen_update(zen);
    }

    // Render frame if it's time
    if (should_render_frame(&zen->frame_timer, &zen->time_manager)) {
        zen_update_screen(zen);
    }
    return false;
}

/*
 * Check for user input
 * Returns true if there is input available from stdin.
 */
bool zen_has_input(void) {
    fd_set readfds;
    FD_ZERO(&readfds);
    FD_SET(STDIN_FILENO, &readfds);

    struct timeval tv = {0, 0}; // Non-blocking check
    return select(STDIN_FILENO + 1, &readfds, NULL, NULL, &tv) > 0;
}

/*
 * Enable FPS statistics
 * Allocates and enables FPS tracking for performance monitoring.
 */
void zen_enable_fps_stats(Zen *zen) {
    if (zen->frame_timer.stats) arena_free_block(zen->frame_timer.stats);
    zen->frame_timer.stats = create_fps_stats(zen->arena);
}

/*
 * Disable FPS statistics
 * Frees and disables FPS tracking.
 */
void zen_disable_fps_stats(Zen *zen) {
    if (zen->frame_timer.stats) arena_free_block(zen->frame_timer.stats);
    zen->frame_timer.stats = NULL;
}

/*
 * Toggle FPS display
 * Enables or disables drawing FPS on the screen.
 */
void zen_show_fps(Zen *zen, bool state) {
    if (zen->frame_timer.stats) zen->frame_timer.stats->draw_to_screen = state;
}

/*
 * Set ticks per second
 * Configures the number of logic updates per second.
 */
void zen_set_ticks_per_second(Zen *zen, int ticks_per_second) {
    set_ticks_per_second(&zen->tick_counter, ticks_per_second);
}
