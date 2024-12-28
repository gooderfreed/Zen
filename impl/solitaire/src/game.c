#include "../inc/solitare.h"

/*
 * Initialize game
 * Creates game with all necessary components
 */
static Game game_init(void) {
    static Deck deck;
    static Field field;
    static Stock stock;

    deck = generate_deck();
    field = init_field(&deck);
    stock = init_stock();

    Game game = {
        .deck  = &deck,
        .field = &field,
        .stock = &stock
    };

    return game;
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
 * Initialize game layer
 * Creates game layer with required interfaces
 */
MapLayer game_layer_init(Core *core, Container *container) {
    (void)core;
    static Game game;
    game = game_init();

    static StockContext stock_context;
    stock_context.deck  = game.deck;
    stock_context.field = game.field;
    stock_context.core  = core;
    stock_context.cursor_container = container;

    SET_UPDATE_CONTEXT(game.stock,   &stock_context);
    SET_BUTTON_CONTEXT(game.deck, 0, container);

    return (MapLayer) {
        .prepare_screen = prepare_game_screen,
        .default_layer_coords = GAME_DEFAULT_COORDS,
        .objects = {
            [0][0] = {.object = game.deck},
            [0][1] = {.object = game.field},
            [0][2] = {.object = game.stock}
        }
    };
}

