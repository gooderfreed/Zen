#ifndef SOLITARE_H
#define SOLITARE_H

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

typedef struct Card Card;


/*
 * Card Provider structure
 * This structure is used to provide cards for the game.
 */
typedef struct CardProvider {
    Card *(*peek) (void *source, Coords coords); // Function to peek at the top card
    void  (*pop)  (void *source, Card *card);    // Function to pop the top card
} CardProvider;




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
struct Card {
    Coords coords;      // Card position on screen
    Numeral numeral;    // Card value (Ace through King)
    Suit suit;          // Card suit (Spades, Hearts, etc)
    Objects object;     // Owner object type
    bool selected : 1;  // Selection state
    bool hidden   : 1;  // Hidden state
};

extern const wchar_t fat_border[8];
extern const wchar_t slim_border[8];
wchar_t suit_to_text(const Suit suit);
const char *numeral_to_text(const Numeral numeral);
void print_card(Screen *screen, const Card *card, int y, int x, int size_y, int size_x);
void colorize_card(Screen *screen, const Card *card, int y, int x, int height, int width);




/*
 * Deck structure
 * Represents the deck of cards in Solitaire
 */
typedef struct Deck {
    ObjectInterfaces interfaces;  // Core engine interfaces
    Card deck[DECK_SIZE + 1];     // Array of all cards (+1 for NULL terminator)
    Card *pointer;                // Current top card pointer
    void *export_methods;         // Deck export methods
} Deck;

typedef struct DeckMethods {
    CardProvider *card_provider;
    Card *(*draw_card)(Deck *deck);
} DeckMethods;

Deck *generate_deck(Arena *arena);
void deck_reset(Deck *deck);




/*
 * Stock structure
 * Represents the stock of cards in Solitaire
 */
typedef struct Stock {
    ObjectInterfaces interfaces;               // Core engine interfaces
    Card *stock[CARD_SUITS][CARD_NUMERALS];    // Foundation piles for each suit
    Card *top_cards[CARD_SUITS];               // Top card pointers for each suit
} Stock;

Stock *init_stock(Arena *arena);
void stock_reset(Stock *stock);




/*
 * Field structure
 * Represents the tableau of cards in Solitaire
 */
typedef struct Field {
    ObjectInterfaces interfaces;                // Core engine interfaces
    Card *field[FIELD_HEIGHT][FIELD_WIDTH];     // Tableau columns for card placement
    void *export_methods;                       // Field export methods
} Field;

typedef struct FieldMethods {
    CardProvider *card_provider;
} FieldMethods;

Field *init_field(Arena *arena);
void field_reset(Field *field, Deck *deck);




/*
 * Stock context structure 
 * Contains pointers to deck and field for auto-update
 */
typedef struct StockContext {
    Deck *deck;               // Deck pointer
    Field *field;             // Field pointer
    Cursor *cursor;           // Cursor pointer
} StockContext;




/*
 * Game structure
 * Contains all game components
 */
typedef struct Game {
    Deck *deck;               // Deck pointer
    Field *field;             // Field pointer
    Stock *stock;             // Stock pointer
} Game;

void game_reset(Game *game);




/*
 * Menu structure
 * Represents the menu of the game
 */
typedef struct Menu {
    ObjectInterfaces interfaces;     // Core engine interfaces
    bool start_game : 1;             // Start game flag
} Menu;

Menu *init_menu(Arena *arena);




/*
 * Controls structure
 * Represents the controls screen of the game
 */
typedef struct Controls {
    ObjectInterfaces interfaces;     // Core engine interfaces
} Controls;

Controls *init_controls(Arena *arena);




/*
 * Win screen structure
 * Represents the win screen of the game
 */
typedef struct WinScreen {
    ObjectInterfaces interfaces;    // Core engine interfaces
} WinScreen;

WinScreen *init_win_screen(Arena *arena);




// Layers
MapLayer *game_layer_init(Arena *arena, Cursor *cursor);
MapLayer *menu_layer_init(Arena *arena, Game *game);
MapLayer *win_layer_init(Arena *arena, Game *game);

#endif
