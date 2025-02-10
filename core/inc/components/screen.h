#ifndef SCREEN_H
#define SCREEN_H

#include "components.h"

/*
 * Color definitions
 * Basic color definitions for foreground and background
 */
enum Color {
    COLOR_NONE,
    COLOR_RESET,

    COLOR_BOLD,

    COLOR_BLACK,
    COLOR_RED,
    COLOR_GREEN,
    COLOR_YELLOW,
    COLOR_BLUE,
    COLOR_MAGENTA,
    COLOR_CYAN,
    COLOR_WHITE,

    COLOR_BRIGHT_BLACK,
    COLOR_BRIGHT_RED,
    COLOR_BRIGHT_GREEN,
    COLOR_BRIGHT_YELLOW,
    COLOR_BRIGHT_BLUE,
    COLOR_BRIGHT_MAGENTA,
    COLOR_BRIGHT_CYAN,
    COLOR_BRIGHT_WHITE,
};

const char *get_foreground(Color color);
const char *get_background(Color color);

/*
 * Terminal control macros
 * Basic terminal manipulation commands
 */
#define clear() wprintf(L"\033[H\033[J")
#define gotoxy(x,y) wprintf(L"\033[%d;%dH", (y), (x))
#define hide_cursor() wprintf(L"\033[?25l")
#define show_cursor() wprintf(L"\033[?25h")



#ifndef CUSTOM_SCREEN
struct Pixel {
    Color background; // Background colors/effects
    Color foreground; // Foreground colors/effects
    wchar_t symbol;   // Actual characters 
};

/*
 * Screen structure
 * Represents the game screen with dimensions, pixel data, and a render buffer.
 */
struct Screen {
    int height;        // Screen height in pixels
    int width;         // Screen width in pixels
    Pixel **pixels;    // 2D array of pixel data
    
    wchar_t *buffer;   // Render buffer for output
    int buffer_size;   // Size of the render buffer
};

#define SET_PIXEL_FOREGROUND(pixel, color) \
    if ((color) != COLOR_NONE) (pixel)->foreground = (color);

#define SET_PIXEL_BACKGROUND(pixel, color) \
    if ((color) != COLOR_NONE) (pixel)->background = (color);

#define SET_PIXEL_COLOR(pixel, config) \
    SET_PIXEL_FOREGROUND(pixel, (config).foreground) \
    SET_PIXEL_BACKGROUND(pixel, (config).background)
    
#define SET_PIXEL(pixel_target, pixel_source) \
    SET_PIXEL_COLOR(pixel_target, pixel_source) \
    (pixel_target)->symbol = (pixel_source).symbol;

/*
* Screen functions
* Screen manipulation and drawing
*/
Screen *init_screen(Arena *arena, int width, int height, Color background, Color foreground, wchar_t symbol);
void screen_shutdown(Screen *screen);
void print_screen(const Screen *screen);
void add_separator(Screen *screen, int y, int x, Color background, Color foreground, const wchar_t *borders);
void fill_area(Screen *screen, int y, int x, int height, int width, wchar_t symbol, Color background, Color foreground);
void add_borders(Screen *screen, int y, int x, int height, int width, Color background, Color foreground, const wchar_t *borders);
void insert_text(Screen *screen, int y, int x, const char *text, Color foreground, Color background);
void screen_draw_cursor(Screen *screen, Coords coords, CursorConfig config);
void set_noncanonical_mode(void);
void restore_terminal_settings(void);
#endif


#endif
