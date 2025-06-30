#include "../../inc/minesweeper.h"

// =========================
//  MINESWEEPER_FIELD METHODS
// =========================
/*
 * Get the cell at the given index
 */
static inline uint8_t *get_spot(MinesweeperBoard *board, int index) {
    return &board->cells[index];
}

/*
 * Check if the cell at the given coordinates is a mine
 */
static inline bool is_mine(MinesweeperBoard *board, int x, int y) {
    int linear_index = y * board->width + x;
    int byte_index = linear_index / 2;
    uint8_t *cell = get_spot(board, byte_index);
    int bit_offset = (linear_index % 2) * 4;
    return (*cell & (CELL_MINE_MASK << bit_offset)) != 0;
}

/*
 * Set the cell at the given coordinates to a mine
 */
static inline void set_mine(MinesweeperBoard *board, int x, int y) {
    int linear_index = y * board->width + x;
    int byte_index = linear_index / 2;
    uint8_t *cell = get_spot(board, byte_index);
    int bit_offset = (linear_index % 2) * 4;
    *cell |= (CELL_MINE_MASK << bit_offset);
}


// /*
//  * Check if the cell at the given coordinates is revealed
//  */
// static inline bool is_revealed(MinesweeperBoard *board, int x, int y) {
//     int linear_index = y * board->width + x;
//     int byte_index = linear_index / 2;
//     uint8_t *cell = get_spot(board, byte_index);
//     int bit_offset = (linear_index % 2) * 4;
//     return (*cell & (CELL_REVEALED_MASK << bit_offset)) != 0;
// }

// /*
//  * Set the cell at the given coordinates to revealed
//  */
// static inline void set_revealed(MinesweeperBoard *board, int x, int y) {
//     int linear_index = y * board->width + x;
//     int byte_index = linear_index / 2;
//     uint8_t *cell = get_spot(board, byte_index);
//     int bit_offset = (linear_index % 2) * 4;
//     *cell |= (CELL_REVEALED_MASK << bit_offset);
// }


// /*
//  * Check if the cell at the given coordinates is flagged
//  */
// static inline bool is_flagged(MinesweeperBoard *board, int x, int y) {
//     int linear_index = y * board->width + x;
//     int byte_index = linear_index / 2;
//     uint8_t *cell = get_spot(board, byte_index);
//     int bit_offset = (linear_index % 2) * 4;
//     return (*cell & (CELL_FLAGGED_MASK << bit_offset)) != 0;
// }

// /*
//  * Set the cell at the given coordinates to flagged
//  */
// static inline void set_flagged(MinesweeperBoard *board, int x, int y) {
//     int linear_index = y * board->width + x;
//     int byte_index = linear_index / 2;
//     uint8_t *cell = get_spot(board, byte_index);
//     int bit_offset = (linear_index % 2) * 4;
//     *cell |= (CELL_FLAGGED_MASK << bit_offset);
// }


// /*
//  * Check if the cell at the given coordinates is highlighted
//  */
// static inline bool is_highlighted(MinesweeperBoard *board, int x, int y) {
//     int linear_index = y * board->width + x;
//     int byte_index = linear_index / 2;
//     uint8_t *cell = get_spot(board, byte_index);
//     int bit_offset = (linear_index % 2) * 4;
//     return (*cell & (CELL_HIGHLIGHTED_MASK << bit_offset)) != 0;
// }

// /*
//  * Set the cell at the given coordinates to highlighted
//  */
// static inline void set_highlighted(MinesweeperBoard *board, int x, int y) {
//     int linear_index = y * board->width + x;
//     int byte_index = linear_index / 2;
//     uint8_t *cell = get_spot(board, byte_index);
//     int bit_offset = (linear_index % 2) * 4;
//     *cell |= (CELL_HIGHLIGHTED_MASK << bit_offset);
// }

/*
 * Get the number of mines around the given coordinates
 */
static inline int get_mines_around(MinesweeperBoard *board, int x, int y) {
    int count = 0;
    for (int dx = -1; dx <= 1; dx++) {
        for (int dy = -1; dy <= 1; dy++) {
            if ((dx == 0 && dy == 0)
                || x + dx < 0
                || x + dx >= board->width
                || y + dy < 0
                || y + dy >= board->height) continue;
            if (is_mine(board, x + dx, y + dy)) count++;
        }
    }
    return count;
}

static void generate_field(MinesweeperBoard *board) {
    for (int i = 0; i < board->width * board->height; i++) {
        board->cells[i] = 0;
    }
    
    for (int i = 0; i < board->mines; i++) {
        int x = rand() % board->width;
        int y = rand() % board->height;
        if (is_mine(board, x, y)) {
            i--;
            continue;
        }
        set_mine(board, x, y);
    }
}



// =========================
//  DRAWABLE INTERFACE IMPLEMENTATION
// =========================
/*
 * Draw a closed cell
 */
// static void draw_closed_cell(Screen *screen, int x, int y) {
//     fill_area(screen, y + 1, x + 1 * SCALE_FACTOR_X, 3 * SCALE_FACTOR_Y, 3 * SCALE_FACTOR_X, L' ', COLOR_LIGHT_GRAY, COLOR_BLACK);
//     fill_area(screen, y + 1, x + 4 * SCALE_FACTOR_X, 4 * SCALE_FACTOR_Y,     SCALE_FACTOR_X, L' ', COLOR_DARK_GRAY,  COLOR_BLACK);
//     fill_area(screen, y + 1, x + 0 * SCALE_FACTOR_X, 3 * SCALE_FACTOR_Y,     SCALE_FACTOR_X, L' ', COLOR_WHITE,      COLOR_BLACK);
//     fill_area(screen, y + 4, x + 1 * SCALE_FACTOR_X, SCALE_FACTOR_Y,     3 * SCALE_FACTOR_X, L' ', COLOR_DARK_GRAY,  COLOR_BLACK);
//     fill_area(screen, y + 0, x + 0 * SCALE_FACTOR_X, SCALE_FACTOR_Y,     4 * SCALE_FACTOR_X, L' ', COLOR_WHITE,      COLOR_BLACK);
// }

static void draw_open_cell_base(Screen *screen, int x, int y) {
    fill_area(screen, y + 0, x + 0, 5, 10, L' ', COLOR_LIGHT_GRAY, COLOR_LIGHT_GRAY);
    insert_wtext(screen, y + 0, x + 0, L"█▀▀▀▀▀▀▀▀█", COLOR_DARK_GRAY, COLOR_NONE, Effect_None);
    insert_wtext(screen, y + 1, x + 0, L"█        █", COLOR_DARK_GRAY, COLOR_NONE, Effect_None);
    insert_wtext(screen, y + 2, x + 0, L"█        █", COLOR_DARK_GRAY, COLOR_NONE, Effect_None);
    insert_wtext(screen, y + 3, x + 0, L"█        █", COLOR_DARK_GRAY, COLOR_NONE, Effect_None);
    insert_wtext(screen, y + 4, x + 0, L"█▄▄▄▄▄▄▄▄█", COLOR_DARK_GRAY, COLOR_NONE, Effect_None);
}

static Color get_cell_color(int mines_around) {
    switch (mines_around) {
        case 1:  return ((Color){0x000000fa});
        case 2:  return ((Color){0x00007d00});
        case 3:  return ((Color){0x00fd0000});
        case 4:  return ((Color){0x0000007b});
        case 5:  return ((Color){0x00800000});
        case 6:  return ((Color){0x00017f7f});
        case 7:  return ((Color){0x00000000});
        case 8:  return ((Color){0x00808080});
        default: return COLOR_NONE;
    }
}

static wchar_t *get_cell_char(int mines_around) {
    switch (mines_around) {
        case 1:  return L"▗▟█ \0 ▐█ \0▟██▙\0";
        case 2:  return L"▟▛▀▙\0 ▗█▛\0▟█▙▄\0";
        case 3:  return L"▟▀▀▙\0 ▝▀▙\0▜▄▟▛\0";
        case 4:  return L"█ █▌\0▜███\0  █▌\0";
        case 5:  return L"▟▛▀▀\0▝▀▀▙\0▜▙▄▛\0";
        case 6:  return L"▟▛▀▘\0▟▛▀▙\0▜▙▄▛\0";
        case 7:  return L"▝▀▜█\0 ▗█▘\0 █▌ \0";
        case 8:  return L"▟▀▀▙\0▟▀▀▙\0▜▄▄▛\0";
        default: return L"    \0    \0    \0";
    }
}

static void draw_cell(Screen *screen, int x, int y, int mines_around) {
    draw_open_cell_base(screen, x, y);
    Color color = get_cell_color(mines_around);
    wchar_t *cell_char = get_cell_char(mines_around);
    insert_wtext(screen, y + 1, x + 3, cell_char, color, COLOR_NONE, Effect_None);
    insert_wtext(screen, y + 2, x + 3, &cell_char[5], color, COLOR_NONE, Effect_None);
    insert_wtext(screen, y + 3, x + 3, &cell_char[10], color, COLOR_NONE, Effect_None);
}

// static void draw_flag(Screen *screen, int x, int y) {
//     draw_closed_cell(screen, x, y);
//     insert_wtext(screen, y + 1, x + 2, L"  ▄▟▌", COLOR_RED, COLOR_NONE, Effect_None);
//     insert_wtext(screen, y + 2, x + 2, L" ▟██▌", COLOR_RED, COLOR_NONE, Effect_None);
//     insert_wtext(screen, y + 3, x + 2, L" ▄▄█▙", COLOR_BLACK, COLOR_NONE, Effect_None);
// }

static void draw_mine(Screen *screen, int x, int y) {
    draw_open_cell_base(screen, x, y);
    insert_wtext(screen, y + 1, x + 2, L"▝▚▟▙▞▘", COLOR_BLACK, COLOR_NONE, Effect_None);
    insert_wtext(screen, y + 2, x + 2, L"▀████▀", COLOR_BLACK, COLOR_NONE, Effect_None);
    insert_wtext(screen, y + 3, x + 2, L"▗▞▜▛▚▖", COLOR_BLACK, COLOR_NONE, Effect_None);
}

/*
 * Render the field on the screen
 * Draws the field using colored characters.
 */
static void print_field(const void *field_pointer, Screen *screen, const Cursor *cursor) {
    (void)cursor;
    MinesweeperBoard *field = (MinesweeperBoard *)field_pointer;

    for (int y = 0; y < field->height; y++) {
        for (int x = 0; x < field->width; x++) {
            if (is_mine(field, x, y)) {
                draw_mine(screen, x * CELL_WIDTH + BORDER_SIZE_X, y * CELL_HEIGHT + BORDER_SIZE_Y);
            } else {
                draw_cell(screen, x * CELL_WIDTH + BORDER_SIZE_X, y * CELL_HEIGHT + BORDER_SIZE_Y, get_mines_around(field, x, y));
            }
            // draw_mine(screen, x * CELL_WIDTH + BORDER_SIZE_X, y * CELL_HEIGHT + BORDER_SIZE_Y);
            // draw_closed_cell(screen, x * CELL_WIDTH + BORDER_SIZE_X, y * CELL_HEIGHT + BORDER_SIZE_Y);
            // draw_open_cell_base(screen, x * CELL_WIDTH + BORDER_SIZE_X, y * CELL_HEIGHT + BORDER_SIZE_Y);
            // draw_cell(screen, x * CELL_WIDTH + BORDER_SIZE_X, y * CELL_HEIGHT + BORDER_SIZE_Y, x);
        }
    }
}




// =========================
//  CURSOR_INTERACTABLE INTERFACE IMPLEMENTATION
// =========================
/*
 * Place cursor in field
 * Calculates target coordinates based on cursor position and updates them
 */
static Coords place_cursor_in_field(const void *field_pointer, const Coords cursor_coords) {
    (void)field_pointer;
    printf("Cursor coords: %d, %d\n", cursor_coords.x, cursor_coords.y);

    return (Coords) {
        .y = (short)(BORDER_SIZE_Y + cursor_coords.y * CELL_HEIGHT),
        .x = (short)(BORDER_SIZE_X + cursor_coords.x * CELL_WIDTH)
    };
}

static void print_cursor_in_field(const void *field_pointer, const Cursor *cursor, Screen *screen, const Coords coords) {
    (void)field_pointer;
    (void)cursor;

    for (int x = 0; x < CELL_WIDTH; x++) {
        if ((x / 2) % 2 == 0) {
            put_pixel(screen, coords.y, coords.x + x, L'▀', COLOR_NONE, COLOR_BLACK, Effect_None);
            put_pixel(screen, coords.y + CELL_HEIGHT - 1, coords.x + x, L'▄', COLOR_NONE, COLOR_BLACK, Effect_None);
        }
    }

    for (int y = 0; y < CELL_HEIGHT; y++) {
        if (y % 2 == 0) {
            put_pixel(screen, coords.y + y, coords.x, L'█', COLOR_NONE, COLOR_BLACK, Effect_None);
            put_pixel(screen, coords.y + y, coords.x + CELL_WIDTH - 1, L'█', COLOR_NONE, COLOR_BLACK, Effect_None);
        }
    }
    
}

/*
 * Move cursor in field
 * Updates cursor coordinates based on delta and ensures they are within valid bounds
 */
static void move_in_field(const void *field_pointer, Coords *coords, const Coords delta) {
    short new_x = coords->x + delta.x;
    short new_y = coords->y + delta.y;

    const MinesweeperBoard *field = (const MinesweeperBoard *)field_pointer;

    if (new_x < 0) new_x = (short)(field->width - 1);
    if (new_x >= field->width) new_x = 0;
    if (new_y < 0) new_y = (short)(field->height - 1);
    if (new_y >= field->height) new_y = 0;

    coords->x = new_x;
    coords->y = new_y;
}

/*
 * Get default coords
 * Gets the default coords of the field
 */
static Coords get_default_coords(const void *field_pointer) {
    const MinesweeperBoard *field = (const MinesweeperBoard *)field_pointer;
    return (Coords) {.x = (short)(field->width / 2), .y = (short)(field->height / 2)};
}

/*
 * Get cursor config
 * Gets the cursor config of the field
 */
static CursorConfig get_cursor_config_in_field(const void *field_pointer, const Coords cursor_coords) {
    (void)field_pointer;
    (void)cursor_coords;
    return (CursorConfig) {
        .type = CURSOR_CUSTOM,
    };
}



/*
 * Initialize field object
 * Allocates memory, sets initial width, height, and mines.
 * Also initializes interfaces for rendering.
 */
MinesweeperBoard *init_field(Arena *arena, Coords size, int mines) {
    // Allocate memory for field structure
    MinesweeperBoard *field = (MinesweeperBoard *)arena_alloc(arena, sizeof(MinesweeperBoard));
    *field = (MinesweeperBoard) {
        .width     = size.x,
        .height    = size.y,
        .mines     = mines,
    };
    field->cells = (uint8_t *)arena_alloc(arena, (size_t)(field->width * field->height) / 2);

    generate_field(field);


    INTERFACES(arena, field, {
        DRAWABLE(print_field);
        CURSOR_INTERACTABLE_CUSTOM(place_cursor_in_field, move_in_field, get_default_coords, get_cursor_config_in_field, print_cursor_in_field);
    });

    return field;
}
