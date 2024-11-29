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
    Deck deck;

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
    deck.pointer = &deck.deck;

    // deck.interface.select_cards = select_card_in_deck;
    // deck.interface.get_card = get_card_in_deck;
    // deck.interface.move

    // deck.interface.place_cursor = place_cursor_in_deck;

    deck.interface.is_drawable  = true;
    deck.interface.print = print_deck;

    return deck;
}

void next_card(Deck *deck) {
    Card *start = deck->pointer;

    do {
        deck->pointer++;

        if (deck->pointer > &deck->deck[DECK_SIZE]) deck->pointer = NULL;
        if (deck->pointer == start) break;

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
    // else add_borders(screen, BORDER_OFFSET_Y, BORDER_OFFSET_X + CARD_WIDTH, CARD_HEIGHT, CARD_WIDTH, card_border);
    // print_card_deck(screen, deck->pointer);
}

// void place_cursor_in_deck(const Cursor *cursor, Coords *coords) {
//     coords->y = BORDER_OFFSET_Y + CARD_HEIGHT;
//     coords->x = cursor->coords.x * CARD_WIDTH + (CARD_WIDTH / 2) + BORDER_OFFSET_X - 1;
// }

// Card *select_card_in_deck(void *deck_pointer, Coords cursor_coords) {
//     (void) cursor_coords;
//     Deck *deck = (Deck *)deck_pointer;
//     deck->pointer->selected = !deck->pointer->selected;
    
//     return deck->pointer;
// }

// Card *get_card_in_deck(void *deck_pointer, Cursor *cursor) {
//     (void) cursor;
//     Deck *deck = (Deck *)deck_pointer;

//     deck->pointer->selected = false;
//     Card *card = draw_card(deck);

//     return card;
// }
