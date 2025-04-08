#include "../inc/donut.h"
#include <math.h>


// =========================
//  DRAWABLE INTERFACE IMPLEMENTATION
// =========================
/*
 * Draw donut on screen
 */
static void print_donut(const void *donut_pointer, Screen *screen, const Cursor *cursor) {
    (void)cursor;
    Donut *donut = (Donut *)donut_pointer;
    static float A;
    static float B;
    if (A == donut->A && B == donut->B) return;

    A = donut->A;
    B = donut->B;

    int bufferSize = DONUT_WIDTH * DONUT_HEIGHT;
    float z_buffer[bufferSize];
    float theta, phi;

    float sinA = sinf(A);
    float cosA = cosf(A);
    float sinB = sinf(B);
    float cosB = cosf(B);

    fill_area(screen, BORDER_OFFSET_Y, BORDER_OFFSET_X, SCREEN_HEIGHT - BORDER_OFFSET_Y * 2, SCREEN_WIDTH - BORDER_OFFSET_X * 2, ' ', COLOR_BLACK, COLOR_RED);
    memset(z_buffer, 0, (size_t)bufferSize * sizeof(float)); // Fill with 0

    // --- ADJUSTED SAMPLING RATES ---
    float thetaStep = 0.0075f;
    float phiStep   = 0.0075f; 

    for (theta = 0; theta < (float)(2 * PI); theta += thetaStep) {
        float sinTheta = sinf(theta);
        float cosTheta = cosf(theta);

        float f_g = sinTheta * cosA;
        float f_e = sinTheta * sinA;
        float h_g = cosTheta * sinB;
        float h = cosTheta + 2.1f;

        for (phi = 0; phi < (float)(2 * PI); phi += phiStep) {
            float sinPhi = sinf(phi);
            float cosPhi = cosf(phi);

            float c_h = sinPhi * h;
            float c_d = sinPhi * cosTheta;
            float l_h = cosPhi * h;

            float D = 1 / (float)(c_h * sinA + f_g + 4.4f); // Inverse distance
            float t = c_h * cosA - f_e;

            // --- ADJUSTED PROJECTION ---
            // 1. Calculate the scaled x and y coordinates.
            float scaledX = D * (l_h * cosB - t * sinB);
            float scaledY = D * (l_h * sinB + t * cosB);

            // 2. Scale by donutWidth/Height and THEN center.
            int x = (int)((DONUT_WIDTH  / 2.0f) * scaledX + (DONUT_WIDTH  / 2.0f));
            int y = (int)((DONUT_HEIGHT / 2.0f) * scaledY + (DONUT_HEIGHT / 2.0f));

            int o = x + DONUT_WIDTH * y;
            float N = ((f_e - c_d * cosA) * cosB - c_d * sinA - f_g - cosPhi * h_g);

            // --- BOUNDS CHECKING ---
            if (y >= 0 && y < DONUT_HEIGHT && x >= 0 && x < DONUT_WIDTH && D > z_buffer[o]) {
                z_buffer[o] = D;
                
                float normalizedN = N > 0 ? N : 0;

                float brightnessF = (normalizedN) * (180.0f);
                brightnessF = powf(brightnessF / 255.0f, 2.2f) * 255.0f; // Gamma correction
                int brightness = (int)brightnessF; // Convert back to integer
                Color grayColor = (Color){0x00000000 | (uint32_t)(brightness << 16) | (uint32_t)(brightness << 8) | (uint32_t)(brightness << 0)};

                fill_area(screen, BORDER_OFFSET_Y + y, BORDER_OFFSET_X + x*2, 1, 2, ' ', grayColor, COLOR_NONE);
            }
        }
    }
}




// =========================
//  UPDATABLE INTERFACE IMPLEMENTATION
// =========================
/*
 * Update Donut and handles rotation
 */
static void update_donut(void *donut_pointer, void *context) {
    (void)context;
    Donut *donut = (Donut *)donut_pointer;

    donut->A += 0.04f;
    donut->B += 0.02f;
}




/*
 * Initialize donut
 * This function initializes the donut object
 */
static Donut *init_donut(Arena *arena) {
    Donut *donut = (Donut *)arena_alloc(arena, sizeof(Donut));

    donut->A = 0;
    donut->B = 0;

    INTERFACES(arena, donut, {
        DRAWABLE(print_donut);
        UPDATEABLE(update_donut);
    });

    return donut;
}




/*
 * Prepare screen
 */
static void layer_prepare_screen(Screen *screen) {
    (void) screen;
    add_borders(screen, 0, 0, SCREEN_HEIGHT, SCREEN_WIDTH, COLOR_BLUE, COLOR_WHITE, L"-|++++++");
}

static void layer_loop(Zen *zen, wint_t key) {
    switch (key) {
        case L'q': case L'й': zen_shutdown(zen); exit(0); break;
    }
}

/*
 * Main function
 * This function initializes the core, screen, map, and other necessary components.
 * It also sets up the game loop and handles user input.
 */
int main(void) {
    // Initialize the memory arena
    Arena *arena = arena_new_dynamic(1024*1024);
    // Initialize the zen framework
    Zen  *zen  = zen_init(arena);

    // Initialize the screen
    Screen *screen = init_screen(arena, SCREEN_WIDTH, SCREEN_HEIGHT, COLOR_BLACK, COLOR_WHITE, ' ');
    zen_set_screen(zen, screen);
    
    // Initialize the map
    Map *map = init_map(arena, 1, (Coords){.x = 0, .y = 0, .z = 0});
    
    // Initialize the map layer
    MapLayer *layer = NULL;
    MAP_LAYER(arena, layer, {
        prepare_screen = layer_prepare_screen;
        loop = layer_loop;
    }, {
        OBJECT(init_donut(arena), COORDS(0, 0));
    });

    // Set the map layer
    map_set_layer(map, layer, 0);
    // Set the map in the core
    zen_set_map(zen, map);


    // Set the target FPS
    zen_set_target_fps(zen, 30);
    // Set the ticks per second
    zen_set_ticks_per_second(zen, 30);
    // Enable FPS stats
    zen_enable_fps_stats(zen);
    // Show FPS
    zen_show_fps(zen, true);

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

    // Return 0 to indicate successful execution
    return 0;
}
