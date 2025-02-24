#include "../../inc/solitaire.h"

/*
 * Initialize game
 * Creates game with all necessary components
 */
static Game *game_init(Arena *arena) {
    Game *game = (Game *)arena_alloc(arena, sizeof(Game));

    static Deck deck;
    static Field field;
    static Stock stock;

    deck = generate_deck();
    field = init_field();
    stock = init_stock();

    *game = (Game) {
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
    field_reset(game->field, game->deck);
    stock_reset(game->stock);
}

/*
 * Prepare game screen
 */
static void prepare_game_screen(Screen *screen) {
    fill_area(screen, 0, 0, SCREEN_HEIGHT, SCREEN_WIDTH, ' ', ((Color){0x0030992e}), COLOR_NONE);
    add_borders(screen, 0, 0, SCREEN_HEIGHT, SCREEN_WIDTH, ((Color){0x004a4201}), ((Color){0x00242001}), fat_border);
    add_separator(screen, DECK_OFFSET + BORDER_OFFSET_Y - 1, 0, ((Color){0x004a4201}), ((Color){0x00242001}), fat_border);
}

/*
 * Game loop
 * Handles game loop
 */
static void game_loop(Core *core, wint_t key) {
    switch (key) {
        case L'q': case L'й':            core_change_layer(core, MENU_ID);     break;
        case KEY_CTRL_A:                 core_global_move(core, CURSOR_LEFT);  break;
        case KEY_CTRL_D:                 core_global_move(core, CURSOR_RIGHT); break;
    }
}

/*
 * Game cursor loop
 * Handles game cursor loop
 */
static bool cursor_loop(Core *core, wint_t key) {
    switch (key) {
        case L'a': case L'ф':            core_local_move(core, CURSOR_LEFT);   break;
        case L'd': case L'в':            core_local_move(core, CURSOR_RIGHT);  break;
        case L'w': case L'ц':            core_local_move(core, CURSOR_UP);     break;
        case L's': case L'ы': case L'і': core_local_move(core, CURSOR_DOWN);   break;
        case KEY_SPACE: case KEY_ENTER:  core_action(core);                    break;
    }
    return false;
}

/*
 * Initialize game layer
 * Creates game layer with required interfaces
 */
MapLayer *game_layer_init(Arena *arena, Cursor *cursor) {
    Game *game = game_init(arena);

    StockContext *stock_context = (StockContext *)arena_alloc(arena, sizeof(StockContext));
    *stock_context = (StockContext) {
        .deck  = game->deck,
        .field = game->field,
        .cursor = cursor,
    };

    SET_UPDATE_CONTEXT(game->stock,   stock_context);
    SET_BUTTON_CONTEXT(game->deck, 0, cursor->cards);

    MapLayer *game_layer = (MapLayer *)arena_alloc(arena, sizeof(MapLayer));

    *game_layer = (MapLayer) {
        .prepare_screen = prepare_game_screen,
        .layer_loop = game_loop,
        .layer_cursor_loop = cursor_loop,
        .default_layer_coords = GAME_DEFAULT_COORDS,
        .layer_main_object = game,
        .height = GAME_LAYER_HEIGHT,
        .width = GAME_LAYER_WIDTH,
    };

    game_layer->objects = (MapObject **)arena_alloc(arena, (size_t)(game_layer->height) * sizeof(MapObject *));
    for (int i = 0; i < game_layer->height; i++) {
        game_layer->objects[i] = (MapObject *)arena_alloc(arena, (size_t)(game_layer->width) * sizeof(MapObject));
    }

    game_layer->objects[0][0].object = game->deck;
    game_layer->objects[0][1].object = game->field;
    game_layer->objects[0][2].object = game->stock;

    return game_layer;
}

