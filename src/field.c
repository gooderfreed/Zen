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

Field init_field(Deck *deck) {
    Field field = {
        .interface = {
            .is_drawable = true,

            .get_cards = get_cards_in_field,
            .can_place = can_place_in_field,
            .place_cards = place_cards_in_field,
            .select_cards = select_cards_in_field,

            .place_cursor = place_cursor_in_field,
            .move = move_in_field,

            .print = print_field
        }
    };

    for (short row = 0; row < FIELD_WIDTH; row++) {
        for (short col = 0; col < FIELD_HEIGHT; col++) {
            if (col > row) field.field[col][row] = NULL;
            else {
                Card *card = draw_card(deck);
                card->coords.x = row;
                card->coords.y = col;
                card->object = Field_enum;
                field.field[col][row] = card;
            }
        }
    }

    return field;
}

void print_field(void *field_pointer, Screen *screen, const Cursor *hovered_card) {
    Field *field = (Field *)field_pointer;
    int contentHeight = 2 * (FIELD_HEIGHT - 1) + CARD_HEIGHT - 2;
    int contentWidth = SCREEN_WIDTH - BORDER_OFFSET_X - 1;

    fill_area(screen, DECK_OFFSET + BORDER_OFFSET_Y, BORDER_OFFSET_X, contentHeight, contentWidth, ' ');

    int y_offset_base = DECK_OFFSET + BORDER_OFFSET_Y;
    int hovered_x = hovered_card->coords.x;
    int hovered_y = hovered_card->coords.y;

    for (int y = 0; y < FIELD_HEIGHT; ++y) {
        int y_0 = y * CARD_COVERED_HEIGHT + y_offset_base;
        int is_hovered_row = y > hovered_y;

        for (int x = 0; x < FIELD_WIDTH; ++x) {
            int additional_offset = (is_hovered_row && x == hovered_x && hovered_card->subject == Field_enum);
            int x_0 = x * CARD_WIDTH + BORDER_OFFSET_X;

            Card *current_card = field->field[y][x];

            if (y == 0 && !current_card) {
                add_borders(screen, y_offset_base, x_0, CARD_HEIGHT, CARD_WIDTH, fat_border);
            }
            else {
                print_card(screen, current_card, y_0 + additional_offset, x_0, CARD_HEIGHT, CARD_WIDTH);
            }
        }
    }
}

int get_last_card_y(const Field *field, int x) {
    for (int i = FIELD_HEIGHT - 1; i >= 0; --i)
        if (field->field[i][x]) return i;
    return 0;
}

void place_cursor_in_field(void *field_pointer, Coords cursor_coords, Coords *target_coords) {
    Field *field = (Field *)field_pointer;
    int part = BORDER_OFFSET_Y + 1 + cursor_coords.y * 2;
    bool is_last_card = FIELD_HEIGHT == cursor_coords.y || !field->field[cursor_coords.y + 1][cursor_coords.x];

    target_coords->y += part + (is_last_card ? CARD_HEIGHT : CARD_COVERED_HEIGHT + 1);
    target_coords->x += BORDER_OFFSET_X - 1;
}

void move_in_field(void *field_pointer, Coords *coords, Coords delta) {
    short new_x = coords->x + delta.x;
    short new_y = coords->y + delta.y;
    Field *field = (Field *)field_pointer;

    coords->x = (new_x + FIELD_WIDTH) % FIELD_WIDTH;
    if (new_y >= 0 && new_y < FIELD_HEIGHT && field->field[new_y][coords->x]) coords->y = new_y;
    if (delta.y == 0) coords->y = (short)get_last_card_y(field, coords->x);
}

void get_cards_in_field(void *field_pointer, CardsContainer *container) {
    Field *field = (Field *)field_pointer;
    Card *target_card = container->container[0];

    for (int i = target_card->coords.y; field->field[i][target_card->coords.x] && i < FIELD_HEIGHT; ++i) {
        field->field[i][target_card->coords.x]->selected = false;
        field->field[i][target_card->coords.x] = NULL;
    }
}

void select_cards_in_field(void *field_pointer, Coords cursor_coords, CardsContainer *container) {
    Field *field = (Field *)field_pointer;
    bool select_mode = (container->size == 0);

    for (int i = cursor_coords.y; field->field[i][cursor_coords.x] && i < FIELD_HEIGHT; ++i) {
        Card *current_card = field->field[i][cursor_coords.x];
        current_card->selected = select_mode;

        if (select_mode) container->container[container->size++] = current_card;
        else container->container[container->size++] = NULL;
    }
    container->source = Field_enum;

    if (!select_mode) {
        container->size = 0;
        container->source = Unknown;
    }
}

bool can_place_in_field(void *field_pointer, Coords cursor_coords, CardsContainer *container) {
    Field *field = (Field *)field_pointer;

    return (cursor_coords.y + container->size < FIELD_HEIGHT && 
            field->field[cursor_coords.y + 1][cursor_coords.x] == NULL && 
            (field->field[cursor_coords.y][cursor_coords.x] == NULL || 
            !field->field[cursor_coords.y][cursor_coords.x]->selected));
}

void place_cards_in_field(void *field_pointer, Coords cursor_coord, CardsContainer *container) {
    Field *field = (Field *)field_pointer;
    bool y_offset = (field->field[cursor_coord.y][cursor_coord.x]);

    for (int i = 0; i < container->size; ++i) {
        Card *card = container->container[i];
        card->object = Field_enum;
        card->coords.x = cursor_coord.x;
        card->coords.y = (short)(cursor_coord.y + y_offset + i);
        field->field[cursor_coord.y + y_offset + i][cursor_coord.x] = card;
        container->container[i] = NULL;
    }
    container->size = 0;
    container->source = Unknown;
}
