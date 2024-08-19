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
    deck.pointer = &deck.deck[0];

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

Card *draw_card(Deck *deck) {
    Card *card = deck->pointer;

    card->object = Unknown;
    next_card(deck);

    return card;
}
