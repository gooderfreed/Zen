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
    bool is_last_card = FIELD_WIDTH == cursor->coords.y || !field->field[cursor->coords.y + 1][cursor->coords.x];

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

void select_column(Field *field, Cursor *cursor) {
    for (int i = cursor->coords.y; field->field[i][cursor->coords.x] && i < FIELD_HEIGHT; ++i) {
        field->field[i][cursor->coords.x]->selected = !field->field[i][cursor->coords.x]->selected;
    }
}
