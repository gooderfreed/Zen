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

Cursor init_cursor(Deck *deck, Field *field, Stock *stock) {
    Cursor cursor;
    cursor.coords.x = 0;
    cursor.coords.y = 0;
    cursor.card = NULL;
    cursor.subject = Field_enum;
    cursor.objects[Deck_enum]  = deck;
    cursor.objects[Field_enum] = field;
    cursor.objects[Stock_enum] = stock;
    return cursor;
}

void print_cursor(Screen *screen, const Cursor *cursor) {
    Coords coords = {.x = cursor->coords.x * CARD_WIDTH + (CARD_WIDTH / 2), 
                     .y = BORDER_OFFSET_Y + CARD_HEIGHT};

    switch (cursor->subject) {

    case Field_enum: {
        print_cursor_in_field(cursor, &coords);
        break;
    }

    // case Deck_enum:
    //     card_y = BORDER_OFFSET_Y + CARD_HEIGHT;
    //     card_x_center = cursor->coords.x * CARD_WIDTH + (CARD_WIDTH / 2) + BORDER_OFFSET_X - 1;
    //     break;

    // case Stock_enum:
    //     card_y = BORDER_OFFSET_Y + CARD_HEIGHT;
    //     card_x_center = cursor->coords.x * CARD_WIDTH + (CARD_WIDTH / 2) + SCREEN_WIDTH - (4 * CARD_WIDTH) - BORDER_OFFSET_X - 1;
    //     break;

    default: break;
    }
    
    screen->data[coords.y][coords.x] = '/';
    screen->data[coords.y][coords.x + 1] = '\\';
}

Coords try_move(const Cursor *cursor, int delta_x, int delta_y) {
    Coords result_coords = {.x = cursor->coords.x, .y = cursor->coords.y};

    switch (cursor->subject) {

    case Field_enum: {
        move_in_field(&result_coords, cursor, delta_x, delta_y);
        break;
    }

    // case Deck_enum:
    //     if (new_x < 2 && new_x >= 0) result_coords.x = new_x;
    //     if (delta_y > 0) result_coords.x = -(Field_enum + 1);
    //     if (new_x == 2) result_coords.x = -(Stock_enum + 1);
    //     break;

    // case Stock_enum:
    //     if (new_x < 4 && new_x >= 0) result_coords.x = new_x;
    //     if (delta_y > 0) result_coords.x = -(Field_enum + 1);
    //     if (new_x < 0) result_coords.x = -(Deck_enum + 1);
    //     break;

    default: break;
    }
    return result_coords;
}


void move_cursor(Cursor *cursor, int delta_x, int delta_y) {
    Coords new_coords = try_move(cursor, delta_x, delta_y);
    if (new_coords.x != cursor->coords.x || new_coords.y != cursor->coords.y) {
        cursor->coords = new_coords;
        // if (new_coords.x < 0) {
        //     cursor->subject = (-new_coords.x) - 1;
        //     cursor->coords  = (Coords){0, 0, 0};
        // }
        // else {
        //     cursor->coords = new_coords;
        // }
    }
}
