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

wchar_t suit_to_text(Suit suit) {
    return suits[suit];
}

char *numeral_to_text(Numeral numeral) {
    return numerals[numeral - 1];
}
// card


int main(void) {
    setlocale(LC_ALL, "");
    // clear();

    Deck deck = generate_deck();
    wprintf(L"Selected card: %s of %lc\n", numeral_to_text(deck.pointer->numeral), suit_to_text(deck.deck->suit));

    next_card(&deck);
    wprintf(L"Selected card: %s of %lc\n", numeral_to_text(deck.pointer->numeral), suit_to_text(deck.deck->suit));
    
    // for (int i = 1; i <= DECK_SIZE; i++) {
    //     wprintf(L"%i Selected card: %s of %lc\n", i, numeral_to_text(deck.deck[i].numeral), suit_to_text(deck.deck[i].suit));
    // }
}

