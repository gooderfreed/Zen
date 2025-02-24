/*
 * Stock implementation
 * Handles stock object management and foundation pile drawing
 */
#include "../../inc/solitaire.h"

// =========================
//  STOCK METHODS
// =========================
/*
 * Check if a card can be placed in the stock
 * Ensures that stock is balanced
 */
static bool is_stock_balanced(const Stock *stock, const Card *card) {
    Numeral min_value = King;
    Numeral max_value = Ace;
    for (Suit suit = Spades; suit < CARD_SUITS; suit++) {
        Numeral value = 0;
        if (stock->top_cards[suit]) {
            value = stock->top_cards[suit]->numeral;
        }
        min_value = (min_value < value) ? min_value : value;
        max_value = (max_value > value) ? max_value : value;
    }
    
    if (card->numeral > min_value + 2) return false;
    if (card->numeral - min_value == 2 && card->numeral == max_value) return false;
    
    return true;
}

/*
 * Check if a card can be placed in the stock pile
 * Ensures the card is of the correct suit and follows the foundation pile rules
 */
static bool can_place_in_stock_pile(const Stock *stock, const Card *card) {
    if (!stock->top_cards[card->suit]) {
        if (card->numeral != Ace) return false;
    }
    else if (card->numeral - stock->top_cards[card->suit]->numeral != 1) return false;

    return true;
}

/*
 * Place a card in the stock pile
 * Updates the stock with the new card and adjusts the top card pointer
 */
static void place_card_in_stock_pile(Stock *stock, Card *card) {
    card->object = Stock_enum;
    card->selected = false;
    stock->stock[card->suit][card->numeral - 1] = card;
    stock->top_cards[card->suit] = card;
}

/*
 * Try to place a card in the stock
 * Checks if the card can be placed in the stock and if it is balanced
 */
static bool try_place(Stock *stock, Card *target, Container *cursor_container) {
    if (!target) return false;
    if (can_place_in_stock_pile(stock, target)
        && is_stock_balanced(stock, target)) {
        
        if (target->selected && !container_is_empty(cursor_container)) {
            container_pop_element(cursor_container);
        }
        place_card_in_stock_pile(stock, target);
        return true;
    }
    return false;
}

/*
 * Check if the game is won
*/
static bool is_win(const Stock *stock) {
    for (int suit = 0; suit < CARD_SUITS; suit++) {
        if (!stock->top_cards[suit] || stock->top_cards[suit]->numeral != King) {
            return false;
        }
    }
    return true;
}

/*
 * Reset stock
 * Clears stock piles and top card pointers
 */
void stock_reset(Stock *stock) {
    for (int suit = 0; suit < CARD_SUITS; suit++) {
        stock->top_cards[suit] = NULL;
        for (int numeral = 0; numeral < CARD_NUMERALS; numeral++) {
            stock->stock[suit][numeral] = NULL;
        }
    }
}




// =========================
//  DRAWABLE INTERFACE IMPLEMENTATION
// =========================
/*
 * Draw stock piles on screen
 * Shows foundation piles for each suit with top cards
 */
static void print_stock(const void *stock_pointer, Screen *screen, const Cursor *cursor) {
    (void)cursor;
    const Stock *stock = (const Stock *)stock_pointer;
    
    fill_area(screen, BORDER_OFFSET_X, STOCK_X_BASE, STOCK_AREA_HEIGHT, STOCK_AREA_WIDTH, L' ', ((Color){0x0030992e}), COLOR_NONE);

    for (int suit = 0; suit < CARD_SUITS; suit++) {
        int x = STOCK_X_BASE + suit * CARD_WIDTH;

        if (stock->top_cards[suit]) {
            print_card(screen, stock->top_cards[suit], BORDER_OFFSET_Y, x, CARD_HEIGHT, CARD_WIDTH);
        }
        else {
            add_borders(screen, BORDER_OFFSET_Y, x, CARD_HEIGHT, CARD_WIDTH, ((Color){0x0030992e}), COLOR_WHITE, fat_border);
            screen->pixels[BORDER_OFFSET_Y + CARD_HEIGHT / 2][x + CARD_WIDTH / 2 - 1].symbol = suit_to_text((Suit)suit);
            screen->pixels[BORDER_OFFSET_Y + CARD_HEIGHT / 2][x + CARD_WIDTH / 2 - 1].foreground = (suit % 2 != 0) ? COLOR_RED : COLOR_BLACK;
        }
    }
}




// =========================
//  CURSOR_INTERACTABLE INTERFACE IMPLEMENTATION
// =========================
/*
 * Position cursor relative to stock piles
 * Places cursor at bottom of selected foundation pile
 */
static void place_cursor_in_stock(const void *stock_pointer, const Coords cursor_coords, Coords *target_coords) {
    (void)stock_pointer;
    
    target_coords->y = BORDER_OFFSET_Y + CARD_HEIGHT;
    target_coords->x = STOCK_CURSOR_BASE_X + CARD_WIDTH * cursor_coords.x;
}

/*
 * Handle cursor movement in stock area
 * Allows horizontal movement between foundation piles
 */
static void move_in_stock(const void *stock_pointer, Coords *coords, const Coords delta) {
    (void)stock_pointer;
    
    if (delta.y != 0) return;
    
    short new_x = coords->x + delta.x;
    coords->x = (new_x + CARD_SUITS) % CARD_SUITS;
}

/*
 * Get default coords
 * Gets the default coords of the stock
 */
static Coords get_default_coords(const void *stock_pointer) {
    (void)stock_pointer;
    return (Coords) {.x = 1, .y = 0};
}

/*
 * Get cursor config
 * Gets the cursor config of the stock
 */
static CursorConfig get_cursor_config_in_stock(const void *stock_pointer, const Coords cursor_coords) {
    (void)stock_pointer;
    (void)cursor_coords;
    return (CursorConfig) {
        .type = CURSOR_UP_WIDE,
        .background = COLOR_NONE,
        .foreground = COLOR_WHITE,
        .effect = Effect_Bold,
    };
}




// =========================
//  CARD_HANDLER INTERFACE IMPLEMENTATION
// =========================
/*
 * Check if a card can be placed in the stock
 * Ensures the card is of the correct suit and follows the foundation pile rules
 */
static bool can_place_in_stock(const void *stock_pointer, const Coords cursor_coords, const Container *container) {
    (void)cursor_coords;
    const Stock *stock = (const Stock *)stock_pointer;
    if (container->size != 1) return false;

    const Card *card = container_get_element(container, 0);
    if (card->suit != (Suit)cursor_coords.x) return false;

    return can_place_in_stock_pile(stock, card);
}

/*
 * Place a card in the stock
 * Updates the stock with the new card and adjusts the top card pointer
 */
static void place_cards_in_stock(void *stock_pointer, Coords cursor_coords, Container *container) {
    (void)cursor_coords;

    Stock *stock = (Stock *)stock_pointer;
    Card *card = container_pop_element(container);

    place_card_in_stock_pile(stock, card);
}




// =========================
//  POSITION_HANDLER INTERFACE IMPLEMENTATION
// =========================
/*
 * Save current position in stock
 * Sets current coordinates to restore coordinates
 */
static void save_current_pos_in_stock(void *stock_pointer, Coords current_coords) {
    Stock *stock = (Stock *)stock_pointer;
    SET_RESTORE_COORDS(stock, current_coords);
}

/*
 * Restore position in stock
 * Sets restore coordinates to current coordinates
 */
static void restore_pos_in_stock(void *stock_pointer, Coords *current_coords) {
    Stock *stock = (Stock *)stock_pointer;
    *current_coords = GET_RESTORE_COORDS(stock);
}




// =========================
//  UPDATABLE INTERFACE IMPLEMENTATION
// =========================
/*
 * Update stock
 * Places cards from deck and field into stock
 */
static void update_stock(void *stock_pointer, void *context) {
    if (!context) return;
    Stock *stock = (Stock *)stock_pointer;
    StockContext *stock_context = (StockContext *)context;
    
    Deck  *deck  = (Deck *)stock_context->deck;
    Field *field = (Field *)stock_context->field;
    Cursor *cursor = (Cursor *)stock_context->cursor;
    Container *cursor_container = (Container *)stock_context->cursor->cards;

    // Check if the game is won
    if (is_win(stock)) {
        CORE_CHANGE_LAYER(stock, WIN_ID);
        return;
    }

    // Place cards from deck to stock
    CardProvider *deck_provider = ((DeckMethods *)(deck->export_methods))->card_provider;
    if (try_place(stock, deck_provider->peek(deck, (Coords){0}), cursor_container)) {
        deck_provider->pop(deck, NULL);
        return;
    }
    
    // Place cards from field to stock
    CardProvider *field_provider = ((FieldMethods *)(field->export_methods))->card_provider;
    for (int x = 0; x < FIELD_WIDTH; x++) {
        if (!field->field[0][x]) continue;

        Card *card = field_provider->peek(field, (Coords){.x = (short)x});
        if (try_place(stock, card, cursor_container)) {
            if (cursor->coords.y != 0 && 
                card->coords.x == cursor->coords.x && 
                card->coords.y == cursor->coords.y) {
                    cursor->coords = (Coords){.x = cursor->coords.x, .y = card->coords.y - 1};
            }
            field_provider->pop(field, card);
            return;
        }
    }
}




/*
 * Initialize empty stock structure
 * Creates stock with empty foundation piles and required interfaces
 */
Stock init_stock(void) {
    Stock stock = {0};

    static Drawable drawable = {
        .is_active = true,
        .print = print_stock
    };

    static const CursorInteractable cursor_interactable = {
        .place_cursor       = place_cursor_in_stock,
        .move_cursor        = move_in_stock,
        .get_default_coords = get_default_coords,
        .get_cursor_config  = get_cursor_config_in_stock
    };

    static const CardHandler card_handler = {
        .can_take_cards = true,
        .can_give_cards = false,

        .can_place   = can_place_in_stock,
        .place_cards = place_cards_in_stock
    };

    static PositionHandler position_handler = {
        .restore_coords = {0},
        .save_current_pos = save_current_pos_in_stock,
        .restore_pos      = restore_pos_in_stock
    };

    static Updateable updateable = {
        .update = update_stock
    };

    stock.interfaces = (ObjectInterfaces) {
        .name             = "Stock",
        .capabilities     = {
            .is_drawable     = true,
            .is_cursor_interactable = true,
            .can_hold_cards  = true,
            .is_positionable = true,
            .requires_update = true,
            .requires_core   = true
        },
        .drawable            = &drawable,
        .cursor_interactable = &cursor_interactable,
        .card_handler        = &card_handler,
        .position_handler    = &position_handler,
        .updateable          = &updateable
    };

    return stock;
}
