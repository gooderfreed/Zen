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

#ifndef SOLITARE_H
#define SOLITARE_H

#include "config.h"
#define CARD_STRUCT
#include "../../../core/inc/core.h"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <locale.h>
#include <termios.h>
#include <unistd.h>
#include <string.h>

typedef enum {
    Spades,
    Hearts,
    Clubs,
    Diamonds,
    CARD_SUITS
} Suit;

typedef enum {
    Ace = 1,
    Two, Three, Four, Five, Six, Seven, Eight, Nine,
    Ten, Jack, Queen, King,
} Numeral;

typedef enum {
    Deck_enum = 0,
    Field_enum,
    Stock_enum,
    OBJECTS_COUNT,
    Unknown
} Objects;

typedef struct Card {
    Coords coords;
    Numeral numeral;
    Suit suit;
    Objects object;
    bool selected;
} Card;

typedef struct Deck {
    ObjectInterfaces interfaces;
    Card deck[DECK_SIZE + 1];
    Card *pointer;
} Deck;

typedef struct Stock {
    ObjectInterfaces interfaces;
    Card *stock[CARD_SUITS][CARD_NUMERALS];
} Stock;

typedef struct Field {
    ObjectInterfaces interfaces;
    Card *field[FIELD_HEIGHT][FIELD_WIDTH];
} Field;

//deck
Deck generate_deck(void);
void next_card(Deck *deck);
void reset_deck(Deck *deck);
Card *draw_card(Deck *deck);
void shuffle_deck(Deck *deck);
void print_deck(void *deck_pointer, Screen *screen, const Cursor *cursor);
void move_in_deck(void *deck_pointer, Coords *coords, Coords delta);
void place_cursor_in_deck(void *deck_pointer, Coords cursor_coords, Coords *target_coords);
bool is_deck_button_position(void *deck_pointer, Coords coords);
void handle_deck_button(void *deck_pointer, Coords coords);
void select_card_in_deck(void *deck_pointer, Coords cursor_coords, CardsContainer *container);
void get_card_in_deck(void *deck_pointer, CardsContainer *container);
bool is_same_card_in_deck(void *deck_pointer, Coords cursor_coords, Card *card);
//deck

//field
Field init_field(Deck *deck);
void select_column(Field *field, Coords coords);
void print_cursor_in_field(const Cursor *cursor, Coords *coords);
void print_field(void *field_pointer, Screen *screen, const Cursor *hovered_card);
void move_in_field(void *field_pointer, Coords *coords, Coords delta);
void place_cursor_in_field(void *field_pointer, Coords cursor_coords, Coords *target_coords);
void get_cards_in_field(void *field_pointer, CardsContainer *container);
bool can_place_in_field(void *field_pointer, Coords cursor_coords, CardsContainer *container);
void place_cards_in_field(void *field_pointer, Coords cursor_coord, CardsContainer *container);
void select_cards_in_field(void *field_pointer, Coords cursor_coords, CardsContainer *container);
bool is_same_card_in_field(void *field_pointer, Coords cursor_coords, Card *card);
//field

//card
extern wchar_t fat_border[8];
extern wchar_t card_border[8];
wchar_t suit_to_text(Suit suit);
char *numeral_to_text(Numeral numeral);
void print_card(Screen *screen, const Card *card, int y, int x, int size_y, int size_x);
void colorize_card(Screen *screen, const Card *card, int y, int x, int height, int width);
//card

//stock
Stock init_stock(void);
void print_stock(void *stock_pointer, Screen *screen, const Cursor *cursor);
void move_in_stock(void *stock_pointer, Coords *coords, Coords delta);
void place_cursor_in_stock(void *stock_pointer, Coords cursor_coords, Coords *target_coords);
//stock 

#endif
