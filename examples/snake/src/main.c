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
static void init_game(Zen *zen) {
    Arena *arena = zen->arena;
    Screen *screen = init_screen(arena, SCREEN_WIDTH, SCREEN_HEIGHT, COLOR_GREEN, COLOR_NONE, ' ');
    MapLayer *game_layer = game_layer_init(arena);

    // create map
    Map *map = init_map(arena, 1, COORDS(0, 1, 0));
    map_set_layer(map, game_layer, 0);

    zen_set_screen(zen, screen);
    zen_set_map(zen, map);

    zen->input_type = INPUT_TYPE_DIRECT;
}

/*
 * Game loop
 * Main game loop
 */
static void game_loop(Zen *zen) {
    zen_set_target_fps(zen, 10);
    zen_set_ticks_per_second(zen, 10);

    while (!zen_should_close(zen)) {
        if (zen_has_input()) {
            wint_t ch = getwchar();

            if (ch == KEY_ESC) {
                zen_shutdown(zen);
                break;
            }

            zen_manage_loop(zen, ch);
        }
    }
}

int main(void) {
    srand((unsigned int)time(NULL));

    ssize_t size = 1024 * 80;  // 73928kb is minimum
    Arena *arena = arena_new_dynamic(size);

    Zen *zen = zen_init(arena);
    
    init_game(zen);
    game_loop(zen);
}
