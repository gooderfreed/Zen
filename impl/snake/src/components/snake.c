#include "../../inc/snake.h"

// =========================
//  DECK METHODS
// =========================
/*
 * Generate apple position
 * Randomly places an apple on the grid, avoiding the snake's body.
 */
static Coords generate_apple(const Snake *snake) {
    Coords apple;
    bool valid;

    do {
        valid = true;
        apple.x = (rand() % (SNAKE_GRID_WIDTH - BORDER_SIZE * 2)) + BORDER_SIZE;
        apple.y = (rand() % (SNAKE_GRID_HEIGHT - BORDER_SIZE * 2)) + BORDER_SIZE;

        for (int i = 0; i < snake->length; i++) {
            if (apple.x == snake->segments[i].x &&
                apple.y == snake->segments[i].y) {
                valid = false;
                break;
            }
        }
    } while (!valid);

    return apple;
}

/*
 * Set snake movement direction
 * Changes the direction of the snake, preventing 180-degree turns.
 */
static void set_snake_direction(Snake *snake, Coords new_direction) {
    if (snake->direction.x + new_direction.x != 0 || 
        snake->direction.y + new_direction.y != 0) {
        snake->direction = new_direction;
    }
}




// =========================
//  DRAWABLE INTERFACE IMPLEMENTATION
// =========================
/*
 * Render the snake on the screen
 * Draws the snake, apple, and game area using colored characters.
 */
static void print_snake(const void *snake_pointer, Screen *screen, const Cursor *cursor) {
    (void)cursor;
    Snake *snake = (Snake *)snake_pointer;

    // Draw game area
    fill_area(screen, SNAKE_AREA_OFFSET_Y + BORDER_SIZE, BORDER_SIZE, 
              SNAKE_AREA_HEIGHT, SNAKE_AREA_WIDTH, ' ', ((Color){0x002fa82d}), COLOR_WHITE);

    // Draw apple
    fill_area(screen, SNAKE_AREA_OFFSET_Y + BORDER_SIZE + snake->apple.y * SNAKE_GRID_CELL_SIZE, 
              BORDER_SIZE + snake->apple.x * SNAKE_GRID_CELL_SIZE * 2, 
              SNAKE_GRID_CELL_SIZE, SNAKE_GRID_CELL_SIZE * 2, L'█', COLOR_NONE, COLOR_RED);

    // Draw snake body
    for (int i = 1; i < snake->length; i++) {
        if (i % 2 == 0) {
            fill_area(screen, SNAKE_AREA_OFFSET_Y + BORDER_SIZE + snake->segments[i].y * SNAKE_GRID_CELL_SIZE, 
                      BORDER_SIZE + snake->segments[i].x * SNAKE_GRID_CELL_SIZE * 2, 
                      SNAKE_GRID_CELL_SIZE, SNAKE_GRID_CELL_SIZE * 2, L'█', COLOR_NONE, ((Color){0x00cc9752}));
        }
        else {
            fill_area(screen, SNAKE_AREA_OFFSET_Y + BORDER_SIZE + snake->segments[i].y * SNAKE_GRID_CELL_SIZE, 
                      BORDER_SIZE + snake->segments[i].x * SNAKE_GRID_CELL_SIZE * 2, 
                      SNAKE_GRID_CELL_SIZE, SNAKE_GRID_CELL_SIZE * 2, L'█', COLOR_NONE, ((Color){0x00dea357}));
        }
    }

    // Draw snake head
    fill_area(screen, SNAKE_AREA_OFFSET_Y + BORDER_SIZE + snake->segments[0].y * SNAKE_GRID_CELL_SIZE, 
              BORDER_SIZE + snake->segments[0].x * SNAKE_GRID_CELL_SIZE * 2, 
              SNAKE_GRID_CELL_SIZE, SNAKE_GRID_CELL_SIZE * 2, L'█', COLOR_NONE, ((Color){0x005c584f}));
}




// =========================
//  UPDATABLE INTERFACE IMPLEMENTATION
// =========================
/*
 * Update snake movement and handle collisions
 * Moves the snake, checks for collisions, and handles apple consumption.
 */
static void update_snake(void *snake_pointer, void *context) {
    (void)context;
    Snake *snake = (Snake *)snake_pointer;

    if (!snake->is_alive || (snake->direction.x == 0 && snake->direction.y == 0)) {
        return;
    }

    // Calculate new head position
    Coords tmp = {
        .x = snake->segments[0].x + snake->direction.x,
        .y = snake->segments[0].y + snake->direction.y
    };

    // Wrap around screen edges
    if (tmp.x < 0) tmp.x = SNAKE_GRID_WIDTH - 1;
    else if (tmp.x >= SNAKE_GRID_WIDTH) tmp.x = 0;

    if (tmp.y < 0) tmp.y = SNAKE_GRID_HEIGHT - 1;
    else if (tmp.y >= SNAKE_GRID_HEIGHT) tmp.y = 0;

    // Check for self-collision
    for (int i = 1; i < snake->length - 1; i++) {
        if (tmp.x == snake->segments[i].x && tmp.y == snake->segments[i].y) {
            snake->is_alive = false;
            return;
        }
    }

    // Check if apple is eaten
    bool ate_apple = (tmp.x == snake->apple.x && tmp.y == snake->apple.y);
    if (ate_apple) {
        snake->length++;
        if (snake->length < SNAKE_MAX_SIZE) {
            snake->apple = generate_apple(snake);
        } else {
            // we win, but for now just "freeze" snake
            snake->is_alive = false;
        }
    }

    // Move snake segments
    for (int i = snake->length - 1; i > 0; i--) {
        snake->segments[i] = snake->segments[i - 1];
    }

    snake->segments[0] = tmp;
}




// =========================
//  INPUT HANDLER INTERFACE IMPLEMENTATION
// =========================
/*
 * Handle snake movement input
 * Updates the snake's direction based on player input.
 */
static bool input_snake(void *snake_pointer, Core *core, wint_t key) {
    (void)core;
    Snake *snake = (Snake *)snake_pointer;

    switch (key) {
        case L'w': case L'ц': set_snake_direction(snake, SNAKE_UP);   break;
        case L's': case L'ы': set_snake_direction(snake, SNAKE_DOWN); break;
        case L'a': case L'ф': set_snake_direction(snake, SNAKE_LEFT); break;
        case L'd': case L'в': set_snake_direction(snake, SNAKE_RIGHT); break;
        default: break;
    }

    return false;
}




/*
 * Initialize snake object
 * Allocates memory, sets initial position, size, and direction.
 * Also initializes interfaces for rendering, updating, and input handling.
 */
Snake *init_snake(Arena *arena) {
    // Allocate memory for snake structure
    Snake *snake = (Snake *)arena_alloc(arena, sizeof(Snake));
    *snake = (Snake) {
        .direction = SNAKE_STAY,
        .length    = 4,
        .is_alive  = true
    };

    // Allocate memory for snake segments
    snake->segments = (Coords *)arena_alloc(arena, sizeof(Coords) * SNAKE_MAX_SIZE);
    snake->segments[0] = (Coords){ .x = 10, .y = 10 };
    snake->segments[1] = (Coords){ .x = 11, .y = 10 };
    snake->segments[2] = (Coords){ .x = 12, .y = 10 };
    snake->segments[3] = (Coords){ .x = 13, .y = 10 };

    // Generate initial apple position
    snake->apple = generate_apple(snake);



    // Define rendering behavior
    static Drawable drawable = {
        .is_active = true,
        .print = print_snake
    };

    // Define update behavior
    static Updateable updateable = {
        .update = update_snake
    };

    // Define input handling behavior
    static InputHandler input_handler = {
        .handle_input = input_snake
    };
    


    // Assign interfaces for core integration
    snake->interfaces = (ObjectInterfaces) {
        .name          = "Snake",
        .capabilities = {
            .is_drawable     = true,
            .requires_update = true,
            .requires_core   = true,
            .requires_input  = true
        },
        .drawable      = &drawable,
        .updateable    = &updateable,
        .input_handler = &input_handler,
    };

    return snake;
}
