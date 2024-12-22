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
        [COLOR_BOLD] = ";1",

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
        [COLOR_BOLD] = "\033[1",

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
Screen init_screen(Color background, Color foreground, wchar_t symbol) {
    Screen screen = {0};

    setlocale(LC_ALL, "");
    hide_cursor();
    // show_cursor();
    clear();

    for (int i = 0; i < SCREEN_HEIGHT; i++) {
        for (int j = 0; j < SCREEN_WIDTH; j++) {
            screen.background[i][j] = background;
            screen.data[i][j] = symbol;
            screen.foreground[i][j] = foreground;
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
void add_separator(Screen *screen, int y, int x, Color background, Color foreground , const wchar_t *borders) {
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

/*
 * Insert text into screen
 * Inserts text into the screen at the specified position
 */
void insert_text(Screen *screen, int y, int x, const char *text, Color foreground, Color background) {
    int text_length;
    for (text_length = 0; text[text_length] != '\0'; text_length++);
    
    for (int i = 0; i < text_length; i++) {
        screen->data[y][x + i] = text[i];
        screen->foreground[y][x + i] = foreground;
        screen->background[y][x + i] = background;
    }
}

/*
 * Set noncanonical mode for terminal
 * Sets the terminal to noncanonical mode
 */
void set_noncanonical_mode(void) {
    struct termios term;
    tcgetattr(STDIN_FILENO, &term);

    term.c_lflag &= (tcflag_t)~(ICANON | ECHO);
    tcsetattr(STDIN_FILENO, TCSANOW, &term);
}

/*
 * Restore terminal settings
 * Restores the terminal to its original settings
 */
void restore_terminal_settings(void) {
    struct termios term;
    tcgetattr(STDIN_FILENO, &term);

    term.c_lflag |= ICANON | ECHO;
    tcsetattr(STDIN_FILENO, TCSANOW, &term);
}

static const wchar_t* get_cursor_string(CursorType type) {
    static const wchar_t* cursor_strings[] = {
        [CURSOR_LEFT_SLIM]  = L"<",
        [CURSOR_RIGHT_SLIM] = L">",
        [CURSOR_UP_SLIM]    = L"⋀",
        [CURSOR_DOWN_SLIM]  = L"⋁",
        
        [CURSOR_LEFT_WIDE]  = L"W⟋⟍",
        [CURSOR_RIGHT_WIDE] = L"W⟍⟋",
        [CURSOR_UP_WIDE]    = L"H╱╲",
        [CURSOR_DOWN_WIDE]  = L"H╲╱",
        
        [CURSOR_SLIM]       = L"●",
        [CURSOR_WIDE]       = L"◖◗",
    };
    
    return cursor_strings[type];
}

/*
 * Draw cursor on screen
 * Draws the cursor at the specified coordinates
 */
void screen_draw_cursor(Screen *screen, Coords coords, CursorType type) {
    const wchar_t *cursor_string = get_cursor_string(type);
    if (cursor_string) {
        if (type <= CURSOR_WIDE) {
            screen->data[coords.y][coords.x] = cursor_string[0];
        } else {
            screen->data[coords.y][coords.x] = cursor_string[1];
            if (cursor_string[0] == L'H') {
                screen->data[coords.y][coords.x + 1] = cursor_string[2];
            } else {
                screen->data[coords.y + 1][coords.x] = cursor_string[2];
            }
        }
    }
}
#endif
