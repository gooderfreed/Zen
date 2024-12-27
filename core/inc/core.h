/* 
 * Core engine header - defines interfaces and base structures for card games
 * This is the main interface file that all implementations must include
 */

#ifndef CORE_H
#define CORE_H

#include <stdbool.h>
#include <wchar.h>
#include <termios.h>
#include <unistd.h>
#include <locale.h>

/*
 * Dynamic memory configuration
 * Define DYNAMIC to enable all dynamic memory features
 */
#ifdef DYNAMIC
    #define CONTAINER_DYNAMIC
    #define SCREEN_DYNAMIC
    #define MAP_DYNAMIC
#endif

#include "interfaces/interfaces.h"
#include "components/components.h"
#include "validator.h"

/*
 * Key definitions
 * Common keyboard input codes
 */
#define KEY_ESC L''
#define KEY_ENTER 10
#define KEY_SPACE 32
#define KEY_CTRL_A 1
#define KEY_CTRL_D 4

/*
 * Core structure
 * Main game engine structure containing all necessary components
 */
typedef struct {
    Cursor *cursor;  // Game cursor
    Screen *screen;  // Game screen
    Map *map;        // Game map
} Core;


/*
 * Core functions
 * Main engine functionality
 */
Core init_core(Map *map, Cursor *cursor, Screen *screen);
void core_shutdown(Core *core);
void core_local_move(Core *core, Coords move);
void core_action(Core *core);
void core_update(Core *core);
void core_update_screen(Core *core);
void core_global_move(Core *core, Coords move);
void core_validate_interfaces(Core *core);
void core_free(Core *core);
void core_change_layer(Core *core, int layer);

#endif
