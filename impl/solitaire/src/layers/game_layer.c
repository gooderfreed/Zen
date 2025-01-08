#include "../inc/solitaire.h"

/*
 * Initialize game
 * Creates game with all necessary components
 */
static Game game_init(void) {
    static Deck deck;
    static Field field;
    static Stock stock;

    deck = generate_deck();
    field = init_field();
    stock = init_stock();

    Game game = {
        .deck  = &deck,
        .field = &field,
        .stock = &stock
    };

    return game;
}

/*
 * Reset game
 * Resets the game state
 */
void game_reset(Game *game) {
    deck_reset(game->deck);
    prepare_field(game->field, game->deck);
    stock_reset(game->stock);
}

/*
 * Prepare game screen
 */
static void prepare_game_screen(Screen *screen) {
    fill_area(screen, 0, 0, SCREEN_HEIGHT, SCREEN_WIDTH, ' ', COLOR_GREEN, COLOR_RESET);
    add_borders(screen, 0, 0, SCREEN_HEIGHT, SCREEN_WIDTH, COLOR_BLACK, COLOR_BLUE, fat_border);
    add_separator(screen, DECK_OFFSET + BORDER_OFFSET_Y - 1, 0, COLOR_BLACK, COLOR_BLUE, fat_border);
}

/*
 * Game loop
 * Handles game loop
 */
static void game_loop(Core *core, wint_t key) {
    switch (key) {
        case L'q': case L'й':            core_change_layer(core, MENU_ID);     break;
        case L'a': case L'ф':            core_local_move(core, CURSOR_LEFT);   break;
        case L'd': case L'в':            core_local_move(core, CURSOR_RIGHT);  break;
        case L'w': case L'ц':            core_local_move(core, CURSOR_UP);     break;
        case L's': case L'ы': case L'і': core_local_move(core, CURSOR_DOWN);   break;
        case KEY_SPACE: case KEY_ENTER:  core_action(core);                    break;
        case KEY_CTRL_A:                 core_global_move(core, CURSOR_LEFT);  break;
        case KEY_CTRL_D:                 core_global_move(core, CURSOR_RIGHT); break;
    }
}

/*
 * Initialize game layer
 * Creates game layer with required interfaces
 */
MapLayer game_layer_init(Container *container) {
    static Game game;
    game = game_init();

    static StockContext stock_context;
    stock_context.deck  = game.deck;
    stock_context.field = game.field;
    stock_context.cursor_container = container;

    SET_UPDATE_CONTEXT(game.stock,   &stock_context);
    SET_BUTTON_CONTEXT(game.deck, 0, container);

    return (MapLayer) {
        .prepare_screen = prepare_game_screen,
        .default_layer_coords = GAME_DEFAULT_COORDS,
        .layer_loop = game_loop,
        .layer_main_object = &game,
        .objects = {
            [0][0] = {.object = game.deck},
            [0][1] = {.object = game.field},
            [0][2] = {.object = game.stock}
        }
    };
}

