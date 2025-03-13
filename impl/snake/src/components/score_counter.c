#include "../../inc/snake.h"

/*
 * Print score on screen
 * Displays the current and max score in the score area.
 */
static void print_score(const void *score_pointer, Screen *screen, const Cursor *cursor) {
    (void)cursor;

    ScoreCounter *score_counter = (ScoreCounter *)score_pointer;
    fill_area(screen, BORDER_SIZE, BORDER_SIZE, SCORE_COUNER_HEIGHT, SNAKE_AREA_WIDTH, ' ', COLOR_BLUE, COLOR_NONE);
    
    char score_buffer[50];
    snprintf(score_buffer, 50, "Score: %i", score_counter->cur_score);
    insert_text(screen, BORDER_SIZE, BORDER_SIZE, score_buffer, COLOR_WHITE, COLOR_NONE, Effect_Bold);

    char score_record_buffer[50];
    int len = snprintf(score_record_buffer, 50, "Record: %i", score_counter->max_score);
    insert_text(screen, BORDER_SIZE, SCREEN_WIDTH - BORDER_SIZE - len, score_record_buffer, COLOR_WHITE, COLOR_NONE, Effect_Bold);
}

/*
 * Update score
 * Increases the current score by the value of the context.
 */
static void score_update_callback(void *score_pointer, void *context) {
    ScoreCounter *score_counter = (ScoreCounter *)score_pointer;
    score_counter->cur_score += *(int *)context;
    if (score_counter->cur_score > score_counter->max_score) {
        score_counter->max_score = score_counter->cur_score;
    }
}

/*
 * Initialize score counter
 * Allocates and initializes a new score counter in the given memory arena.
 */
ScoreCounter *init_score_counter(Arena *arena) {
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

    OBSERVER(arena, score_counter, {
        NEW_OBSERVER("score_update", score_update_callback);
    });

    return score_counter;
}
