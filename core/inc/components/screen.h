#ifndef SCREEN_H
#define SCREEN_H

#include "components.h"

/*
 * Terminal control macros
 * Basic terminal manipulation commands
 */

// -----------------------------------------------------------------------------
//  Constants and Macros
// -----------------------------------------------------------------------------
#define COLOR_BLACK  ((Color){0x00000000})
#define COLOR_RED    ((Color){0x00FF0000})
#define COLOR_GREEN  ((Color){0x0000FF00})
#define COLOR_BLUE   ((Color){0x000000FF})
#define COLOR_WHITE  ((Color){0x00FFFFFF})
#define COLOR_YELLOW ((Color){0x00FFFF00})

#define COLOR_NONE   ((Color){0xF0FFFFFF}) // Use a value unlikely to be a valid color


#define MAX_ANSI_LENGTH 50 // Define a reasonable maximum for ANSI sequences

#ifndef CUSTOM_SCREEN // Allow users to provide their own screen implementation

// Terminal control macros
#define clear()       wprintf(L"\033[H\033[J")          // Clear the entire screen
#define gotoxy(x, y)  wprintf(L"\033[%d;%dH", (y), (x)) // Position cursor
#define hide_cursor() wprintf(L"\033[?25l")             // Hide the cursor
#define show_cursor() wprintf(L"\033[?25h")             // Show the cursor


// -----------------------------------------------------------------------------
//  Type Definitions
// -----------------------------------------------------------------------------
struct Color {
    uint32_t color; // 32-bit integer for color (0x00RRGGBB)
};

// Text effects (bold, italic, etc.).
enum TextEffect {
    Effect_None       = 0,
    Effect_Bold       = 1,  // Bold/bright
    Effect_Italic     = 3,  // Italic (not widely supported)
    Effect_Underline  = 4,  // Underline
    Effect_Blink      = 5,  // Blink (rarely supported, can be annoying)
    Effect_Reverse    = 7,  // Reverse video (swap foreground and background)
    Effect_Conceal    = 8,  // Conceal (hide text, rarely supported)
};

// Represents a single pixel on the screen.
struct Pixel {
    Color      background; // Background color
    Color      foreground; // Foreground color
    wchar_t    symbol;     // Character to display
    TextEffect effect;     // Text effect (bold, italic, etc.)
};

// Terminal color modes.
typedef enum {
    Color_Base, // Basic 8/16 colors
    Color_256,  // 256-color mode
    Color_RGB   // TrueColor (RGB) mode
} TerminalMode;


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

    TerminalMode mode; // Terminal color mode
};


/*
* Screen functions
* Screen manipulation and drawing
*/
// Color component extraction (inline for efficiency)
inline unsigned char get_red(Color color);
inline unsigned char get_green(Color color);
inline unsigned char get_blue(Color color);
inline Color create_color_rgb(int r, int g, int b);
inline bool is_none(Color color);

// Screen management
Screen *init_screen(Arena *arena, int width, int height, Color background, Color foreground, wchar_t symbol);
void    screen_shutdown(Screen *screen);
void    print_screen(const Screen *screen);

// Drawing functions
void add_borders(Screen *screen, int y, int x, int height, int width, Color background, Color foreground, const wchar_t *borders);
void add_separator(Screen *screen, int y, int x, Color background, Color foreground, const wchar_t *borders);
void fill_area(Screen *screen, int y, int x, int height, int width, wchar_t symbol, Color background, Color foreground);
void insert_text(Screen *screen, int y, int x, const char *text, Color foreground, Color background, TextEffect effect);
void screen_draw_cursor(Screen *screen, Coords coords, CursorConfig config); // Assuming Coords and CursorConfig are defined in components.h

// Terminal setup (non-canonical mode)
void set_noncanonical_mode(void);
void restore_terminal_settings(void);


// -----------------------------------------------------------------------------
//  Macros for Pixel Manipulation
// -----------------------------------------------------------------------------
#define SET_PIXEL_FOREGROUND(pixel, color)                 \
    if (!is_none(color)) (pixel)->foreground = (color);

#define SET_PIXEL_BACKGROUND(pixel, color)                 \
    if (!is_none(color)) (pixel)->background = (color);

#define SET_PIXEL_COLOR(pixel, config)                     \
    SET_PIXEL_FOREGROUND(pixel, (config).foreground)       \
    SET_PIXEL_BACKGROUND(pixel, (config).background)
    
#define SET_PIXEL(pixel_target, pixel_source)              \
    do {                                                   \
        SET_PIXEL_COLOR(pixel_target, pixel_source)        \
        (pixel_target)->symbol = (pixel_source).symbol;    \
    } while(0);

#endif  // CUSTOM_SCREEN

#endif // SCREEN_H
