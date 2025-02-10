#include "../inc/snake.h"

/*
 * Border characters for screen elements
 * Used for drawing borders around cards and game areas
 */
const wchar_t fat_border[8] = {
    L'═', L'║', L'╔', L'╗', L'╚', L'╝', L'╠', L'╣'
};

const wchar_t card_border[8] = {
    L'─', L'│', L'┌', L'┐', L'└', L'┘', L'├', L'┤'
};

/*
 * Initialize game
 * Initializes the game with all necessary components
 */
static void init_game(Core *core) {
    Arena *arena = core->arena;
    Screen *screen = init_screen(arena, SCREEN_WIDTH, SCREEN_HEIGHT, COLOR_GREEN, COLOR_RESET, ' ');
    MapLayer *game_layer = game_layer_init(arena);

    // create map
    Map *map = init_map(arena, 1, (Coords) {.x = 0, .y = 1, .z = 0});
    map_set_layer(map, game_layer, 0);

    core_set_screen(core, screen);
    core_set_map(core, map);

    core->input_type = INPUT_TYPE_DIRECT;
}

/*
 * Game loop
 * Main game loop
 */
static void game_loop(Core *core) {
    core_set_target_fps(core, 10);
    core_set_ticks_per_second(core, 10);

    while (!core_should_close(core)) {
        if (core_has_input()) {
            wint_t ch = getwchar();

            if (ch == KEY_ESC) {
                core_shutdown(core);
                break;
            }

            core_manage_loop(core, ch);
        }
    }
}

int main(void) {
    srand((unsigned int)time(NULL));

    size_t size = 1024 * 1024;
    Arena *arena = arena_new_dynamic(size);

    Core *core = core_init(arena);
    
    init_game(core);
    game_loop(core);
}
