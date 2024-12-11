/*
 * Copyright 2024 Cheban Sergiy
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
*/

/* * Core engine header - defines interfaces and base structures for card games
 * This is the main interface file that all implementations must include
 */

#ifndef CORE_H
#define CORE_H

#include <stdbool.h>
#include <wchar.h>

/*
 * Dynamic memory configuration
 * Define DYNAMIC to enable all dynamic memory features
 */
#ifdef DYNAMIC
    #define CONTAINER_DYNAMIC
    #define SCREEN_DYNAMIC
    #define MAP_DYNAMIC
#endif

// Base game structures
// -------------------

// primitive types
typedef struct Screen Screen;
typedef struct Cursor Cursor;
typedef struct Card Card;

/*
 * Coords - Basic 3D coordinate structure
 * Used for positioning objects in the game space
 */
typedef struct Coords {
    short y;
    short x;
    short z;
} Coords;

/*
 * Card - Base card structure that can be extended by implementations
 * Default implementation provides only coordinates
 */
#ifndef CARD_HEIGHT
    #define CARD_HEIGHT 1
#endif
#ifndef CARD_WIDTH
    #define CARD_WIDTH 1
#endif

#ifndef CARD_STRUCT
typedef struct Card {
    Coords coords;
} Card;
#endif

/*
 * CardsContainer - Container for card operations
 * Used for moving cards between objects and tracking their source
 */
#ifndef CONTAINER_DYNAMIC
#ifndef CONTAINER_SIZE
    #define CONTAINER_SIZE 1
#endif
#endif

typedef struct Container {
    int size;
    void *source;
    #ifndef CONTAINER_DYNAMIC
        void *container[CONTAINER_SIZE];
    #else
        int length;
        void **container;
    #endif
} Container;

#ifndef CUSTOM_CONTAINER_IMPL
    void container_add_element(Container *container, void *element);
    void container_clear_container(Container *container);
    bool container_is_empty(Container *container);
    void container_set_source(Container *container, void *source);
    void *container_pop_element(Container *container);
    void *container_get_element(Container *container, int index);
    #ifndef CONTAINER_DYNAMIC
        Container container_init(void);
    #else
        Container container_init(int length);
        void container_free(Container *container);
    #endif
#endif

// Interface definitions
// -------------------

/*
 * Drawable - Interface for objects that can be drawn on screen
 * Implementations must provide print method
 */
typedef struct {
    void (*print)(void *, Screen *, const Cursor *);
} Drawable;

/*
 * Interactable - Interface for objects that can be interacted with
 * Provides cursor placement and movement capabilities
 */
typedef struct {
    void (*place_cursor)(void *, Coords, Coords *);
    void (*move)(void *, Coords *, Coords);
} Interactable;

/*
 * CardHandler - Interface for objects that can work with cards
 * Provides full card manipulation capabilities
 */
typedef struct {
    bool can_give_cards : 1;
    bool can_take_cards : 1;

    void (*select_cards)(void *self, Coords, Container *);
    void (*get_cards)(void *self, Container *);
    void (*place_cards)(void *self, Coords, Container *);
    bool (*can_place)(void *self, Coords, Container *);
    bool (*is_same_card)(void *self, Coords, Card *);
} CardHandler;

/*
 * ButtonHandler - Interface for objects that can handle buttons
 * Provides button position and handling capabilities
 */
#ifndef BUTTON_HANDLER_SIZE
    #define BUTTON_HANDLER_SIZE 1
#endif

typedef struct {
    Coords coords;
    void *context;
    void (*on_click)(void *self, void *context);
} Button;

typedef struct {
    #ifndef BUTTON_HANDLER_DYNAMIC
        Button *buttons[BUTTON_HANDLER_SIZE];
    #else
        Button **buttons;
        int length;
    #endif
    int buttons_count;
} ButtonHandler;

#ifndef CUSTOM_BUTTON_HANDLER_IMPL
    #ifdef BUTTON_HANDLER_DYNAMIC
        Button **buttons_init(int length)
    #endif
    void add_button(ButtonHandler *button_handler, Button *button);
    void remove_button(ButtonHandler *button_handler, int index);
    void set_button_context(ButtonHandler *button_handler, int index, void *context);
    Button *get_button(ButtonHandler *button_handler, int index);
    bool is_button(void *object, Coords coords);
    Button *get_button_by_coords(void *object, Coords coords);
    void handle_button(void *object, Coords coords);
#endif

/*
 * PositionHandler - Interface for objects that can handle position
 * Provides position restoration and saving capabilities
 */
typedef struct {
    Coords restore_coords;
    void (*restore_pos)(void *self, Coords *current_coords);
    void (*save_current_pos)(void *self, Coords current_coords);
} PositionHandler;

/*
 * Dynamic - Interface for objects that can be dynamically allocated
 * Provides free method
 */
typedef struct {
    void (*free)(void *self);
} Dynamic;

typedef struct {
    void *context;
    void (*update)(void *self, void *context);
} Updateable;

/*
 * ObjectInterfaces - Interface for objects that can be interacted with
 * Provides access to all interfaces and capabilities
 */
typedef struct ObjectInterfaces {
    char *name;
    struct {
        bool can_hold_cards  : 1;
        bool have_buttons    : 1;
        bool is_drawable     : 1;
        bool is_interactable : 1;
        bool is_dynamic      : 1;
        bool is_positionable : 1;
        bool requires_update : 1;
    } capabilities;

    const CardHandler     *card_handler;
    const Drawable        *drawable;
    const Interactable    *interactable;
    const Dynamic         *dynamic;
    ButtonHandler         *button_handler;
    PositionHandler       *position_handler;
    Updateable            *updateable;
} ObjectInterfaces;

// Interface macros
// ---------------

/*
 * Base interface access macros
 * Provide safe access to object interfaces and capabilities
 */
#define GET_INTERFACE(object, iface) \
    (((ObjectInterfaces*)(object))->iface)

#define HAS_CAPABILITY(object, cap) \
    (((ObjectInterfaces*)(object))->capabilities.cap)

/*
 * Specialized interface macros for each handler type
 * Provide clean syntax for common operations
 */

// Card Handler macros
#define CARD_HANDLER(object) \
    GET_INTERFACE(object, card_handler)

#define CAN_GIVE_CARDS(object) \
    (HAS_CAPABILITY(object, can_hold_cards) && CARD_HANDLER(object)->can_give_cards)

#define CAN_TAKE_CARDS(object) \
    (HAS_CAPABILITY(object, can_hold_cards) && CARD_HANDLER(object)->can_take_cards)

#define CAN_PLACE_CARDS(object, coords, container) \
    (CARD_HANDLER(object)->can_place(object, coords, container))

#define SELECT_CARDS(object, coords, container) \
    (CARD_HANDLER(object)->select_cards(object, coords, container))

#define IS_SAME_CARD(object, coords, card) \
    (CARD_HANDLER(object)->is_same_card(object, coords, card))

#define GET_CARDS(object, container) \
    (CARD_HANDLER(object)->get_cards(object, container))

#define PLACE_CARDS(object, coords, container) \
    (CARD_HANDLER(object)->place_cards(object, coords, container))

// Button Handler macros
#define BUTTON_HANDLER(object) \
    GET_INTERFACE(object, button_handler)

#define HAVE_BUTTONS(object) \
    (HAS_CAPABILITY(object, have_buttons) && BUTTON_HANDLER(object)->buttons_count > 0)


// Drawable macros
#define DRAW_HANDLER(object) \
    GET_INTERFACE(object, drawable)

#define DRAWABLE(object) \
    (HAS_CAPABILITY(object, is_drawable))

#define DRAW(object, screen, cursor) \
    (DRAW_HANDLER(object)->print(object, screen, cursor))

// Dynamic macros
#define DYNAMIC_HANDLER(object) \
    GET_INTERFACE(object, dynamic)

#define DYNAMIC(object) \
    (HAS_CAPABILITY(object, is_dynamic))

#define FREE(object) \
    (DYNAMIC_HANDLER(object)->free(object))

// Interactable macros
#define INTERACT_HANDLER(object) \
    GET_INTERFACE(object, interactable)

#define INTERACTABLE(object) \
    (HAS_CAPABILITY(object, is_interactable))

#define MOVE(object, coords, delta) \
    (INTERACT_HANDLER(object)->move(object, coords, delta))

#define PLACE_CURSOR(object, coords, base_coords) \
    (INTERACT_HANDLER(object)->place_cursor(object, coords, base_coords))

// Position Handler macros
#define POSITION_HANDLER(object) \
    GET_INTERFACE(object, position_handler)

#define POSITIONABLE(object) \
    (HAS_CAPABILITY(object, is_positionable))

#define RESTORE_POSITION(object, coords) \
    (POSITION_HANDLER(object)->restore_pos(object, coords))

#define SAVE_POSITION(object, coords) \
    (POSITION_HANDLER(object)->save_current_pos(object, coords))

#define GET_RESTORE_COORDS(object) \
    (POSITION_HANDLER(object)->restore_coords)

// Updateable macros
#define UPDATEABLE(object) \
    (HAS_CAPABILITY(object, requires_update))

#define UPDATE_HANDLER(object) \
    GET_INTERFACE(object, updateable)

#define UPDATE(object) \
    (UPDATE_HANDLER(object)->update(object, UPDATE_HANDLER(object)->context))

// Screen related definitions
// ------------------------

/*
 * Screen dimensions
 * Can be overridden by implementation
 */
#ifndef SCREEN_HEIGHT
    #define SCREEN_HEIGHT 1
#endif
#ifndef SCREEN_WIDTH
    #define SCREEN_WIDTH 1
#endif

/*
 * Terminal control macros
 * Basic terminal manipulation commands
 */
#define clear() wprintf(L"\033[H\033[J")
#define gotoxy(x,y) wprintf(L"\033[%d;%dH", (y), (x))
#define hide_cursor() wprintf(L"\033[?25l")
#define show_cursor() wprintf(L"\033[?25h")

/*
 * Color definitions
 * Basic color definitions for foreground and background
 */
typedef enum {
    COLOR_RESET,

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
} Color;

const char *get_foreground(Color color);
const char *get_background(Color color);

/*
 * Screen structure
 * Represents the game screen with background, foreground and actual data layers
 */
#ifndef CUSTOM_SCREEN
struct Screen {
    #ifdef SCREEN_DYNAMIC
        int height;
        int width;
        Color **background;
        wchar_t **data;
        Color **foreground;
    #else
        Color background[SCREEN_HEIGHT][SCREEN_WIDTH];  // Background colors/effects
        wchar_t data[SCREEN_HEIGHT][SCREEN_WIDTH];      // Actual characters
        Color foreground[SCREEN_HEIGHT][SCREEN_WIDTH];  // Foreground colors/effects
    #endif
};

/*
 * Screen functions
 * Screen manipulation and drawing
 */
Screen init_screen(Color background, Color foreground, wchar_t symbol);
void print_screen(const Screen *screen);
void add_separator(Screen *screen, int y, int x, Color background, Color foreground, wchar_t *borders);
void fill_area(Screen *screen, int y, int x, int height, int width, wchar_t symbol, Color background, Color foreground);
void add_borders(Screen *screen, int y, int x, int height, int width, Color background, Color foreground, const wchar_t *borders);
#endif

// Cursor related definitions
// ------------------------

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
 * Cursor movement vectors
 * Predefined movement directions for cursor
 */
#define CURSOR_UP    (Coords){.x = 0,  .y = -1}
#define CURSOR_DOWN  (Coords){.x = 0,  .y = 1}
#define CURSOR_LEFT  (Coords){.x = -1, .y = 0}
#define CURSOR_RIGHT (Coords){.x = 1,  .y = 0}
#define CURSOR_STAY  (Coords){.x = 0,  .y = 0}

/*
 * Cursor structure
 * Represents cursor state including position, selected cards and current object
 */
struct Cursor {
    Coords coords;          // Current cursor position
    Container *cards;        // Currently selected cards
    void *subject;          // Object under cursor
};

// Map related definitions
// ---------------------

/*
 * Map dimensions
 * Can be overridden by implementation
 */
#ifndef MAP_HEIGHT
    #define MAP_HEIGHT 1
#endif
#ifndef MAP_WIDTH
    #define MAP_WIDTH 1
#endif

/*
 * MapObject structure
 * Represents a single object on the map with its position
 */
typedef struct MapObject {
    void *object;           // Pointer to the actual object
    Coords default_coords;  // Default position for this object
} MapObject;

/*
 * Map structure
 * Represents the game map with objects and global position
 */
typedef struct Map {
    #ifdef MAP_DYNAMIC
        int height;
        int width;
        MapObject **objects;
    #else
        MapObject objects[MAP_HEIGHT][MAP_WIDTH];  // Grid of objects
    #endif
    Coords global_coords;                      // Global map position
} Map;

/*
 * Core structure
 * Main game engine structure containing all necessary components
 */
typedef struct {
    Cursor *cursor;  // Game cursor
    Screen *screen;  // Game screen
    Map *map;        // Game map
} Core;

// Validation related definitions
// ---------------------------

/*
 * Interface validator macro
 * Creates validator structure for interface checking
 */
#define VALIDATOR(cap_field, iface_field, val_func) \
    { \
        .capability_flag = interfaces->capabilities.cap_field, \
        .interface = interfaces->iface_field, \
        .validate = val_func \
    }

/*
 * Validation function type
 * Function pointer type for interface validators
 */
typedef bool (*ValidateFunc)(const void *interface, const char *name);

/*
 * Interface validator structure
 * Used to validate interface implementation
 */
typedef struct InterfaceValidator {
    bool capability_flag;         // Is this interface enabled
    const void *interface;        // Interface pointer
    ValidateFunc validate;        // Validation function
} InterfaceValidator;

// Function prototypes
// -----------------

/*
 * Core functions
 * Main engine functionality
 */
Core init_core(Map *map, Cursor *cursor, Screen *screen);
void core_local_move(Core *core, Coords move);
void core_action(Core *core);
void core_update(Core *core);
void core_update_screen(Core *core);
void core_global_move(Core *core, Coords move);
void core_validate_interfaces(Core *core);
void core_free(Core *core);

/*
 * Cursor functions
 * Cursor manipulation and rendering
 */
Cursor init_cursor(void *start_object, Coords start_coords, Container *cursor_cards);
void print_cursor(Cursor *cursor, Screen *screen);

/*
 * Map functions
 * Map manipulation and object access
 */
void map_move(Map *map, Coords move);
MapObject map_get_object(Map *map, Coords coords);
MapObject map_get_current_object(Map *map);

/*
 * Validation functions
 * Interface validation
 */
bool validate_object_interfaces(const ObjectInterfaces *interfaces);

#endif
