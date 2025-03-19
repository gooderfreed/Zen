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

    SET_BUTTON_CONTEXT(menu, "on_start_click", game);

    MapLayer *menu_layer = NULL;
    MAP_LAYER(arena, menu_layer, {
        prepare_screen = prepare_menu_screen;
        cursor_loop = menu_cursor_loop;
        main_object = menu;
    }, {
        OBJECT(menu,     COORDS(0, 0));
        OBJECT(controls, COORDS(1, 0));
    });

    return menu_layer;
}
