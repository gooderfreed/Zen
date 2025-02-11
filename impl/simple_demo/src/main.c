#include "../inc/demo.h"

static void print_demo(const void *demo_pointer, Screen *screen, const Cursor *cursor) {
    (void)cursor;
    Demo *demo = (Demo *)demo_pointer;

    insert_text(screen, 3, 3, "Hello", COLOR_BRIGHT_RED, COLOR_BLACK);
    insert_text(screen, 4, 3, "from", COLOR_BRIGHT_RED, COLOR_BLACK);
    insert_text(screen, 5, 3, demo->interfaces.name, COLOR_BRIGHT_GREEN, COLOR_BLACK);
}

static Demo *init_demo(Arena *arena) {
    Demo *demo = (Demo *)arena_alloc(arena, sizeof(Demo));

    static Drawable drawable = (Drawable) {
        .is_active = true,
        .print = print_demo,
    };

    demo->interfaces = (ObjectInterfaces) {
        .name = "Demo",
        .capabilities = {
            .is_drawable = true,
        },
        .drawable = &drawable,
    };

    return demo;
}

/*
 * Prepare screen
 */
static void prepare_screen(Screen *screen) {
    add_borders(screen, 0, 0, 10, 10, COLOR_BLUE, COLOR_BOLD, L"-|++++++");
}

static void loop(Core *core, wint_t key) {
    switch (key) {
        case L'q': case L'й': core_shutdown(core); exit(0); break;
    }
}

int main(void) {
    Arena *arena = arena_new_dynamic(1024*10);
    Core  *core  = core_init(arena);

    Screen *screen = init_screen(arena, 10, 10, COLOR_BLACK, COLOR_BOLD, ' ');
    core_set_screen(core, screen);
    
    Map *map = init_map(arena, 1, (Coords){.x = 0, .y = 0, .z = 0});
    
    MapLayer *layer = create_map_layer(arena, 1, 1, (Coords){.x = 0, .y = 0});
    layer->prepare_screen = prepare_screen;
    layer->layer_loop = loop;
    layer->objects[0][0].object = init_demo(arena);

    map_set_layer(map, layer, 0);
    core_set_map(core, map);


    core_set_target_fps(core, 10);
    core_set_ticks_per_second(core, 10);
    core_enable_fps_stats(core);
    core_show_fps(core, true);

    while (!core_should_close(core)) {
        if (core_has_input()) {
            wint_t ch = getwchar();

            if (ch == KEY_ESC) {
                core_shutdown(core);
                break;
            }

            core_manage_loop(core, ch);
        }
    }

    return 0;
}
