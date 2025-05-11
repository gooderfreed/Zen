#ifndef MINESWEEPER_H
#define MINESWEEPER_H

#define _POSIX_C_SOURCE 199309L

#include "config.h"
#include "zen.h"
// #include <stdio.h>
// #include <stdlib.h>
// #include <time.h>
// #include <string.h>

/*
 * Minesweeper game implementation
 * Extends zen framework with specific minesweeper game logic
 */

#define CELL_MINE_MASK         0x01
#define CELL_REVEALED_MASK     0x02
#define CELL_FLAGGED_MASK      0x04
#define CELL_HIGHLIGHTED_MASK  0x08

/*
 * MinesweeperBoard structure
 * Represents the minesweeper board with cells, mines, and state.
 */
typedef struct MinesweeperBoard {
    ObjectInterfaces interfaces; // Zen framework interfaces for interaction
    
    uint8_t *cells;    // packed cells array
    int width;         // width of the board
    int height;        // height of the board
    int mines;         // number of mines on the board
} MinesweeperBoard;

MinesweeperBoard *init_field(Arena *arena, Coords size, int mines);

extern const wchar_t fat_border[8];
extern const wchar_t card_border[8];

MapLayer *game_layer_init(Arena *arena);

#endif
