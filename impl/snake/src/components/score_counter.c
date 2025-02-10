#include "../../inc/snake.h"

/*
 * Print score on screen
 * Displays the current and max score in the score area.
 */
static void print_score(const void *score_pointer, Screen *screen, const Cursor *cursor) {
    (void)cursor;

    ScoreCounter *score_counter = (ScoreCounter *)score_pointer;
    fill_area(screen, BORDER_SIZE, BORDER_SIZE, SCORE_COUNER_HEIGHT, SNAKE_AREA_WIDTH, ' ', COLOR_BLUE, COLOR_RESET);
    char buffer[50];
    snprintf(buffer, 50, "Score: %i", score_counter->cur_score);
    int len = snprintf(buffer, 50, "Record: %i", score_counter->max_score);
    insert_text(screen, BORDER_SIZE, BORDER_SIZE, buffer, COLOR_BOLD, COLOR_NONE);
    insert_text(screen, BORDER_SIZE, SCREEN_WIDTH - BORDER_SIZE - len, buffer, COLOR_BOLD, COLOR_NONE);
}

/*
 * Initialize score counter
 * Allocates and initializes a new score counter in the given memory arena.
 */
ScoreCounter *init_score_conter(Arena *arena) {
    ScoreCounter *score_counter = (ScoreCounter *)arena_alloc(arena, sizeof(ScoreCounter));

    *score_counter = (ScoreCounter) {
        .cur_score = 0,
        .max_score = 0,
    };

    static Drawable drawable = {
        .is_active = true,
        .print = print_score
    };

    score_counter->interfaces = (ObjectInterfaces) {
        .name          = "Score Counter",
        .capabilities = {
            .is_drawable = true,
        },
        .drawable      = &drawable,
    };

    return score_counter;
}
