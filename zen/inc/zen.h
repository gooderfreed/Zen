#ifndef ZEN_H
#define ZEN_H

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
 * Zen structure
 * Main framework structure containing all necessary components
 */
typedef struct Zen {
    Cursor      *cursor;     // cursor
    Screen      *screen;     // screen
    Arena       *arena;      // arena allocator
    Map         *map;        // map
    InputType   input_type;
    TimeManager time_manager;
    TickCounter tick_counter;
    FrameTimer  frame_timer;
} Zen;


/*
 * Zen functions
 * Main framework functionality
 */
Zen *zen_init(Arena *arena);
void zen_set_map(Zen *zen, Map *map);
void zen_manage_loop(Zen *zen, wint_t key);
void zen_set_cursor(Zen *zen, Cursor *cursor);
void zen_set_screen(Zen *zen, Screen *screen);
void zen_shutdown(Zen *zen);
void zen_local_move(Zen *zen, Coords move);
void zen_action(Zen *zen);
void zen_update(Zen *zen);
void zen_update_screen(Zen *zen);
void zen_global_move(Zen *zen, Coords move);
void zen_free(Zen *zen);
void zen_change_layer(Zen *zen, int layer);
void zen_set_target_fps(Zen *zen, int fps);
bool zen_should_close(Zen *zen);
bool zen_has_input(void);
void zen_enable_fps_stats(Zen *zen);
void zen_disable_fps_stats(Zen *zen);
void zen_show_fps(Zen *zen, bool state);
void zen_set_ticks_per_second(Zen *zen, int ticks_per_second);

Screen *zen_get_screen(Zen *zen);

#endif
