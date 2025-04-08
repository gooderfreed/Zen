/*
 * Deck implementation
 * Handles deck object management and card drawing
 */
#include "../../inc/solitaire.h"

// =========================
//  DECK METHODS
// =========================
/*
 * Shuffle deck array using Fisher-Yates algorithm
 * Randomly permutes all cards in the deck
 */
static void deck_shuffle(Deck *deck) {
    for (int i = DECK_SIZE - 1; i > 0; i--) {
        int j = rand() % (i + 1);
        
        Card temp = deck->deck[i];
        deck->deck[i] = deck->deck[j];
        deck->deck[j] = temp;
    }
    
    deck->pointer = &deck->deck[0];
}

/*
 * Reset deck to initial state
 * Resets all cards in the deck to default state
 */
void deck_reset(Deck *deck) {
    for (int i = 0; i < DECK_SIZE; i++) {
        deck->deck[i].object = Deck_enum;
        deck->deck[i].selected = false;
        deck->deck[i].hidden = false;
        deck->deck[i].coords = (Coords) {.x = 1, .y = 0};
    }
    deck->pointer = &deck->deck[0];
    deck_shuffle(deck);
}

/*
 * Move to next card in deck
 * Cycles through deck until finding next available card
 */
static void next_card(Deck *deck) {
    const Card *start = deck->pointer;
    if (!start) return;

    do {
        deck->pointer++;

        if (deck->pointer >= &deck->deck[DECK_SIZE]) deck->pointer = &deck->deck[0];
        if (deck->pointer == start) {
            if (deck->pointer->object != Deck_enum)
                deck->pointer = NULL;
            break;
        }
    } while (deck->pointer->object != Deck_enum);
}

/*
 * Remove current card from deck
 * Returns removed card and advances to next card
 */
static Card *draw_card(Deck *deck) {
    Card *card = deck->pointer;

    card->object = Unknown;
    next_card(deck);

    return card;
}




// =========================
//  DRAWABLE INTERFACE IMPLEMENTATION
// =========================
/*
 * Draw deck on screen
 * Shows deck pile and current top card
 */
static void print_deck(const void *deck_pointer, Screen *screen, const Cursor *cursor) {
    const Deck *deck = (const Deck *)deck_pointer;
    (void) cursor;
    fill_area(screen, BORDER_OFFSET_Y, BORDER_OFFSET_X, DECK_OFFSET - 1, 2 * CARD_WIDTH, L' ', ((Color){0x0030992e}), COLOR_WHITE);
    if (deck->pointer) {
        print_card(screen, deck->pointer, BORDER_OFFSET_Y, BORDER_OFFSET_X + CARD_WIDTH, CARD_HEIGHT, CARD_WIDTH);
        fill_area(screen, BORDER_OFFSET_Y, BORDER_OFFSET_X, CARD_HEIGHT - 1, CARD_WIDTH, L'░', ((Color){0x006e651a}), ((Color){0x006e651a}));
        add_borders(screen, BORDER_OFFSET_Y, BORDER_OFFSET_X, CARD_HEIGHT, CARD_WIDTH, ((Color){0x00383307}), COLOR_WHITE, slim_border);
    }
    else {
        add_borders(screen, BORDER_OFFSET_Y, BORDER_OFFSET_X + CARD_WIDTH, CARD_HEIGHT, CARD_WIDTH, COLOR_NONE, COLOR_WHITE, fat_border);
        add_borders(screen, BORDER_OFFSET_Y, BORDER_OFFSET_X, CARD_HEIGHT, CARD_WIDTH, COLOR_NONE, COLOR_WHITE, fat_border);
    }
}




// =========================
//  CURSOR_INTERACTABLE INTERFACE IMPLEMENTATION
// =========================
/*
 * Handle cursor movement in deck area
 * Restricts movement to horizontal only between deck and current card
 */
static void move_in_deck(const void *deck_pointer, Coords *coords, const Coords delta) {
    (void)deck_pointer;
    
    if (delta.y != 0) return;
    
    short new_x = coords->x + delta.x;
    if (new_x < 0 || new_x > 1) return;
    
    coords->x = new_x;
}

/*
 * Position cursor relative to deck elements
 * Places cursor at bottom of deck or current card
 */
static Coords place_cursor_in_deck(const void *deck_pointer, const Coords cursor_coords) {
    (void)deck_pointer;
    return (Coords) {
        .y = BORDER_OFFSET_Y + CARD_HEIGHT,
        .x = (short)(cursor_coords.x * CARD_WIDTH + (CARD_WIDTH / 2) + BORDER_OFFSET_X - 1)
    };
}

/*
 * Get default coords
 * Gets the default coords of the deck
 */
static Coords get_default_coords(const void *deck_pointer) {
    (void)deck_pointer;
    return (Coords) {.x = 0, .y = 0};
}

/*
 * Get cursor config
 * Gets the cursor config of the deck
 */
static CursorConfig get_cursor_config_in_deck(const void *deck_pointer, const Coords cursor_coords) {
    (void)deck_pointer;
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
 * Select card in deck
 * Adds selected card to container and marks it as selected
 */
static void select_card_in_deck(void *deck_pointer, const Coords cursor_coords, Container *container) {
    Deck *deck = (Deck *)deck_pointer;
    (void)cursor_coords;

    if (deck->pointer == NULL) return;

    if (container->size == 0) {
        deck->pointer->selected = true;
        container_add_element(container, deck->pointer);
        container_set_source(container, deck_pointer);
    } else {
        deck->pointer->selected = false;
        container_pop_element(container);
    }
}

/*
 * Get card from deck
 * Removes selected card from deck and advances to next card
 */
static void get_card_in_deck(void *deck_pointer, const Container *container) {
    Deck *deck = (Deck *)deck_pointer;
    (void)container;

    deck->pointer->selected = false;
    deck->pointer->object = Unknown;
    next_card(deck);
}

/*
 * Check if cursor is on the same card in deck
 * Compares cursor position to deck pointer
 */
static bool is_same_card_in_deck(const void *deck_pointer, const Coords cursor_coords, const void *card) {
    (void)cursor_coords;
    const Deck *deck = (const Deck *)deck_pointer;
    return deck->pointer == (Card *)card;
}




// =========================
//  BUTTONS IMPLEMENTATION
// =========================
/*
 * Handle next card button click
 * Advances to next card in deck and clears cursor container
 */
static void handle_next_card_button(void *deck_pointer, void *context) {
    next_card((Deck *)deck_pointer);
    Container *container = (Container *)context;
    if (container->size > 0 && ((Deck *)deck_pointer)->pointer) {
        while (!container_is_empty(container)) {
            Card *card = (Card *)container_pop_element(container);
            card->selected = false;
        }
    }
}




// =========================
//  CARD_PROVIDER INTERFACE IMPLEMENTATION
// =========================
/*
 * Peek at top card of deck
 * Returns a pointer to the top card without modifying the deck
 */
static Card *deck_peek(void *deck_pointer, Coords coords) {
    (void)coords;
    Deck *deck = (Deck *)deck_pointer;

    return deck->pointer;
}

/*
 * Remove top card from deck
 * Advances the deck pointer to the next card
 */
static void deck_pop(void *deck_pointer, Card *card) {
    (void)card;
    Deck *deck = (Deck *)deck_pointer;

    next_card(deck);
}




/*
 * Generate a deck of cards
 * Initializes deck structure and fills it with cards
 */
Deck *generate_deck(Arena *arena) {
    Deck *deck = arena_alloc(arena, sizeof(Deck));
    
    int i = 0;
    for (Suit suit = Spades; suit < CARD_SUITS; suit++) {
        for (Numeral numeral = Ace; numeral <= CARD_NUMERALS; numeral++) {
            Card card = {
                .suit = suit,
                .numeral = numeral,
                .selected = false,
                .object = Deck_enum,
                .coords = {.x = 1, .y = 0}
            };
            deck->deck[i++] = card;
        }
    }
    deck->pointer = &deck->deck[0];

    INTERFACES(arena, deck, {
        DRAWABLE(print_deck);
        CURSOR_INTERACTABLE(place_cursor_in_deck, move_in_deck, get_default_coords, get_cursor_config_in_deck);
        BUTTONS_HANDLER({
            BUTTONS_GROUP({
                BUTTON(handle_next_card_button);
            });
        });
        CARD_HANDLER({
            CAN_GIVE_CARDS(select_card_in_deck, is_same_card_in_deck, get_card_in_deck);
        });
    });


    static CardProvider card_provider = (CardProvider) {
        .peek = deck_peek,
        .pop  = deck_pop,
    };  

    static DeckMethods deck_methods = (DeckMethods) {
        .card_provider = &card_provider,
        .draw_card = draw_card,
    };

    deck->export_methods = &deck_methods;

    deck_shuffle(deck);
    return deck;
}
