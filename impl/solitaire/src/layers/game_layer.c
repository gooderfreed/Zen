#include "../../inc/solitaire.h"

/*
 * Initialize game
 * Creates game with all necessary components
 */
static Game *game_init(Arena *arena) {
    Game *game = (Game *)arena_alloc(arena, sizeof(Game));

    Deck  *deck  = generate_deck(arena);
    Field *field = init_field(arena);
    Stock *stock = init_stock(arena);

    *game = (Game) {
        .deck  = deck,
        .field = field,
        .stock = stock
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
static bool game_cursor_loop(Core *core, wint_t key) {
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

    SET_UPDATE_CONTEXT(game->stock, stock_context);
    SET_BUTTON_CONTEXT(game->deck, "handle_next_card_button", cursor->cards);

    MapLayer *game_layer = NULL;
    MAP_LAYER(arena, game_layer, {
        prepare_screen = prepare_game_screen;
        loop = game_loop;
        cursor_loop = game_cursor_loop;
        main_object = game;
    }, {
        OBJECT(game->deck,  COORDS(0, 0));
        OBJECT(game->field, COORDS(1, 0), {is_main = true;});
        OBJECT(game->stock, COORDS(2, 0));
    });

    return game_layer;
}

