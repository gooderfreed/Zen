/*
 * Copyright 2024 Cheban Sergiy
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
*/

/*
 * Stock implementation
 * Handles stock object management and foundation pile drawing
 */
#include "../inc/solitare.h"

/*
 * Draw stock piles on screen
 * Shows foundation piles for each suit with top cards
 */
static void print_stock(void *stock_pointer, Screen *screen, const Cursor *cursor) {
    (void)cursor;
    Stock *stock = (Stock *)stock_pointer;
    
    fill_area(screen, BORDER_OFFSET_X, STOCK_X_BASE, STOCK_AREA_HEIGHT, STOCK_AREA_WIDTH, L' ', COLOR_GREEN, COLOR_RESET);

    for (int suit = 0; suit < CARD_SUITS; suit++) {
        int x = STOCK_X_BASE + suit * CARD_WIDTH;

        if (stock->top_cards[suit]) {
            print_card(screen, stock->top_cards[suit], BORDER_OFFSET_Y, x, CARD_HEIGHT, CARD_WIDTH);
        }
        else {
            add_borders(screen, BORDER_OFFSET_Y, x, CARD_HEIGHT, CARD_WIDTH, COLOR_GREEN, COLOR_WHITE, fat_border);
            screen->data[BORDER_OFFSET_Y + CARD_HEIGHT / 2][x + CARD_WIDTH / 2 - 1] = suit_to_text((Suit)suit);
            screen->foreground[BORDER_OFFSET_Y + CARD_HEIGHT / 2][x + CARD_WIDTH / 2 - 1] = (suit % 2 != 0) ? COLOR_RED : COLOR_BLACK;
        }
    }
}

/*
 * Handle cursor movement in stock area
 * Allows horizontal movement between foundation piles
 */
static void move_in_stock(void *stock_pointer, Coords *coords, Coords delta) {
    (void)stock_pointer;
    
    if (delta.y != 0) return;
    
    short new_x = coords->x + delta.x;
    coords->x = (new_x + CARD_SUITS) % CARD_SUITS;
}

/*
 * Position cursor relative to stock piles
 * Places cursor at bottom of selected foundation pile
 */
static void place_cursor_in_stock(void *stock_pointer, Coords cursor_coords, Coords *target_coords) {
    (void)stock_pointer;
    
    target_coords->y = BORDER_OFFSET_Y + CARD_HEIGHT;
    target_coords->x = STOCK_CURSOR_BASE_X + CARD_WIDTH * cursor_coords.x;
}

/*
 * Check if a card can be placed in the stock
 * Ensures the card is of the correct suit and follows the foundation pile rules
 */
static bool can_place_in_stock(void *stock_pointer, Coords cursor_coords, Container *container) {
    Stock *stock = (Stock *)stock_pointer;
    if (container->size != 1) return false;

    Card *card = container_get_element(container, 0);
    if (card->suit != (Suit)cursor_coords.x) return false;

    if (!stock->top_cards[card->suit]) {
        if (card->numeral != Ace) return false;
    }
    else if (card->numeral != stock->top_cards[card->suit]->numeral + 1) return false;

    return true;
}

/*
 * Place a card in the stock
 * Updates the stock with the new card and adjusts the top card pointer
 */
static void place_cards_in_stock(void *stock_pointer, Coords cursor_coords, Container *container) {
    (void)cursor_coords;

    Stock *stock = (Stock *)stock_pointer;
    Card *card = container_pop_element(container);

    stock->stock[card->suit][card->numeral] = card;
    stock->top_cards[card->suit] = card;
}

/*
 * Save current position in stock
 * Sets current coordinates to restore coordinates
 */
static void save_current_pos_in_stock(void *stock_pointer, Coords current_coords) {
    Stock *stock = (Stock *)stock_pointer;
    GET_RESTORE_COORDS(stock) = current_coords;
}

/*
 * Restore position in stock
 * Sets restore coordinates to current coordinates
 */
static void restore_pos_in_stock(void *stock_pointer, Coords *current_coords) {
    Stock *stock = (Stock *)stock_pointer;
    *current_coords = GET_RESTORE_COORDS(stock);
}

static void update_stock(void *stock_pointer, void *context) {
    Stock *stock = (Stock *)stock_pointer;
    StockContext *stock_context = (StockContext *)context;

    (void)stock;
    wprintf(L"update_stock\n");

    if (stock_context->deck->pointer == NULL) return;

    // Card *card = draw_card(stock_context->deck);
    // stock->stock[card->suit][card->numeral] = card;
    // stock->top_cards[card->suit] = card;
}

/*
 * Initialize empty stock structure
 * Creates stock with empty foundation piles and required interfaces
 */
Stock init_stock(void) {
    Stock stock = {0};

    static const Drawable drawable = {
        .print = print_stock
    };

    static const Interactable interactable = {
        .place_cursor = place_cursor_in_stock,
        .move         = move_in_stock
    };

    static const CardHandler card_handler = {
        .can_take_cards = true,
        .can_give_cards = false,

        .can_place    = can_place_in_stock,
        .place_cards  = place_cards_in_stock
    };

    static PositionHandler position_handler = {
        .restore_coords = {0},
        .save_current_pos = save_current_pos_in_stock,
        .restore_pos = restore_pos_in_stock
    };

    static Updateable updateable = {
        .update = update_stock
    };

    stock.interfaces = (ObjectInterfaces) {
        .name           = "Stock",
        .capabilities = {
            .is_drawable     = true,
            .is_interactable = true,
            .can_hold_cards  = true,
            .is_positionable = true,
            .requires_update = true
        },
        .drawable       = &drawable,
        .interactable   = &interactable,
        .card_handler   = &card_handler,
        .position_handler = &position_handler,
        .updateable       = &updateable
    };

    return stock;
}
