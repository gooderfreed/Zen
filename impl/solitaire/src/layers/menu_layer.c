#include "../../inc/solitaire.h"

/*
 * Prepare menu screen
 */
static void prepare_menu_screen(Screen *screen) {
    fill_area(screen, 0, 0, SCREEN_HEIGHT, SCREEN_WIDTH, ' ', COLOR_BLACK, COLOR_WHITE);
    add_borders(screen, 0, 0, SCREEN_HEIGHT, SCREEN_WIDTH, COLOR_NONE, COLOR_WHITE, fat_border);
}

/*
 * Menu loop
 * Handles the menu loop
 */
static bool menu_cursor_loop(Core *core, wint_t key) {
    switch (key) {
        case L'w': case L'ц':            core_local_move(core, CURSOR_UP);   break;
        case L's': case L'ы': case L'і': core_local_move(core, CURSOR_DOWN); break;
        case KEY_SPACE: case KEY_ENTER:  core_action(core);                  break;
    }
    return false;
}

/*
 * Menu layer
 * Creates the menu layer
 */
MapLayer *menu_layer_init(Arena *arena, Game *game) {
    Menu     *menu     = init_menu(arena);
    Controls *controls = init_controls(arena);

    SET_BUTTON_CONTEXT(menu, 1, game);

    MapLayer *menu_layer = (MapLayer *)arena_alloc(arena, sizeof(MapLayer));

    *menu_layer = (MapLayer) {
        .prepare_screen = prepare_menu_screen,
        .layer_cursor_loop = menu_cursor_loop,
        .default_layer_coords = MENU_DEFAULT_COORDS,
        .height = MENU_LAYER_HEIGHT,
        .width = MENU_LAYER_WIDTH,
    };

    menu_layer->objects = (MapObject **)arena_alloc(arena, (size_t)(menu_layer->height) * sizeof(MapObject *));
    for (int i = 0; i < menu_layer->height; i++) {
        menu_layer->objects[i] = (MapObject *)arena_alloc(arena, (size_t)(menu_layer->width) * sizeof(MapObject));
    }
    
    menu_layer->objects[0][0].object = menu;
    menu_layer->objects[0][1].object = controls;

    return menu_layer;
}
