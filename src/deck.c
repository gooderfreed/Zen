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

#include "../inc/solitare.h"

Deck generate_deck(void) {
    Deck deck = {0};
    
    int i = 0;
    for (Suit suit = Spades; suit < CARD_SUITS; suit++) {
        for (Numeral numeral = Ace; numeral <= CARD_NUMERALS; numeral++) {
            Card card = {
                .suit = suit,
                .numeral = numeral,
                .selected = false,
                .object = Deck_enum
            };
            deck.deck[i++] = card;
        }
    }
    deck.pointer = &deck.deck[0];

    static const Drawable drawable = {
        .print = print_deck
    };

    static const Interactable interactable = {
        .place_cursor = place_cursor_in_deck,
        .move = move_in_deck
    };

    static const CardHandler card_handler = {
        .can_give_cards = true,
        .can_take_cards = false,
        .select_cards = select_card_in_deck,
        .get_cards = get_card_in_deck,
        .place_cards = NULL,
        .can_place = NULL
    };

    static const ButtonHandler button_handler = {
    .is_button_position = is_deck_button_position,
        .handle_button = handle_deck_button
    };

    deck.interfaces = (ObjectInterfaces){
        .capabilities = {
            .can_hold_cards = true,
            .is_drawable = true,
            .is_interactable = true,
            .have_buttons = true
        },
        .drawable = &drawable,
        .interactable = &interactable,
        .card_handler = &card_handler,
        .button_handler = &button_handler
    };

    return deck;
}

void next_card(Deck *deck) {
    Card *start = deck->pointer;

    do {
        deck->pointer++;

        if (deck->pointer >= &deck->deck[DECK_SIZE]) deck->pointer = &deck->deck[0];
        if (deck->pointer == start) {
            deck->pointer = NULL;
            break;
        }
    } while (deck->pointer->object != Deck_enum);
}

Card *draw_card(Deck *deck) {
    Card *card = deck->pointer;

    card->object = Unknown;
    next_card(deck);

    return card;
}

void print_deck(void *deck_pointer, Screen *screen, const Cursor *cursor) {
    (void) cursor;
    Deck *deck = (Deck *)deck_pointer;
    fill_area(screen, BORDER_OFFSET_Y, BORDER_OFFSET_X, DECK_OFFSET - 2, 2 * CARD_WIDTH, L' ');
    fill_area(screen, BORDER_OFFSET_Y, BORDER_OFFSET_X, CARD_HEIGHT - 1, CARD_WIDTH, L'░');
    add_borders(screen, BORDER_OFFSET_Y, BORDER_OFFSET_X, CARD_HEIGHT, CARD_WIDTH, card_border);
    
    if (deck->pointer) 
        print_card(screen, deck->pointer, BORDER_OFFSET_Y, BORDER_OFFSET_X + CARD_WIDTH, CARD_HEIGHT, CARD_WIDTH);
    else add_borders(screen, BORDER_OFFSET_Y, BORDER_OFFSET_X + CARD_WIDTH, CARD_HEIGHT, CARD_WIDTH, card_border);
}

void move_in_deck(void *deck_pointer, Coords *coords, Coords delta) {
    (void)deck_pointer;
    
    if (delta.y != 0) return;
    
    short new_x = coords->x + delta.x;
    if (new_x < 0 || new_x > 1) return;
    
    coords->x = new_x;
}

void place_cursor_in_deck(void *deck_pointer, Coords cursor_coords, Coords *target_coords) {
    (void)deck_pointer;
    
    target_coords->y = BORDER_OFFSET_Y + CARD_HEIGHT;
    target_coords->x = cursor_coords.x * CARD_WIDTH + (CARD_WIDTH / 2) + BORDER_OFFSET_X - 1;
}

void next_card_action(void *deck_pointer) {
    Deck *deck = (Deck *)deck_pointer;
    next_card(deck);
}

bool is_deck_button_position(void *deck_pointer, Coords coords) {
    (void)deck_pointer;
    return coords.x == 0 && coords.y == 0;
}

void handle_deck_button(void *deck_pointer, Coords coords) {
    if (coords.x == 0 && coords.y == 0) {
        next_card_action(deck_pointer);
    }
}

void select_card_in_deck(void *deck_pointer, Coords cursor_coords, CardsContainer *container) {
    Deck *deck = (Deck *)deck_pointer;
    (void)cursor_coords;
    
    if (container->size == 0) {
        deck->pointer->selected = true;
        container->container[container->size++] = deck->pointer;
        container->source = Deck_enum;
    } else {
        deck->pointer->selected = false;
        container->size = 0;
        container->source = Unknown;
    }
}

void get_card_in_deck(void *deck_pointer, CardsContainer *container) {
    Deck *deck = (Deck *)deck_pointer;
    (void)container;

    deck->pointer->selected = false;
    deck->pointer->object = Unknown;
    next_card(deck);
}
