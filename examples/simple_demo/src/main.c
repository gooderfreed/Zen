#include "../inc/demo.h"

// =========================
//  DRAWABLE INTERFACE IMPLEMENTATION
// =========================
/*
 * Draw demo on screen
 */
static void print_demo(const void *demo_pointer, Screen *screen, const Cursor *cursor) {
    (void)cursor;
    Demo *demo = (Demo *)demo_pointer;

    insert_text(screen, 3, 3, "Hello",               COLOR_RED,    COLOR_BLACK, Effect_Blink);
    insert_text(screen, 4, 3, "from",                COLOR_RED,    COLOR_BLACK, Effect_Underline);
    insert_text(screen, 5, 3, demo->interfaces.name, COLOR_GREEN,  COLOR_BLACK, Effect_Italic);
    insert_text(screen, 6, 1, demo->my_string,       COLOR_YELLOW, COLOR_BLACK, Effect_Bold);
}




/*
 * Initialize demo
 * This function initializes the demo object
 */
static Demo *init_demo(Arena *arena) {
    // Allocate memory for the demo object
    Demo *demo = (Demo *)arena_alloc(arena, sizeof(Demo));

    // Set the demo's string
    demo->my_string = "Mystring";

    // Initialize the demo's interfaces
    INTERFACES(arena, demo, {
        DRAWABLE(print_demo);
    });

    // Return the initialized demo object
    return demo;
}




/*
 * Prepare screen
 */
static void layer_prepare_screen(Screen *screen) {
    add_borders(screen, 0, 0, 10, 10, COLOR_BLUE, COLOR_WHITE, L"-|++++++");
}


/*
 * Main loop
 * This function handles user input and updates the core
 */
static void layer_loop(Zen *zen, wint_t key) {
    switch (key) {
        case L'q': case L'й': zen_shutdown(zen); exit(0); break;
    }
}

/*
 * Main function
 * This function initializes the zen, screen, map, and other necessary components.
 * It also sets up the game loop and handles user input.
 */
#define ARENA_SIZE 4096
int main(void) {
    // Initialize the memory arena
    size_t buffer[ARENA_SIZE]; // 4KB | demo requires -3172- 3076 bytes
    Arena *arena = arena_new_static(buffer, ARENA_SIZE);
    // Initialize the zen framework
    Zen   *zen   = zen_init(arena);

    // Initialize the screen
    Screen *screen = init_screen(arena, 10, 10, COLOR_BLACK, COLOR_WHITE, ' ');
    zen_set_screen(zen, screen);
    
    // Initialize the map
    Map *map = init_map(arena, 1, (Coords){.x = 0, .y = 0, .z = 0});
    
    // Initialize the map layer
    MapLayer *layer = NULL;
    MAP_LAYER(arena, layer, {
        prepare_screen = layer_prepare_screen;
        loop = layer_loop;
    }, {
        OBJECT(init_demo(arena), COORDS(0, 0));
    });

    // Set the map layer
    map_set_layer(map, layer, 0);
    // Set the map in the core
    zen_set_map(zen, map);


    // Set the target FPS
    zen_set_target_fps(zen, 10);
    // Set the ticks per second
    zen_set_ticks_per_second(zen, 10);

    // Game loop
    while (!zen_should_close(zen)) {
        // Handle user input
        if (zen_has_input()) {
            wint_t ch = getwchar();

            // Shutdown the core if the user presses ESC
            if (ch == KEY_ESC) {
                zen_shutdown(zen);
                break;
            }

            // Manage the game loop
            zen_manage_loop(zen, ch);
        }
    }

    return 0;
}
