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

// deck
Deck generate_deck(void) {
    Deck deck;

    int i = 1;
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
    Card card = { .suit = 0, .numeral = Null, .selected = false, .object = Unknown };
    deck.deck[0] = card;
    deck.pointer = &deck.deck[1];

    return deck;
}

void next_card(Deck *deck) {
    Card *start = deck->pointer;

    do {
        deck->pointer++;

        if (deck->pointer > &deck->deck[DECK_SIZE]) deck->pointer = &deck->deck[1];
        if (deck->pointer == start) break;

    } while (deck->pointer->object != Deck_enum);
}

Card* draw_card(Deck *deck) {
    Card *card = deck->pointer;

    card->object = Unknown;
    next_card(deck);

    return card;
}
// deck

// card
char *numerals[] = {
    " A", " 2", " 3", " 4", " 5", " 6",
    " 7", " 8", " 9", "10", " J", " Q", " K"
};

wchar_t suits[] = {
    L'♠', L'♥', L'♣', L'♦'
};

wchar_t fat_border[8] = {
    L'═', L'║', L'╔', L'╗', L'╚', L'╝', L'╠', L'╣'
};

wchar_t card_border[8] = {
    L'─', L'│', L'┌', L'┐', L'└', L'┘', L'├', L'┤'
};

wchar_t suit_to_text(Suit suit) {
    return suits[suit];
}

char *numeral_to_text(Numeral numeral) {
    return numerals[numeral - 1];
}

void print_card(Screen *screen, const Card *card, int y, int x, int size_y, int size_x) {
    if (card->numeral == Null) return;

    int x_0 = x;
    int y_0 = y;

    fill_area(screen, y_0, x_0, size_y, size_x, ' ');
    colorize_area(screen, card, y_0 + 1, x_0 + 1, CARD_HEIGHT - 2, CARD_WIDTH - 2);
    add_borders(screen, y_0, x_0, size_y, size_x, card_border);

    if (y != 10) {
        screen->data[y_0][x_0] = card_border[6];
        screen->data[y_0][x_0 + CARD_WIDTH - 1] = card_border[7];
    }

    if (card->selected) {
        screen->data[y_0][x_0 + 3] = L'◖';
        screen->data[y_0][x_0 + 4] = L'◗';
    }

    wchar_t suit = suit_to_text(card->suit);
    screen->data[y_0 + 1][x_0 + 1] = suit;
    screen->data[y_0 + CARD_HEIGHT / 2][x_0 + CARD_WIDTH / 2 - 1] = suit;
    screen->data[y_0 + CARD_HEIGHT - 2][x_0 + CARD_WIDTH - 3] = suit;

    char *numeral = numeral_to_text(card->numeral);
    screen->data[y_0 + 1][x_0 + CARD_WIDTH - 3] = numeral[0];
    screen->data[y_0 + 1][x_0 + CARD_WIDTH - 2] = numeral[1];
    if (numeral[0] == ' ') {
        screen->data[y_0 + CARD_HEIGHT - 2][x_0 + 1] = numeral[1];
    }
    else {
        screen->data[y_0 + CARD_HEIGHT - 2][x_0 + 1] = numeral[0];
        screen->data[y_0 + CARD_HEIGHT - 2][x_0 + 2] = numeral[1];
    }
}
// card

// field
void init_field(Field field, Deck *deck) {
    for (int col = 0; col < FIELD_WIDTH; col++) {
        for (int row = 0; row <= col && row < FIELD_HEIGHT; row++) {
            Card *card = draw_card(deck);
            card->coords.x = row;
            card->coords.y = col;
            card->object = Field_enum;
            field[row][col] = card;
        }
        for (int row = col + 1; row < FIELD_HEIGHT; row++) {
            field[row][col] = &deck->deck[0];
        }
    }
}

void print_field(Screen *screen, const Field *field) {
    int contentHeight = SCREEN_HEIGHT - 2 * DECK_OFFSET;
    int contentWidth = SCREEN_WIDTH - BORDER_OFFSET_X - 1;

    fill_area(screen, BORDER_OFFSET_Y + DECK_OFFSET, BORDER_OFFSET_X, contentHeight, contentWidth, ' ');
    for (int x = 0; x < FIELD_WIDTH; ++x) {
        int x_0 = x * CARD_WIDTH + BORDER_OFFSET_Y;
        for (int y = 0; y < FIELD_HEIGHT; ++y) {
            int y_0 = y * 2 + DECK_OFFSET + BORDER_OFFSET_Y;
            print_card(screen, (*field)[y][x], y_0, x_0, CARD_HEIGHT, CARD_WIDTH);
        }
    }
}
// field


int main(void) {
    setlocale(LC_ALL, "");
    clear();

    Deck deck = generate_deck();
    Field field;
    Screen screen = init_screen();

    init_field(field, &deck);
    add_borders(&screen, 0, 0, SCREEN_HEIGHT, SCREEN_WIDTH, fat_border);
    add_separator(&screen, DECK_OFFSET + BORDER_OFFSET_Y - 1, 0, fat_border);

    // print_card(&screen, field[0][0], 0, 0, CARD_HEIGHT, CARD_WIDTH);
    print_field(&screen, &field);
    print_screen(&screen);

    // wprintf(L"Selected card: %i\n", field[1][1]->numeral);

    // for (int i = 1; i <= DECK_SIZE; i++) {
    //     wprintf(L"%i Selected card: %s of %lc\n", i, numeral_to_text(deck.deck[i].numeral), suit_to_text(deck.deck[i].suit));
    // }
}

