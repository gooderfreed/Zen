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


    // Initialize the drawable object
    static Drawable drawable = (Drawable) {
        .is_active = true,
        .print = print_demo,
    };


    // Initialize the demo's interfaces
    demo->interfaces = (ObjectInterfaces) {
        .name = "Demo",
        .capabilities = {
            .is_drawable = true,
        },
        .drawable = &drawable,
    };

    // Return the initialized demo object
    return demo;
}




/*
 * Prepare screen
 */
static void prepare_screen(Screen *screen) {
    add_borders(screen, 0, 0, 10, 10, COLOR_BLUE, COLOR_WHITE, L"-|++++++");
}


/*
 * Main loop
 * This function handles user input and updates the core
 */
static void loop(Core *core, wint_t key) {
    switch (key) {
        case L'q': case L'й': core_shutdown(core); exit(0); break;
    }
}

/*
 * Main function
 * This function initializes the core, screen, map, and other necessary components.
 * It also sets up the game loop and handles user input.
 */
int main(void) {
    // Initialize the memory arena
    Arena *arena = arena_new_dynamic(1024*10);
    // Initialize the core
    Core  *core  = core_init(arena);

    // Initialize the screen
    Screen *screen = init_screen(arena, 10, 10, COLOR_BLACK, COLOR_WHITE, ' ');
    core_set_screen(core, screen);
    
    // Initialize the map
    Map *map = init_map(arena, 1, (Coords){.x = 0, .y = 0, .z = 0});
    
    // Initialize the map layer
    MapLayer *layer = create_map_layer(arena, 1, 1, (Coords){.x = 0, .y = 0});
    layer->prepare_screen = prepare_screen;
    layer->layer_loop = loop;
    layer->objects[0][0].object = init_demo(arena);

    // Set the map layer
    map_set_layer(map, layer, 0);
    // Set the map in the core
    core_set_map(core, map);


    // Set the target FPS
    core_set_target_fps(core, 10);
    // Set the ticks per second
    core_set_ticks_per_second(core, 10);
    // Enable FPS stats
    core_enable_fps_stats(core);
    // Show FPS
    core_show_fps(core, true);

    // Game loop
    while (!core_should_close(core)) {
        // Handle user input
        if (core_has_input()) {
            wint_t ch = getwchar();

            // Shutdown the core if the user presses ESC
            if (ch == KEY_ESC) {
                core_shutdown(core);
                break;
            }

            // Manage the game loop
            core_manage_loop(core, ch);
        }
    }

    return 0;
}
