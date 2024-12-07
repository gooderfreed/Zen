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

/*
 * Solitaire game implementation
 * Extends core engine with specific card game logic
 */

// Game specific types
// ------------------

/*
 * Card suits enumeration
 * Classic playing card suits
 */
typedef enum {
    Spades,
    Hearts,
    Clubs,
    Diamonds,
    CARD_SUITS
} Suit;

/*
 * Card numerals enumeration
 * Standard playing card values from Ace to King
 */
typedef enum {
    Ace = 1,
    Two, Three, Four, Five, Six, Seven, Eight, Nine,
    Ten, Jack, Queen, King,
} Numeral;

/*
 * Game objects enumeration
 * Defines all possible object types in solitaire
 */
typedef enum {
    Deck_enum = 0,
    Field_enum,
    Stock_enum,
    OBJECTS_COUNT,
    Unknown
} Objects;

/*
 * Extended Card structure
 * Adds suit and numeral to base Card
 */
typedef struct Card {
    Coords coords;      // Card position on screen
    Numeral numeral;    // Card value (Ace through King)
    Suit suit;          // Card suit (Spades, Hearts, etc)
    Objects object;     // Owner object type
    bool selected;      // Selection state
} Card;

/*
 * Deck structure
 * Represents the deck of cards in Solitaire
 */
typedef struct Deck {
    ObjectInterfaces interfaces;  // Core engine interfaces
    Card deck[DECK_SIZE + 1];     // Array of all cards (+1 for NULL terminator)
    Card *pointer;                // Current top card pointer
} Deck;

/*
 * Stock structure
 * Represents the stock of cards in Solitaire
 */
typedef struct Stock {
    ObjectInterfaces interfaces;                     // Core engine interfaces
    Card *stock[CARD_SUITS][CARD_NUMERALS];          // Foundation piles for each suit
} Stock;

/*
 * Field structure
 * Represents the tableau of cards in Solitaire
 */
typedef struct Field {
    ObjectInterfaces interfaces;                   // Core engine interfaces
    Card *field[FIELD_HEIGHT][FIELD_WIDTH];        // Tableau columns for card placement
} Field;

//field
Field init_field(Deck *deck);

//stock
Stock init_stock(void);

//deck
Deck generate_deck(void);
void next_card(Deck *deck);
Card *draw_card(Deck *deck);

//card
extern wchar_t fat_border[8];
extern wchar_t card_border[8];
wchar_t suit_to_text(Suit suit);
char *numeral_to_text(Numeral numeral);
void print_card(Screen *screen, const Card *card, int y, int x, int size_y, int size_x);
void colorize_card(Screen *screen, const Card *card, int y, int x, int height, int width);

#endif
