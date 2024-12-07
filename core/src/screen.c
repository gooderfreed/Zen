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

/*
 * Screen implementation
 * Handles terminal output and screen buffer management
 */
#include "../inc/core.h"

/*
 * Initialize empty screen
 * Creates screen structure with cleared buffers
 */
Screen init_screen(void) {
    Screen screen;

    for (int i = 0; i < SCREEN_HEIGHT; i++) {
        for (int j = 0; j < SCREEN_WIDTH; j++) {
            screen.background[i][j] = "\033[0";
            screen.data[i][j] = '#';
            screen.foreground[i][j] = "";
        }
    }

    return screen;
}

/*
 * Add borders to specified area of screen
 * Creates a border using provided border characters
 */
void add_borders(Screen *screen, int y, int x, int height, int width, const wchar_t *borders) {
    for (int i = 0; i < width; ++i) {
        screen->data[y][x + i] = borders[0];
        screen->data[y + height - 1][x + i] = borders[0];
    }
    for (int i = 0; i < height; ++i) {
        screen->data[y + i][x] = borders[1];
        screen->data[y + i][x + width - 1] = borders[1];
    }

    screen->data[y][x] = borders[2];
    screen->data[y][x + width - 1] = borders[3];
    screen->data[y + height - 1][x] = borders[4];
    screen->data[y + height - 1][x + width - 1] = borders[5];
}

/*
 * Add separator line to screen
 * Used for visual separation of screen areas
 */
void add_separator(Screen *screen, int y, int x, wchar_t *borders) {
    for (int i = 0; i < SCREEN_WIDTH; ++i) {
        screen->data[y][x + i] = borders[0];
    }
    screen->data[y][x] = borders[6];
    screen->data[y][SCREEN_WIDTH - 1] = borders[7];
}

/*
 * Print screen content to terminal
 * Outputs the entire screen buffer with formatting
 */
void print_screen(const Screen *screen) {
    gotoxy(0, 0);
    for (int y = 0; y < SCREEN_HEIGHT; ++y) {
        for (int x = 0; x < SCREEN_WIDTH; ++x) {
            wprintf(L"%s%sm%lc\033[0m", screen->background[y][x], screen->foreground[y][x], screen->data[y][x]);
        }
        wprintf(L"\n");
    }
}

/*
 * Fill rectangular area with specified symbol
 * Clears background and foreground formatting in the area
 */
void fill_area(Screen *screen, int y, int x, int height, int width, wchar_t symbol) {
    for (int i = y; i < y + height; i++) {
        for (int j = x; j < x + width; j++) {
            screen->background[i][j] = "\033[0";
            screen->data[i][j] = symbol;
            screen->foreground[i][j] = " ";
        }
    }
}
