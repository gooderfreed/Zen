#ifndef CUSTOM_SCREEN
/*
 * Default Screen implementation
 * Handles terminal output and screen buffer management
 */
#include "../inc/core.h"

// -----------------------------------------------------------------------------
//  Color Utilities
// -----------------------------------------------------------------------------
// Extracts the red component from a 32-bit color.
inline unsigned char get_red(Color color) { return (color.color >> 16) & 0xFF; }

// Extracts the green component from a 32-bit color.
inline unsigned char get_green(Color color) { return (color.color >> 8) & 0xFF; }

// Extracts the blue component from a 32-bit color.
inline unsigned char get_blue(Color color) { return color.color & 0xFF; }

// Checks if a color is set to the 'NONE' value.
inline bool is_none(Color color) { return color.color == COLOR_NONE.color; }

// Creates a 32-bit RGB color from individual red, green, and blue components.
inline Color create_color_rgb(int r, int g, int b) {
    return (Color){0x00000000 | (uint32_t)((r & 0xFF) << 16) | (uint32_t)((g & 0xFF) << 8) | (uint32_t)(b & 0xFF << 0)};
}


// -----------------------------------------------------------------------------
//  ANSI Escape Code Generation
// -----------------------------------------------------------------------------
// Generates an ANSI escape sequence for text effects.
static char* get_effect_ansi(TextEffect effect) {
    static char ansi_str[16]; // Buffer for the ANSI escape sequence
    if (effect == Effect_None) return ""; // Return empty string if no effect

    snprintf(ansi_str, sizeof(ansi_str), "\033[%dm", (int)effect);
    return ansi_str;
}



// -----------------------------------------------------------------------------
//  Terminal Capability Detection (tput)
// -----------------------------------------------------------------------------

// static inline int is_terminal(void) {
//     return isatty(STDOUT_FILENO);
// }

// Checks if the 'tput' command exists in the system.
static inline int tput_exists(void) {
    return access("/usr/bin/tput", X_OK) == 0 || access("/bin/tput", X_OK) == 0;
}

// Executes a 'tput' command and returns the integer result.
static int tput_command(const char *command) {
    FILE *fp = popen(command, "r");
    if (!fp) return -1; // Return -1 on failure

    int result = -1;
    if (fscanf(fp, "%d", &result) != 1) { // Check for fscanf errors
        result = -1;
    }
    pclose(fp);

    return result;
}

// Retrieves and caches the number of colors supported by the terminal.
static int get_cached_tput_colors(void) {
    static int cached_colors = -2; // -2: uninitialized, -1: tput failed

    if (cached_colors == -2) {
        cached_colors = tput_exists() ? tput_command("tput colors 2>/dev/null") : -1;
    }

    return cached_colors;
}

// Checks if the terminal supports RGB (TrueColor) mode.
static int supports_rgb(int tput_colors) {
    const char *colorterm = getenv("COLORTERM");
    if (colorterm && strstr(colorterm, "truecolor")) return 1;

    const char *term = getenv("TERM");
    if (term && (strstr(term, "truecolor")    || strstr(term, "direct") ||
                 strstr(term, "xterm-direct") || strstr(term, "xterm-truecolor"))) {
        return 1;
    }
    if (tput_colors > 0) {
        return tput_colors >= (1 << 24); // TrueColor requires at least 2^24 colors
    }
    return 0;
}

// Checks if the terminal supports 256 colors.
static int supports_256(int tput_colors) {
    const char *term = getenv("TERM");
    if (term && strstr(term, "256color")) return 1;

    if (term) {
        // Check for common terminal types that often support 256 colors
        if (strstr(term, "xterm")  || strstr(term, "rxvt")  || strstr(term, "linux")     ||
            strstr(term, "screen") || strstr(term, "tmux")  || strstr(term, "vt100")     ||
            strstr(term, "vt220")  || strstr(term, "ansi")  || strstr(term, "konsole")   ||
            strstr(term, "Eterm")  || strstr(term, "gnome") || strstr(term, "alacritty") ||
            strstr(term, "st")     || strstr(term, "foot")  || strstr(term, "kitty")) {
            if (tput_colors > 0) return tput_colors >= 256; // Use tput if available
            return 1; // Assume 256-color support if TERM matches
        }
    }

    if (tput_colors >= 256) return 1; // Fallback to tput result

    return 0;
}

// Determines the terminal's color mode (Base, 256, or RGB).
static TerminalMode get_terminal_mode(void) {
    int tput_colors = get_cached_tput_colors();

    if (supports_rgb(tput_colors)) return Color_RGB;
    if (supports_256(tput_colors)) return Color_256;

    return Color_Base;
}


// -----------------------------------------------------------------------------
//  ANSI Escape Code Generation (Color Conversion)
// -----------------------------------------------------------------------------

// Converts RGB color to an ANSI escape sequence for TrueColor terminals.
static char* rgb_to_ansi(Color fg_color, Color bg_color) {
    static char ansi_str[128]; // Buffer for the ANSI escape sequence

    snprintf(ansi_str, sizeof(ansi_str), "\033[38;2;%d;%d;%d;48;2;%d;%d;%dm",
             get_red(fg_color), get_green(fg_color), get_blue(fg_color),
             get_red(bg_color), get_green(bg_color), get_blue(bg_color));

    return ansi_str;
}

// Converts RGB color to the nearest index in the 256-color palette.
static int rgb_to_256_index(Color color) {
    int r = get_red(color);
    int g = get_green(color);
    int b = get_blue(color);

    // Check for grayscale colors first
    int gray_index = -1;
    float gray_step = 255.0f / 24.0f;
    for (int i = 0; i < 24; i++) {
        int gray_val = 8 + i * 10;

        if (fabsf((float)(r - gray_val)) <= gray_step &&
            fabsf((float)(g - gray_val)) <= gray_step &&
            fabsf((float)(b - gray_val)) <= gray_step) {
            gray_index = 232 + i;
            break;
        }
    }
    if (gray_index >= 0) return gray_index;

    // If not grayscale, map to the 6x6x6 color cube
    int r_6 = (int)lroundf((float)r / 255.0f * 5.0f);
    int g_6 = (int)lroundf((float)g / 255.0f * 5.0f);
    int b_6 = (int)lroundf((float)b / 255.0f * 5.0f);

    return 16 + 36 * r_6 + 6 * g_6 + b_6;
}

// Converts RGB color to an ANSI escape sequence for 256-color terminals.
static char* rgb_to_ansi_256(Color fg_color, Color bg_color) {
    static char ansi_str[64]; // Buffer for the ANSI escape sequence

    snprintf(ansi_str, sizeof(ansi_str), "\033[38;5;%d;48;5;%dm",
             rgb_to_256_index(fg_color), rgb_to_256_index(bg_color));

    return ansi_str;
}

// Converts RGB color to an ANSI escape sequence for basic 8/16 color terminals.
static char *rgb_to_ansi_base(Color fg_color, Color bg_color) {
    static char ansi_str[32];  // Buffer for the ANSI escape sequence
    int r_fg = get_red(fg_color);
    int g_fg = get_green(fg_color);
    int b_fg = get_blue(fg_color);

    int r_bg = get_red(bg_color);
    int g_bg = get_green(bg_color);
    int b_bg = get_blue(bg_color);

    // Find the closest basic color index for foreground
    int index_fg = 0;
    float min_dist_fg = 1e9f; // Initialize with a large value

    // Find the closest basic color index for background
    int index_bg = 0;
    float min_dist_bg = 1e9f;

    // Standard 8 colors (black, red, green, yellow, blue, magenta, cyan, white)
    int basic_colors[8][3] = {
        {0, 0, 0},      // Black
        {128, 0, 0},    // Red
        {0, 128, 0},    // Green
        {128, 128, 0},  // Yellow
        {0, 0, 128},    // Blue
        {128, 0, 128},  // Magenta
        {0, 128, 128},  // Cyan
        {192, 192, 192} // Light Gray (NOT White)
    };

    // Bright 8 colors (corresponding bright versions)
    int bright_colors[8][3] = {
        {128, 128, 128},  // Dark Gray (Bright Black)
        {255, 0, 0},      // Bright Red
        {0, 255, 0},      // Bright Green
        {255, 255, 0},    // Bright Yellow
        {0, 0, 255},      // Bright Blue
        {255, 0, 255},    // Bright Magenta
        {0, 255, 255},    // Bright Cyan
        {255, 255, 255}   // Bright White
    };

    // Calculate Euclidean distance to find the closest basic color (foreground)
    for (int i = 0; i < 8; i++) {
        float dist = (float)sqrt(pow(r_fg - basic_colors[i][0], 2) +
                                 pow(g_fg - basic_colors[i][1], 2) +
                                 pow(b_fg - basic_colors[i][2], 2));
        if (dist < min_dist_fg) {
            min_dist_fg = dist;
            index_fg = i;
        }
        // Calculate Euclidean distance for background
        dist = (float)sqrt(pow(r_bg - basic_colors[i][0], 2) +
                           pow(g_bg - basic_colors[i][1], 2) +
                           pow(b_bg - basic_colors[i][2], 2));
        if (dist < min_dist_bg) {
            min_dist_bg = dist;
            index_bg = i;
        }
    }

    // Calculate Euclidean distance to find the closest bright color (foreground)
    for (int i = 0; i < 8; i++) {
        float dist = (float)sqrt(pow(r_fg - bright_colors[i][0], 2) +
                                 pow(g_fg - bright_colors[i][1], 2) +
                                 pow(b_fg - bright_colors[i][2], 2));
        if (dist < min_dist_fg) {
            min_dist_fg = dist;
            index_fg = i + 8; // Offset index for bright colors
        }
        //background
        dist = (float)sqrt(pow(r_bg - bright_colors[i][0], 2) +
                           pow(g_bg - bright_colors[i][1], 2) +
                           pow(b_bg - bright_colors[i][2], 2));
        if (dist < min_dist_bg) {
            min_dist_bg = dist;
            index_bg = i + 8;
        }
    }
    // Determine ANSI color codes based on foreground and background indices
    int fg_code = (index_fg < 8) ? (30 + index_fg) : (90 + (index_fg - 8)); // 30-37 or 90-97
    int bg_code = (index_bg < 8) ? (40 + index_bg) : (100 + (index_bg - 8)); // 40-47 or 100-107

    snprintf(ansi_str, sizeof(ansi_str), "\033[%d;%dm", fg_code, bg_code);
    return ansi_str;
}

// Selects the appropriate ANSI escape sequence based on terminal color mode.
static char* get_color_ansi(Color fg_color, Color bg_color, TerminalMode mode) {
    switch (mode) {
        case Color_RGB:   return rgb_to_ansi(fg_color, bg_color);
        case Color_256:   return rgb_to_ansi_256(fg_color, bg_color);
        case Color_Base:  return rgb_to_ansi_base(fg_color, bg_color);
        default:          return ""; // Should not happen, but return empty string for safety
    }
}



// -----------------------------------------------------------------------------
//  Screen Initialization and Shutdown
// -----------------------------------------------------------------------------
/*
 * Initializes an empty screen.
 * Creates the screen structure with cleared buffers and sets the terminal mode.
 */
Screen *init_screen(Arena *arena, int width, int height, Color background, Color foreground, wchar_t symbol) {
    Screen *screen = (Screen *)arena_alloc(arena, sizeof(Screen));
    screen->width = width;
    screen->height = height;
    screen->pixels = (Pixel **)arena_alloc(arena, (size_t)(height) * sizeof(Pixel *));
    screen->mode = get_terminal_mode();

    Pixel pixel = (Pixel) {background, foreground, symbol, Effect_None};
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
 * Shuts down the screen.
 * Restores terminal settings and clears the screen.
 */
void screen_shutdown(Screen *screen) {
    if (!screen) return; // defensive check
    clear();           // Clear the screen
    show_cursor();     // Show the cursor
    restore_terminal_settings(); // Restore terminal settings
}


// -----------------------------------------------------------------------------
//  Drawing Functions
// -----------------------------------------------------------------------------
/*
 * Add borders to specified area of screen
 * Creates a border using provided border characters
 */
void add_borders(Screen *screen, int y, int x, int height, int width, Color background, Color foreground, const wchar_t *borders) {
    if (!screen || !borders) return; // Check for NULL pointers
    if (y < 0 || x < 0 || height <= 0 || width <= 0) return; //basic checks
    if (y + height > screen->height || x + width > screen->width) return;

    // Draw horizontal borders (top and bottom)
    Pixel horizontal_pixel = (Pixel) {background, foreground, borders[0], Effect_None};
    for (int i = 0; i < width; ++i) {
        SET_PIXEL(&screen->pixels[y][x + i], horizontal_pixel);
        SET_PIXEL(&screen->pixels[y + height - 1][x + i], horizontal_pixel);
    }

    // Draw vertical borders (left and right)
    Pixel vertical_pixel = (Pixel) {background, foreground, borders[1], Effect_None};
    for (int i = 0; i < height; ++i) {
        SET_PIXEL(&screen->pixels[y + i][x], vertical_pixel);
        SET_PIXEL(&screen->pixels[y + i][x + width - 1], vertical_pixel);
    }

    // Set corner characters
    screen->pixels[y][x].symbol = borders[2];                          // Top-left
    screen->pixels[y][x + width - 1].symbol = borders[3];              // Top-right
    screen->pixels[y + height - 1][x].symbol = borders[4];             // Bottom-left
    screen->pixels[y + height - 1][x + width - 1].symbol = borders[5]; // Bottom-right
}

/*
 * Add separator line to screen
 * Used for visual separation of screen areas
 */
void add_separator(Screen *screen, int y, int x, Color background, Color foreground , const wchar_t *borders) {
    if (!screen || !borders) return;
    if (y < 0 || x < 0) return;
    if (y >= screen->height || x >= screen->width) return;

    Pixel pixel = (Pixel) {background, foreground, borders[0], Effect_Bold}; // Use bold effect
    for (int i = 0; i < screen->width; ++i) {
        SET_PIXEL(&screen->pixels[y][x + i], pixel);
    }
    // Set separator start/end characters
    screen->pixels[y][x].symbol = borders[6];
    screen->pixels[y][screen->width - 1].symbol = borders[7];
}

/*
 * Print screen content to terminal
 * Outputs the entire screen buffer with formatting
 */
void print_screen(const Screen *screen) {
    if (!screen) return;

    int buf_idx = 0;

    // Move cursor to the top-left corner (home position)
    buf_idx += swprintf(screen->buffer + buf_idx, MAX_ANSI_LENGTH, L"\033[0;0H");

    // Initialize last colors/effect to a value that won't match any real pixel
    Color last_bg = COLOR_NONE;
    Color last_fg = COLOR_NONE;
    TextEffect last_effect = Effect_None;

    for (int y = 0; y < screen->height; ++y) {
        for (int x = 0; x < screen->width; ++x) {
            Pixel px = screen->pixels[y][x];

            // Buffer overflow check
            if ((int)buf_idx > screen->buffer_size - MAX_ANSI_LENGTH) {
                screen->buffer[buf_idx] = L'\0'; // Null-terminate the string
                wprintf(L"%ls", screen->buffer);  // Print the buffer
                buf_idx = 0;                  // Reset the index
            }

            // Check if colors or effect have changed
            if (px.background.color != last_bg.color ||
                px.foreground.color != last_fg.color ||
                px.effect != last_effect) {

                // Combine reset, effect, and color setting into one escape sequence
                buf_idx += swprintf(screen->buffer + buf_idx, MAX_ANSI_LENGTH,
                                    L"\033[0m%s%s", // Reset and then set new attributes
                                    get_effect_ansi(px.effect),
                                    get_color_ansi(px.foreground, px.background, screen->mode)
                                    );
                // Update last known colors/effect
                last_bg = px.background;
                last_fg = px.foreground;
                last_effect = px.effect;
            }

            // Add the character to the buffer
            screen->buffer[buf_idx++] = px.symbol;
        }
        screen->buffer[buf_idx++] = L'\n'; // Add a newline at the end of each row
    }

    screen->buffer[buf_idx] = L'\0'; // Null-terminate the string
    wprintf(L"%ls\033[0m", screen->buffer); // Print the entire buffer and reset
}

/*
 * Fill rectangular area with specified symbol
 * Fills the area with specified symbol, background and foreground formatting
 */
void fill_area(Screen *screen, int y, int x, int height, int width, wchar_t symbol, Color background, Color foreground) {
    if (!screen) return; // NULL check
    if (x < 0 || y < 0 || height <= 0 || width <= 0) return;
    if (x + width > screen->width || y + height > screen->height) return;

    Pixel pixel = (Pixel) {background, foreground, symbol, Effect_None}; // Create the pixel
    for (int i = y; i < y + height; i++) {
        for (int j = x; j < x + width; j++) {
            SET_PIXEL(&screen->pixels[i][j], pixel); // Set the pixel using the macro
        }
    }
}

/*
 * Insert text into screen
 * Inserts text into the screen at the specified position
 */
void insert_text(Screen *screen, int y, int x, const char *text, Color foreground, Color background, TextEffect effect) {
    if (!screen || !text) return; // Check for NULL pointers
    if(x < 0 || y < 0) return;
    if (y >= screen->height || x >= screen->width) return;

    int text_length;
    for (text_length = 0; text[text_length] != '\0'; text_length++);

    if (x + text_length > screen->width) {
        text_length = screen->width - x;  // Truncate if text goes beyond screen width
    }
    
    Pixel pixel = (Pixel) {background, foreground, ' ', effect};
    for (int i = 0; i < text_length; i++) {
        SET_PIXEL(&screen->pixels[y][x + i], pixel);
        screen->pixels[y][x + i].symbol = text[i];
        screen->pixels[y][x + i].effect = effect;
    }
}

/*
 * Set noncanonical mode for terminal
 * Sets the terminal to noncanonical mode
 */
void set_noncanonical_mode(void) {
    struct termios term;
    if (tcgetattr(STDIN_FILENO, &term) == -1) return;

    term.c_lflag &= (tcflag_t)~(ICANON | ECHO);
    if (tcsetattr(STDIN_FILENO, TCSANOW, &term) == -1) return;
}

/*
 * Restore terminal settings
 * Restores the terminal to its original settings
 */
void restore_terminal_settings(void) {
    struct termios term;
    if (tcgetattr(STDIN_FILENO, &term) == -1) return;

    term.c_lflag |= ICANON | ECHO;
    if (tcsetattr(STDIN_FILENO, TCSANOW, &term) == -1) return;
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
    if (!screen) return;
    if (coords.x < 0 || coords.y < 0 || coords.x >= screen->width || coords.y >= screen->height) return;

    const wchar_t *cursor_string = get_cursor_string(config.type);
    if (!cursor_string) return;


    Pixel *main_pixel = &screen->pixels[coords.y][coords.x];
    SET_PIXEL_COLOR(main_pixel, config);
    main_pixel->effect = config.effect;

    main_pixel->symbol = cursor_string[0];
    // Handle wide cursors (which occupy two cells)
    if (config.type > CURSOR_WIDE) {
        main_pixel->symbol = cursor_string[1];
        int dx = (cursor_string[0] == L'H') ? 1 : 0; // Horizontal cursor: second char is to the right
        int dy = (cursor_string[0] == L'V') ? 1 : 0; // Vertical cursor: second char is below

        //bounds check
        if (coords.x + dx < screen->width && coords.y + dy < screen->height){
            Pixel *second_pixel = &screen->pixels[coords.y + dy][coords.x + dx];
            second_pixel->symbol = cursor_string[2];
            SET_PIXEL_COLOR(second_pixel, config);
        }
    }
}

#endif // CUSTOM_SCREEN
