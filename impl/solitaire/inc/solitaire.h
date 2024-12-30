#ifndef SOLITARE_H
#define SOLITARE_H

#define _POSIX_C_SOURCE 199309L

#include "config.h"
#define CARD_STRUCT
#include "../../../core/inc/core.h"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
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
    bool selected : 1;  // Selection state
    bool hidden   : 1;  // Hidden state
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
    ObjectInterfaces interfaces;               // Core engine interfaces
    Card *stock[CARD_SUITS][CARD_NUMERALS];    // Foundation piles for each suit
    Card *top_cards[CARD_SUITS];               // Top card pointers for each suit
} Stock;


/*
 * Field structure
 * Represents the tableau of cards in Solitaire
 */
typedef struct Field {
    ObjectInterfaces interfaces;                   // Core engine interfaces
    Card *field[FIELD_HEIGHT][FIELD_WIDTH];        // Tableau columns for card placement
} Field;

/*
 * Menu structure
 * Represents the menu of the game
 */
typedef struct Menu {
    ObjectInterfaces interfaces;     // Core engine interfaces
    bool start_game : 1;             // Start game flag
} Menu;

typedef struct Controls {
    ObjectInterfaces interfaces;     // Core engine interfaces
} Controls;

/*
 * Win screen structure
 * Represents the win screen of the game
 */
typedef struct WinScreen {
    ObjectInterfaces interfaces;    // Core engine interfaces
} WinScreen;

/*
 * Game structure
 * Contains all game components
 */
typedef struct Game
{
    Deck *deck;
    Field *field;
    Stock *stock;
} Game;

/*
 * Stock context structure 
 * Contains pointers to deck and field for auto-update
 */
typedef struct StockContext {
    Deck *deck;                   // Deck pointer
    Field *field;                 // Field pointer
    Container *cursor_container;  // Cursor container
    Core *core;                   // Core pointer
} StockContext;

/*
 * New game structure
 * Contains pointers to core and game for menu interaction
 */
typedef struct NewGame {
    Game *game;
    Core *core;
} NewGame;



//field
Field init_field(void);
int get_last_card_y(const Field *field, int x);
void prepare_field(Field *field, Deck *deck);

//stock
Stock init_stock(void);
void stock_reset(Stock *stock);

//deck
Deck generate_deck(void);
void next_card(Deck *deck);
Card *draw_card(Deck *deck);
bool have_hidden_cards(const Deck *deck);
void deck_reset(Deck *deck);

//card
extern const wchar_t fat_border[8];
extern const wchar_t card_border[8];
wchar_t suit_to_text(const Suit suit);
const char *numeral_to_text(const Numeral numeral);
void print_card(Screen *screen, const Card *card, int y, int x, int size_y, int size_x);
void colorize_card(Screen *screen, const Card *card, int y, int x, int height, int width);

//menu
MapLayer menu_layer_init(Core *core, Game *game);
void prepare_menu_screen(Screen *screen);
Controls init_controls(void);

//winscreen
MapLayer win_layer_init(Core *core, Game *game);

//game
MapLayer game_layer_init(Core *core, Container *container);
void game_reset(Game *game);

#endif
