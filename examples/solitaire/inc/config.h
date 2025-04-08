#ifndef CONFIG_H
#define CONFIG_H

// =========================
//  CARD CONFIGURATION
// =========================
#define CARD_NUMERALS 13
#define CARD_SCALE_FACTOR 7
#define CARD_COVERED_HEIGHT 2
#define CARD_HEIGHT CARD_SCALE_FACTOR
#define CARD_WIDTH (CARD_SCALE_FACTOR + 1)

// =========================
//  BORDER CONFIGURATION
// =========================
#define BORDER_OFFSET_X 1
#define BORDER_OFFSET_Y 1

// =========================
//  DECK CONFIGURATION
// =========================
#define DECK_OFFSET (CARD_HEIGHT + 2 * BORDER_OFFSET_Y)
#define DECK_SIZE (CARD_SUITS * CARD_NUMERALS)

// =========================
//  FIELD CONFIGURATION
// =========================
#define FIELD_HEIGHT 19
#define FIELD_WIDTH 7

// =========================
//  SCREEN CONFIGURATION
// =========================
#define SCREEN_HEIGHT (2 * (FIELD_HEIGHT - 1) + CARD_HEIGHT + DECK_OFFSET + BORDER_OFFSET_X)
#define SCREEN_WIDTH (FIELD_WIDTH * CARD_WIDTH + 2 * BORDER_OFFSET_X)

// =========================
//  STOCK CONFIGURATION
// =========================
#define STOCK_X_BASE (SCREEN_WIDTH - BORDER_OFFSET_X - 4 * CARD_WIDTH)
#define STOCK_AREA_HEIGHT (CARD_HEIGHT + 1)
#define STOCK_AREA_WIDTH (4 * CARD_WIDTH)
#define STOCK_CURSOR_BASE_X (STOCK_X_BASE + (CARD_WIDTH / 2 - 1))

// =========================
//  LAYERS CONFIGURATION
// =========================
#define GAME_LAYERS 3

// =========================
//  MENU LAYER CONFIGURATION
// =========================
#define MENU_LAYER_WIDTH  2
#define MENU_LAYER_HEIGHT 1
#define MENU_ID 0
#define MENU_DEFAULT_COORDS (Coords){.x = 0, .y = 0}

// =========================
//  GAME LAYER CONFIGURATION
// =========================
#define GAME_LAYER_WIDTH  3
#define GAME_LAYER_HEIGHT 1
#define GAME_ID 1
#define GAME_DEFAULT_COORDS (Coords){.x = 1, .y = 0}

// =========================
//  WIN LAYER CONFIGURATION
// =========================
#define WIN_LAYER_WIDTH  1
#define WIN_LAYER_HEIGHT 1
#define WIN_ID  2
#define WIN_DEFAULT_COORDS  (Coords){.x = 0, .y = 0}
#define WIN_SCREEN_HEIGHT 20
#define WIN_SCREEN_WIDTH 39

// =========================
//  UTILS CONFIGURATION
// =========================
#define BUTTON_HANDLER_SIZE 4
#define CONTAINER_SIZE CARD_NUMERALS

#endif
