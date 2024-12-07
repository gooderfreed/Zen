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
    
    fill_area(screen, BORDER_OFFSET_X, STOCK_X_BASE, STOCK_AREA_HEIGHT, STOCK_AREA_WIDTH, L' ');

    for (int suit = 0; suit < CARD_SUITS; suit++) {
        int x = STOCK_X_BASE + suit * CARD_WIDTH;
        
        add_borders(screen, BORDER_OFFSET_Y, x, CARD_HEIGHT, CARD_WIDTH, card_border);
        
        Card *top_card = NULL;
        for (int i = CARD_NUMERALS - 1; i >= 0; i--) {
            if (stock->stock[suit][i]) {
                top_card = stock->stock[suit][i];
                break;
            }
        }
        if (top_card) {
            print_card(screen, top_card, BORDER_OFFSET_Y, x, CARD_HEIGHT, CARD_WIDTH);
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

    stock.interfaces = (ObjectInterfaces) {
        .name           = "Stock",
        .capabilities = {
            .is_drawable     = true,
            .is_interactable = true,
            .can_hold_cards  = false,
            .have_buttons    = false
        },
        .drawable       = &drawable,
        .interactable   = &interactable,
    };

    return stock;
}
