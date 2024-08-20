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
    Field field;
    for (int row = 0; row < FIELD_WIDTH; row++) {
        for (int col = 0; col < FIELD_HEIGHT; col++) {
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
    field.interface.get_card = get_card_in_field;
    field.interface.can_place = can_place_in_field;
    field.interface.place_card = place_card_in_filed;
    field.interface.select_card = select_card_in_field;
    return field;
}

void print_field(Screen *screen, const Field *field, const Cursor *hovered_card) {
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

void print_cursor_in_field(const Cursor *cursor, Coords *coords) {
    Field *field = (Field *)(cursor->objects[Field_enum]);
    int part = BORDER_OFFSET_Y + 1 + cursor->coords.y * 2;
    bool is_last_card = FIELD_HEIGHT == cursor->coords.y || !field->field[cursor->coords.y + 1][cursor->coords.x];
    wprintf(L"%i, %i\n", FIELD_HEIGHT == cursor->coords.y, !field->field[cursor->coords.y + 1][cursor->coords.x]);

    coords->y += part + (is_last_card ? CARD_HEIGHT : CARD_COVERED_HEIGHT + 1);
    coords->x += BORDER_OFFSET_X - 1;
}

void move_in_field(Coords *res_coords, const Cursor *cursor, int delta_x, int delta_y) {
    int new_x = cursor->coords.x + delta_x;
    int new_y = cursor->coords.y + delta_y;
    Field *field = (Field *)(cursor->objects[Field_enum]);

    if (new_x >= 0 && new_x < FIELD_WIDTH) res_coords->x = new_x;
    else if (new_x < 0) res_coords->x = FIELD_WIDTH - 1;
    else if (new_x >= FIELD_WIDTH || new_x == 0) res_coords->x = 0;
    if (new_y >= 0 && new_y < FIELD_HEIGHT && field->field[new_y][res_coords->x]) res_coords->y = new_y;
    if (delta_y == 0) res_coords->y = get_last_card_y(field, res_coords->x);
}

void select_column(Field *field, Coords coords) {
    for (int i = coords.y; field->field[i][coords.x] && i < FIELD_HEIGHT; ++i) {
        field->field[i][coords.x]->selected = !field->field[i][coords.x]->selected;
    }
}

bool is_column(Field *field, Card *card) {
    if (card->coords.y + 1 < FIELD_HEIGHT &&
        field->field[card->coords.y + 1][card->coords.x]) 
        return true;
    return false;
} 

Card *select_card_in_field(void *field_pointer, Coords cursor_coords) {
    Field *field = (Field *)field_pointer;

    select_column(field, cursor_coords);
    return (field->field[cursor_coords.y][cursor_coords.x]);
}


Card *get_card_in_field(void *field_pointer, Cursor *cursor) {
    Field *field = (Field *)field_pointer;

    if (is_column(field, cursor->card) && cursor->subject != Field_enum) return NULL;

    Card *card = cursor->card;
    field->field[card->coords.y][card->coords.x] = NULL;
    return card;
}

int get_column_size(Field *field, Card *card) {
    int column_offset = 0;
    for (; card->coords.y + column_offset + 1 < FIELD_HEIGHT; column_offset++) {
        if (!field->field[card->coords.y + column_offset + 1][card->coords.x]) break;
    }
    return column_offset + 1;
}

bool can_place_in_field(void *field_pointer, Card *card, Coords cursor_coords) {
    Field *field = (Field *)field_pointer;

    int size = card->object == Field_enum && is_column(field, card) ? get_column_size(field, card) : 1;
    return (cursor_coords.y + size < FIELD_HEIGHT && 
            field->field[cursor_coords.y + 1][cursor_coords.x] == NULL && 
            (field->field[cursor_coords.y][cursor_coords.x] == NULL || 
             !field->field[cursor_coords.y][cursor_coords.x]->selected));
}

void place_card_in_filed(void *field_pointer, Card *card, Coords cursor_coord) {
    Field *field = (Field *)field_pointer;
    bool y_offset = (field->field[cursor_coord.y][cursor_coord.x]);

    field->field[cursor_coord.y + y_offset][cursor_coord.x] = card;
    if (card->object != Field_enum) card->object = Field_enum;
    else if (is_column(field, card)) {
        int column_size = get_column_size(field, card);

        for (int column_offset = 1; column_offset != column_size; column_offset++) {
            Card *column_card = field->field[card->coords.y + column_offset][card->coords.x];
            field->field[cursor_coord.y + column_offset + y_offset][cursor_coord.x] = column_card;
            column_card->coords.x = cursor_coord.x;
            column_card->coords.y = cursor_coord.y + y_offset + column_offset;
            column_card->selected = false;

            field->field[card->coords.y + column_offset][card->coords.x] = NULL;
        }
    }

    card->coords.x = cursor_coord.x;
    card->coords.y = cursor_coord.y + y_offset;
    card->selected = false;
}
