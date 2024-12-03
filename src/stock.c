#include "../inc/solitare.h"

Stock init_stock(void) {
    Stock stock = {0};

    static const Drawable drawable = {
        .print = print_stock
    };

    static const Interactable interactable = {
        .place_cursor = place_cursor_in_stock,
        .move = move_in_stock
    };

    stock.interfaces = (ObjectInterfaces){
        .capabilities = {
            .can_hold_cards = false,
            .is_drawable = true,
            .is_interactable = true,
            .have_buttons = false
        },
        .drawable = &drawable,
        .interactable = &interactable,
        .card_handler = NULL,
        .button_handler = NULL
    };

    return stock;
}

void print_stock(void *stock_pointer, Screen *screen, const Cursor *cursor) {
    (void)cursor;
    Stock *stock = (Stock *)stock_pointer;
    
    fill_area(screen, BORDER_OFFSET_X, STOCK_X_BASE, STOCK_AREA_HEIGHT, STOCK_AREA_WIDTH, L' ');

    for (int suit = 0; suit < CARD_SUITS; suit++) {
        int x = STOCK_X_BASE + suit * CARD_WIDTH;
        
        // Рисуем пустые места для карт
        add_borders(screen, BORDER_OFFSET_Y, x, CARD_HEIGHT, CARD_WIDTH, card_border);
        
        // Если есть карта, рисуем её
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

void move_in_stock(void *stock_pointer, Coords *coords, Coords delta) {
    (void)stock_pointer;
    
    if (delta.y != 0) return;
    
    short new_x = coords->x + delta.x;
    if (new_x < 0 || new_x >= CARD_SUITS) return;
    
    coords->x = new_x;
}

void place_cursor_in_stock(void *stock_pointer, Coords cursor_coords, Coords *target_coords) {
    (void)stock_pointer;
    
    target_coords->y = BORDER_OFFSET_Y + CARD_HEIGHT;
    target_coords->x = STOCK_CURSOR_BASE_X + CARD_WIDTH * cursor_coords.x;
}
