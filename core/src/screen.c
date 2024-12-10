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

#ifndef CUSTOM_SCREEN
/*
 * Default Screen implementation
 * Handles terminal output and screen buffer management
 */
#include "../inc/core.h"

/*
 * Get ANSI escape sequence for foreground color
 * Returns the ANSI escape sequence for the specified color
 */
const char *get_foreground(Color color) {
    static const char *foreground_colors[] = {
        [COLOR_RESET] = "",

        [COLOR_BLACK] = ";30",
        [COLOR_RED] = ";31",
        [COLOR_GREEN] = ";32",
        [COLOR_YELLOW] = ";33",
        [COLOR_BLUE] = ";34",
        [COLOR_MAGENTA] = ";35",
        [COLOR_CYAN] = ";36",
        [COLOR_WHITE] = ";37",

        [COLOR_BRIGHT_BLACK] = ";90",
        [COLOR_BRIGHT_RED] = ";91",
        [COLOR_BRIGHT_GREEN] = ";92",
        [COLOR_BRIGHT_YELLOW] = ";93",
        [COLOR_BRIGHT_BLUE] = ";94",
        [COLOR_BRIGHT_MAGENTA] = ";95",
        [COLOR_BRIGHT_CYAN] = ";96",
        [COLOR_BRIGHT_WHITE] = ";97",
    };
    return foreground_colors[color];
}

/*
 * Get ANSI escape sequence for background color
 * Returns the ANSI escape sequence for the specified color
 */
const char *get_background(Color color) {
    static const char *background_colors[] = {
        [COLOR_RESET] = "\033[0",
        [COLOR_BLACK] = "\033[40",
        [COLOR_RED] = "\033[41",
        [COLOR_GREEN] = "\033[42",
        [COLOR_YELLOW] = "\033[43",
        [COLOR_BLUE] = "\033[44",
        [COLOR_MAGENTA] = "\033[45",
        [COLOR_CYAN] = "\033[46",
        [COLOR_WHITE] = "\033[47",

        [COLOR_BRIGHT_BLACK] = "\033[100",
        [COLOR_BRIGHT_RED] = "\033[101",
        [COLOR_BRIGHT_GREEN] = "\033[102",
        [COLOR_BRIGHT_YELLOW] = "\033[103",
        [COLOR_BRIGHT_BLUE] = "\033[104",
        [COLOR_BRIGHT_MAGENTA] = "\033[105",
        [COLOR_BRIGHT_CYAN] = "\033[106",
        [COLOR_BRIGHT_WHITE] = "\033[107",
    };
    return background_colors[color];
}


/*
 * Initialize empty screen
 * Creates screen structure with cleared buffers
 */
Screen init_screen(void) {
    Screen screen;

    for (int i = 0; i < SCREEN_HEIGHT; i++) {
        for (int j = 0; j < SCREEN_WIDTH; j++) {
            screen.background[i][j] = COLOR_BLACK;
            screen.data[i][j] = ' ';
            screen.foreground[i][j] = COLOR_RESET;
        }
    }

    return screen;
}

/*
 * Add borders to specified area of screen
 * Creates a border using provided border characters
 */
void add_borders(Screen *screen, int y, int x, int height, int width, Color background, Color foreground, const wchar_t *borders) {
    for (int i = 0; i < width; ++i) {
        screen->background[y][x + i] = background;
        screen->data[y][x + i] = borders[0];
        screen->foreground[y][x + i] = foreground;

        screen->background[y + height - 1][x + i] = background;
        screen->data[y + height - 1][x + i] = borders[0];
        screen->foreground[y + height - 1][x + i] = foreground;
    }
    for (int i = 0; i < height; ++i) {
        screen->background[y + i][x] = background;
        screen->data[y + i][x] = borders[1];
        screen->foreground[y + i][x] = foreground;
        
        screen->background[y + i][x + width - 1] = background;
        screen->data[y + i][x + width - 1] = borders[1];
        screen->foreground[y + i][x + width - 1] = foreground;
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
void add_separator(Screen *screen, int y, int x, Color background, Color foreground , wchar_t *borders) {
    for (int i = 0; i < SCREEN_WIDTH; ++i) {
        screen->background[y][x + i] = background;
        screen->data[y][x + i] = borders[0];
        screen->foreground[y][x + i] = foreground;
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
            wprintf(L"%s%sm%lc\033[0m", get_background(screen->background[y][x]), get_foreground(screen->foreground[y][x]), screen->data[y][x]);
        }
        wprintf(L"\n");
    }
}

/*
 * Fill rectangular area with specified symbol
 * Fills the area with specified symbol, background and foreground formatting
 */
void fill_area(Screen *screen, int y, int x, int height, int width, wchar_t symbol, Color background, Color foreground) {
    for (int i = y; i < y + height; i++) {
        for (int j = x; j < x + width; j++) {
            screen->background[i][j] = background;
            screen->data[i][j] = symbol;
            screen->foreground[i][j] = foreground;
        }
    }
}
#endif
