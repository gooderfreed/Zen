/* 
 * Core engine header - defines interfaces and base structures for card games
 * This is the main interface file that all implementations must include
 */
#ifndef CORE_H
#define CORE_H

#define _POSIX_C_SOURCE 199309L

#include <fcntl.h>
#include <sys/select.h>
#include <stdbool.h>
#include <wchar.h>
#include <time.h>
#include <termios.h>
#include <unistd.h>
#include <stdio.h>
#include <locale.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>
#include <stdint.h>

#include "interfaces/interfaces.h"
#include "components/components.h"

/*
 * Key definitions
 * Common keyboard input codes
 */
#define KEY_ESC L''
#define KEY_ENTER 10
#define KEY_SPACE 32
#define KEY_CTRL_A 1
#define KEY_CTRL_D 4


typedef enum {
    INPUT_TYPE_CURSOR,
    INPUT_TYPE_DIRECT
} InputType;

/*
 * Core structure
 * Main game engine structure containing all necessary components
 */
typedef struct Core {
    Cursor      *cursor;     // cursor
    Screen      *screen;     // screen
    Arena       *arena;      // arena allocator
    Map         *map;        // map
    InputType   input_type;
    TimeManager time_manager;
    TickCounter tick_counter;
    FrameTimer  frame_timer;
} Core;


/*
 * Core functions
 * Main engine functionality
 */
Core *core_init(Arena *arena);
void core_set_map(Core *core, Map *map);
void core_manage_loop(Core *core, wint_t key);
void core_set_cursor(Core *core, Cursor *cursor);
void core_set_screen(Core *core, Screen *screen);
void core_shutdown(Core *core);
void core_local_move(Core *core, Coords move);
void core_action(Core *core);
void core_update(Core *core);
void core_update_screen(Core *core);
void core_global_move(Core *core, Coords move);
void core_free(Core *core);
void core_change_layer(Core *core, int layer);
void core_set_target_fps(Core *core, int fps);
bool core_should_close(Core *core);
bool core_has_input(void);
void core_enable_fps_stats(Core *core);
void core_disable_fps_stats(Core *core);
void core_show_fps(Core *core, bool state);
void core_set_ticks_per_second(Core *core, int ticks_per_second);

Screen *core_get_screen(Core *core);

#endif
