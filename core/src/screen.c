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
    if (color == COLOR_NONE) return "";

    static const char *foreground_colors[] = {
        [COLOR_RESET] = "m",
        [COLOR_BOLD] = ";1m",

        [COLOR_BLACK] = ";30m",
        [COLOR_RED] = ";31m",
        [COLOR_GREEN] = ";32m",
        [COLOR_YELLOW] = ";33m",
        [COLOR_BLUE] = ";34m",
        [COLOR_MAGENTA] = ";35m",
        [COLOR_CYAN] = ";36m",
        [COLOR_WHITE] = ";37m",

        [COLOR_BRIGHT_BLACK] = ";90m",
        [COLOR_BRIGHT_RED] = ";91m",
        [COLOR_BRIGHT_GREEN] = ";92m",
        [COLOR_BRIGHT_YELLOW] = ";93m",
        [COLOR_BRIGHT_BLUE] = ";94m",
        [COLOR_BRIGHT_MAGENTA] = ";95m",
        [COLOR_BRIGHT_CYAN] = ";96m",
        [COLOR_BRIGHT_WHITE] = ";97m",
    };
    return foreground_colors[color];
}

/*
 * Get ANSI escape sequence for background color
 * Returns the ANSI escape sequence for the specified color
 */
const char *get_background(Color color) {
    if (color == COLOR_NONE) return "";

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
Screen *init_screen(Arena *arena, int width, int height, Color background, Color foreground, wchar_t symbol) {
    Screen *screen = (Screen *)arena_alloc(arena, sizeof(Screen));
    screen->width = width;
    screen->height = height;
    screen->pixels = (Pixel **)arena_alloc(arena, (size_t)(height) * sizeof(Pixel *));

    Pixel pixel = (Pixel) {background, foreground, symbol};
    for (int i = 0; i < height; i++) {
        screen->pixels[i] = (Pixel *)arena_alloc(arena, (size_t)(width) * sizeof(Pixel));
        for (int j = 0; j < screen->width; j++) {
            screen->pixels[i][j] = pixel;
        }
    }
    screen->buffer_size = ((15) * screen->width * screen->height + 8 + screen->height) / 20;
    screen->buffer = (wchar_t *)arena_alloc(arena, sizeof(wchar_t) * (size_t)(screen->buffer_size));

    set_noncanonical_mode();
    setlocale(LC_ALL, "");
    hide_cursor();
    clear();

    return screen;
}

/*
 * Shutdown screen
 * Restores terminal settings and clears screen
 */
void screen_shutdown(Screen *screen) {
    (void)screen;
    clear();
    show_cursor();
    restore_terminal_settings();
}

/*
 * Add borders to specified area of screen
 * Creates a border using provided border characters
 */
void add_borders(Screen *screen, int y, int x, int height, int width, Color background, Color foreground, const wchar_t *borders) {
    Pixel horizontal_pixel = (Pixel) {background, foreground, borders[0]};
    for (int i = 0; i < width; ++i) {
        SET_PIXEL(&screen->pixels[y][x + i], horizontal_pixel);
        SET_PIXEL(&screen->pixels[y + height - 1][x + i], horizontal_pixel);
    }

    Pixel vertical_pixel = (Pixel) {background, foreground, borders[1]};
    for (int i = 0; i < height; ++i) {
        SET_PIXEL(&screen->pixels[y + i][x], vertical_pixel);
        SET_PIXEL(&screen->pixels[y + i][x + width - 1], vertical_pixel);
    }

    screen->pixels[y][x].symbol = borders[2];
    screen->pixels[y][x + width - 1].symbol = borders[3];
    screen->pixels[y + height - 1][x].symbol = borders[4];
    screen->pixels[y + height - 1][x + width - 1].symbol = borders[5];
}

/*
 * Add separator line to screen
 * Used for visual separation of screen areas
 */
void add_separator(Screen *screen, int y, int x, Color background, Color foreground , const wchar_t *borders) {
    Pixel pixel = (Pixel) {background, foreground, borders[0]};
    for (int i = 0; i < screen->width; ++i) {
        SET_PIXEL(&screen->pixels[y][x + i], pixel);
    }
    screen->pixels[y][x].symbol = borders[6];
    screen->pixels[y][screen->width - 1].symbol = borders[7];
}

/*
 * Print screen content to terminal
 * Outputs the entire screen buffer with formatting
 */
void print_screen(const Screen *screen) {
    int buf_idx = 0;
    
    buf_idx += swprintf(screen->buffer, 7, L"\033[0;0H");
    
    Color last_bg = COLOR_RESET;
    Color last_fg = COLOR_RESET;

    for (int y = 0; y < screen->height; ++y) {
        for (int x = 0; x < screen->width; ++x) {
            Pixel px = screen->pixels[y][x];

            if (buf_idx > screen->buffer_size - 20) {
                screen->buffer[buf_idx] = L'\0';
                wprintf(L"%ls", screen->buffer);
                buf_idx = 0;

                swprintf(screen->buffer, 11, L"%s%s", get_background(last_bg), get_foreground(last_fg));
                buf_idx = (int)wcslen(screen->buffer);
            }
            
            if (px.background != last_bg || px.foreground != last_fg) {
                buf_idx += swprintf(screen->buffer + buf_idx, 5, L"\033[0m");
                buf_idx += swprintf(screen->buffer + buf_idx, 11, L"%s%s", 
                                  get_background(px.background), 
                                  get_foreground(px.foreground));
                last_bg = px.background;
                last_fg = px.foreground;
            }
            
            screen->buffer[buf_idx++] = px.symbol;
        }
        screen->buffer[buf_idx++] = L'\n';
    }

    screen->buffer[buf_idx] = L'\0';
    wprintf(L"%ls\033[0m", screen->buffer);
}

/*
 * Fill rectangular area with specified symbol
 * Fills the area with specified symbol, background and foreground formatting
 */
void fill_area(Screen *screen, int y, int x, int height, int width, wchar_t symbol, Color background, Color foreground) {
    Pixel pixel = (Pixel) {background, foreground, symbol};
    for (int i = y; i < y + height; i++) {
        for (int j = x; j < x + width; j++) {
            SET_PIXEL(&screen->pixels[i][j], pixel);
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
    
    Pixel pixel = (Pixel) {background, foreground, ' '};
    for (int i = 0; i < text_length; i++) {
        SET_PIXEL(&screen->pixels[y][x + i], pixel);
        screen->pixels[y][x + i].symbol = text[i];
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

/*
 * Get cursor string
 * Returns the cursor string for the specified cursor type
 */
static const wchar_t* get_cursor_string(CursorType type) {
    static const wchar_t* cursor_strings[] = {
        [CURSOR_LEFT_SLIM]  = L"<",
        [CURSOR_RIGHT_SLIM] = L">",
        [CURSOR_UP_SLIM]    = L"⋀",
        [CURSOR_DOWN_SLIM]  = L"⋁",
        
        [CURSOR_LEFT_WIDE]  = L"V╱╲",
        [CURSOR_RIGHT_WIDE] = L"V╲╱",
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
void screen_draw_cursor(Screen *screen, Coords coords, CursorConfig config) {
    const wchar_t *cursor_string = get_cursor_string(config.type);
    if (!cursor_string) return;

    Pixel *main_pixel = &screen->pixels[coords.y][coords.x];
    SET_PIXEL_COLOR(main_pixel, config);

    main_pixel->symbol = cursor_string[0];
    if (config.type > CURSOR_WIDE) {
        main_pixel->symbol = cursor_string[1];
        int dx = (cursor_string[0] == L'H') ? 1 : 0;
        int dy = (cursor_string[0] == L'V') ? 1 : 0;

        Pixel *second_pixel = &screen->pixels[coords.y + dy][coords.x + dx];
        second_pixel->symbol = cursor_string[2];
        SET_PIXEL_COLOR(second_pixel, config);
    }
}
#endif
