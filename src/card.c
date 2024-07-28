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
