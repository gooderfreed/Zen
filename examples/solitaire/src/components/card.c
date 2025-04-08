#include "../../inc/solitaire.h"

/*
 * Arrays of card display elements
 * Used for converting card values to display text
 */
static const char *numerals[] = {
    " A", " 2", " 3", " 4", " 5", " 6",
    " 7", " 8", " 9", "10", " J", " Q", " K"
};

static const wchar_t suits[] = {
    L'♠', L'♥', L'♣', L'♦'
};

/*
 * Border characters for screen elements
 * Used for drawing borders around cards and game areas
 */
const wchar_t fat_border[8] = {
    L'═', L'║', L'╔', L'╗', L'╚', L'╝', L'╠', L'╣'
};

const wchar_t slim_border[8] = {
    L'─', L'│', L'┌', L'┐', L'└', L'┘', L'├', L'┤'
};

/*
 * Convert suit enum to display character
 * Returns Unicode character for card suit
 */
wchar_t suit_to_text(const Suit suit) {
    return suits[suit];
}

/*
 * Convert numeral enum to display text
 * Returns string representation of card value
 */
const char *numeral_to_text(const Numeral numeral) {
    return numerals[numeral - 1];
}

/*
 * Draw card on screen at specified position
 * Handles card borders, suit symbols, and numerals
 */
void print_card(Screen *screen, const Card *card, int y, int x, int size_y, int size_x) {
    if (!card) return;

    int x_0 = x;
    int y_0 = y;

    if (card->hidden) {
        fill_area(screen, y_0, x_0, size_y, size_x, L'░', ((Color){0x006e651a}), ((Color){0x006e651a}));
        add_borders(screen, y_0, x_0, size_y, size_x, ((Color){0x00383307}), COLOR_WHITE, slim_border);
        if (y != CARD_HEIGHT + 2 * BORDER_OFFSET_Y + 1 && y != BORDER_OFFSET_Y) {
            screen->pixels[y_0][x_0].symbol = slim_border[6];
            screen->pixels[y_0][x_0 + CARD_WIDTH - 1].symbol = slim_border[7];
        }
        return;
    }

    fill_area(screen, y_0, x_0, size_y, size_x, ' ', COLOR_NONE, COLOR_NONE);
    colorize_card(screen, card, y_0 + 1, x_0 + 1, CARD_HEIGHT - 2, CARD_WIDTH - 2);
    add_borders(screen, y_0, x_0, size_y, size_x, ((Color){0x00383307}), COLOR_WHITE, slim_border);

    if (y != CARD_HEIGHT + 2 * BORDER_OFFSET_Y + 1 && y != BORDER_OFFSET_Y) {
        screen->pixels[y_0][x_0].symbol = slim_border[6];
        screen->pixels[y_0][x_0 + CARD_WIDTH - 1].symbol = slim_border[7];
    }

    if (card->selected) {
        screen->pixels[y_0][x_0 + 3].symbol = L'◖';
        screen->pixels[y_0][x_0 + 4].symbol = L'◗';
    }

    wchar_t suit = suit_to_text(card->suit);
    screen->pixels[y_0 + 1][x_0 + 1].symbol = suit;
    screen->pixels[y_0 + CARD_HEIGHT / 2][x_0 + CARD_WIDTH / 2 - 1].symbol = suit;
    screen->pixels[y_0 + CARD_HEIGHT - 2][x_0 + CARD_WIDTH - 3].symbol = suit;

    const char *numeral = numeral_to_text(card->numeral);
    screen->pixels[y_0 + 1][x_0 + CARD_WIDTH - 3].symbol = numeral[0];
    screen->pixels[y_0 + 1][x_0 + CARD_WIDTH - 2].symbol = numeral[1];
    if (numeral[0] == ' ') {
        screen->pixels[y_0 + CARD_HEIGHT - 2][x_0 + 1].symbol = numeral[1];
    }
    else {
        screen->pixels[y_0 + CARD_HEIGHT - 2][x_0 + 1].symbol = numeral[0];
        screen->pixels[y_0 + CARD_HEIGHT - 2][x_0 + 2].symbol = numeral[1];
    }
}

/*
 * Apply color formatting to card area
 * Sets background color and text color based on suit
 */
void colorize_card(Screen *screen, const Card *card, int y, int x, int height, int width) {
    if (y < 0 || x < 0 || y + height > SCREEN_HEIGHT || x + width > SCREEN_WIDTH) {
        return;
    }

    Pixel pixel = (Pixel) { COLOR_WHITE, COLOR_NONE, ' ', Effect_None};
    for (int i = y; i < y + height; i++) {
        for (int j = x; j < x + width; j++) {
            if (i < SCREEN_HEIGHT && j < SCREEN_WIDTH) {
                screen->pixels[i][j] = pixel;
            }
        }
    }

    screen->pixels[y][x].foreground = (card->suit % 2 != 0) ? COLOR_RED : COLOR_BLACK;
    screen->pixels[y + CARD_HEIGHT / 2 - 1][x + CARD_WIDTH / 2 - 1 - 1].foreground  = (card->suit % 2 != 0) ? COLOR_RED : COLOR_BLACK;
    screen->pixels[y + CARD_HEIGHT - 2 - 1][x + CARD_WIDTH - 3 - 1].foreground  = (card->suit % 2 != 0) ? COLOR_RED : COLOR_BLACK;

    screen->pixels[y][x + width - 2].foreground = COLOR_BLACK;
    screen->pixels[y][x + width - 1].foreground = COLOR_BLACK;
    screen->pixels[y + height - 1][x].foreground = COLOR_BLACK;
    if (card->numeral % 10 == 0) screen->pixels[y + height - 1][x + 1].foreground = COLOR_BLACK;
}
